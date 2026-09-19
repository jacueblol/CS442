#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	FILE *input_file;
	FILE *output_file;
	char **lines;
	int num_lines;
	int capacity;
	char *line;
	size_t line_buffer_size;
	size_t line_length;
	size_t last_line_length;

	// Check that we got the right number of arguments.
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "usage: reverse <input> <output>\n");
		return 1;
	}

	// The input and output files can't be the same file.
	if (argc == 3 && strncmp(argv[1], argv[2], (size_t)-1) == 0) {
		fprintf(stderr, "error: input and output file must differ\n");
		return 1;
	}

	// Open the input file for reading.
	input_file = fopen(argv[1], "r");
	if (input_file == NULL) {
		fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
		return 1;
	}

	// Open the output file for writing, or use stdout if we only got
	// one argument.
	if (argc == 3) {
		output_file = fopen(argv[2], "w");
		if (output_file == NULL) {
			fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
			fclose(input_file);
			return 1;
		}
	} else {
		output_file = stdout;
	}

	num_lines = 0;
	capacity = 0;
	last_line_length = 0;
	line = NULL;
	line_buffer_size = 0;

	while ((line_length = getline(&line, &line_buffer_size, input_file)) != -1) {
		// Grow the array if it's full.
		if (num_lines == capacity) {
			if (capacity == 0)
				capacity = 16;
			else
				capacity = capacity * 2;

			char **new_lines = realloc(lines, capacity * sizeof(char *));
			if (new_lines == NULL) {
				fprintf(stderr, "error: memory allocation failed\n");
				free(line);
				for (int i = 0; i < num_lines; i++)
					free(lines[i]);
				free(lines);
				fclose(input_file);
				if (output_file != stdout)
					fclose(output_file);
				return 1;
			}
			lines = new_lines;
		}

		lines[num_lines] = line;
		num_lines = num_lines + 1;
		last_line_length = (size_t)line_length;

		line = NULL;
		line_buffer_size = 0;
	}
	free(line);

	if (num_lines > 1 && lines[num_lines - 1][last_line_length - 1] != '\n') {
		char *fixed_line = realloc(lines[num_lines - 1], last_line_length + 2);
		if (fixed_line == NULL) {
			fprintf(stderr, "error: memory allocation failed\n");
			for (int i = 0; i < num_lines; i++)
				free(lines[i]);
			free(lines);
			fclose(input_file);
			if (output_file != stdout)
				fclose(output_file);
			return 1;
		}
		fixed_line[last_line_length] = '\n';
		fixed_line[last_line_length + 1] = '\0';
		lines[num_lines - 1] = fixed_line;
	}

	if (fclose(input_file) != 0) {
		fprintf(stderr, "error: cannot close file '%s'\n", argv[1]);
		for (int i = 0; i < num_lines; i++)
			free(lines[i]);
		free(lines);
		if (output_file != stdout)
			fclose(output_file);
		return 1;
	}

	// Reverse the array of lines by swapping pairs from the outside in.
	for (int i = 0; i < num_lines / 2; i++) {
		char *temp = lines[i];
		lines[i] = lines[num_lines - 1 - i];
		lines[num_lines - 1 - i] = temp;
	}

	for (int i = 0; i < num_lines; i++) {
		if (fputs(lines[i], output_file) == EOF) {
			fprintf(stderr, "error: cannot write to output\n");
			for (int j = 0; j < num_lines; j++)
				free(lines[j]);
			free(lines);
			if (output_file != stdout)
				fclose(output_file);
			return 1;
		}
	}

	// Clean up.
	for (int i = 0; i < num_lines; i++)
		free(lines[i]);
	free(lines);

	if (output_file != stdout && fclose(output_file) != 0) {
		fprintf(stderr, "error: cannot close file '%s'\n", argv[2]);
		return 1;
	}

	return 0;
}
