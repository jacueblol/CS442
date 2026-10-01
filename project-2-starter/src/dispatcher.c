#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dispatcher.h"
#include "shell_builtins.h"
#include "parser.h"

/* Sentinel meaning "no file descriptor" (valid fds are always >= 0). */
#define NO_FD -1
#define NO_PID -1

/* Permissions for files created by > and >> (rw-r--r--). */
#define OUTPUT_FILE_MODE 0644

/* Exit status when a command cannot be executed (POSIX convention). */
#define EXIT_CMD_NOT_FOUND 127
#define DISPATCH_FAILURE -1

static void redirect_fd(int fd, int target_fd)
{
	if (fd == target_fd)
		return;

	if (dup2(fd, target_fd) < 0) {
		fprintf(stderr, "dup2: %s\n", strerror(errno));
		_exit(EXIT_FAILURE);
	}
	close(fd);
}

static int open_or_exit(const char *filename, int flags)
{
	int fd = open(filename, flags, OUTPUT_FILE_MODE);

	if (fd < 0) {
		fprintf(stderr, "%s: %s\n", filename, strerror(errno));
		_exit(EXIT_FAILURE);
	}
	return fd;
}

static void setup_output(struct command *cmd, int pipe_out)
{
	switch (cmd->output_type) {
	case COMMAND_OUTPUT_FILE_TRUNCATE:
		redirect_fd(open_or_exit(cmd->output_filename,
					 O_WRONLY | O_CREAT | O_TRUNC),
			    STDOUT_FILENO);
		break;
	case COMMAND_OUTPUT_FILE_APPEND:
		redirect_fd(open_or_exit(cmd->output_filename,
					 O_WRONLY | O_CREAT | O_APPEND),
			    STDOUT_FILENO);
		break;
	case COMMAND_OUTPUT_PIPE:
		redirect_fd(pipe_out, STDOUT_FILENO);
		break;
	case COMMAND_OUTPUT_STDOUT:
		break;
	}
}

static void run_child(struct command *cmd, int in_fd, int pipe_fds[2])
{
	/* The read end belongs to the next command, not this one. */
	if (pipe_fds[0] >= 0)
		close(pipe_fds[0]);

	if (in_fd >= 0)
		redirect_fd(in_fd, STDIN_FILENO);
	else if (cmd->input_filename)
		redirect_fd(open_or_exit(cmd->input_filename, O_RDONLY),
			    STDIN_FILENO);

	setup_output(cmd, pipe_fds[1]);

	execvp(cmd->argv[0], cmd->argv);

	/* execvp() only returns on failure. */
	fprintf(stderr, "%s: %s\n", cmd->argv[0], strerror(errno));
	_exit(EXIT_CMD_NOT_FOUND);
}

static int wait_for_child(pid_t pid)
{
	int status;

	if (waitpid(pid, &status, 0) < 0) {
		fprintf(stderr, "waitpid: %s\n", strerror(errno));
		return DISPATCH_FAILURE;
	}

	if (WIFSIGNALED(status))
		return 128 + WTERMSIG(status);

	return WEXITSTATUS(status);
}

static int dispatch_external_command(struct command *pipeline)
{
	struct command *cmd;
	pid_t last_pid = NO_PID;
	int in_fd = NO_FD;
	int rv;

	for (cmd = pipeline; cmd; ) {
		int pipe_fds[2] = { NO_FD, NO_FD };
		pid_t pid;

		if (cmd->output_type == COMMAND_OUTPUT_PIPE &&
		    pipe(pipe_fds) < 0) {
			fprintf(stderr, "pipe: %s\n", strerror(errno));
			break;
		}

		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "fork: %s\n", strerror(errno));
			if (pipe_fds[0] >= 0) {
				close(pipe_fds[0]);
				close(pipe_fds[1]);
			}
			break;
		}

		if (pid == 0)
			run_child(cmd, in_fd, pipe_fds);

		/* Parent: close fds now owned by the child. */
		if (in_fd >= 0)
			close(in_fd);
		if (pipe_fds[1] >= 0)
			close(pipe_fds[1]);
		in_fd = pipe_fds[0];

		if (cmd->output_type == COMMAND_OUTPUT_PIPE) {
			cmd = cmd->pipe_to;
		} else {
			last_pid = pid;
			cmd = NULL;
		}
	}

	if (in_fd >= 0)
		close(in_fd);

	rv = last_pid > 0 ? wait_for_child(last_pid) : DISPATCH_FAILURE;

	while (wait(NULL) > 0)
		;

	return rv;
}

static int dispatch_parsed_command(struct command *cmd, int last_rv,
				   bool *shell_should_exit)
{
	for (size_t i = 0; builtin_commands[i].name; i++) {
		if (!strcmp(builtin_commands[i].name, cmd->argv[0])) {
			return builtin_commands[i].handler(
				(const char *const *)cmd->argv, last_rv,
				shell_should_exit);
		}
	}

	return dispatch_external_command(cmd);
}

int shell_command_dispatcher(const char *input, int last_rv,
			     bool *shell_should_exit)
{
	int rv;
	struct command *parse_result;
	enum parse_error parse_error = parse_input(input, &parse_result);

	if (parse_error) {
		fprintf(stderr, "Input parse error: %s\n",
			parse_error_str[parse_error]);
		return DISPATCH_FAILURE;
	}

	if (!parse_result)
		return last_rv;

	rv = dispatch_parsed_command(parse_result, last_rv, shell_should_exit);
	free_parse_result(parse_result);
	return rv;
}
