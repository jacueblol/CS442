# CSCI442: Student Environment

[![release.yml](https://github.com/CSCI-442-Mines/student-env/actions/workflows/release.yml/badge.svg)](https://github.com/CSCI-442-Mines/student-env/actions/workflows/release.yml)

This is the student environment for CSCI442: Operating Systems at the
[Colorado School of Mines](https://mines.edu). If you are a student, please go to the
[course website](https://csci-442-mines.github.io/student-environment) for instructions on how to
set up your environment. The project structure is as follows:

- `/workspace/`: This is the directory of the dev container which is shared with your host machine.
  **Any data outside of this directory will not be saved when the container is stopped.**
  - `.devcontainer/`: This directory contains the configuration for the dev container itself. Do not
    modify this directory.
  - `scripts/`: This directory contains miscellaneous scripts for the course. A course staff member
    will provide instructions on how to use these utilities, if necessary.
  - `README.md`: This is the file you are currently reading. This file contains helpful reminders
    and instructions for using the dev container.
  - `SEMESTER-project-1-USERNAME/`: This is your personal project 1 repository, where `SEMESTER` is
    the current semester (e.g.: `sp25`) and `USERNAME` is your GitHub username (e.g.: `ghost`). It
    will be created for you when you accept the GitHub Classroom assignment for project 1
    (in Canvas).
  - `SEMESTER-project-2-USERNAME/`: Same as above, but for project 2.
  - `SEMESTER-project-3-USERNAME/`: Same as above, but for project 3.
  - `SEMESTER-project-4-USERNAME/`: Same as above, but for project 4.
  - `SEMESTER-project-5-USERNAME/`: Same as above, but for project 5.
