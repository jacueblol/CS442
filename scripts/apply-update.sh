#!/usr/bin/env bash

# Apply the update. This script is automatically invoked by download-update.sh. Do not run this
# script manually!

################################################################################
#                                  Internals                                   #
################################################################################

# Get the directory of the script
SCRIPT_DIR="$(dirname "$(readlink -f "${0}")")"

# Common functions and variables for scripts
. "${SCRIPT_DIR}/common.sh"

# Validate arguments
if [ "$#" -ne 2 ]; then
  echo -e "${ERROR} Invalid number of arguments!"
  exit 1
fi

# Extract arguments
PREVIOUS_VERSION="${1}"
PREVIOUS_ROOT_DIR="${2}"

# Check if the previous version is empty
if [ -z "${PREVIOUS_VERSION}" ]; then
  echo -e "${ERROR} The previous version is empty!"
  exit 1
fi

# Check if the previous script directory is a directory
if [ ! -d "${PREVIOUS_ROOT_DIR}" ]; then
  echo -e "${ERROR} The previous script directory is not a directory!"
  exit 1
fi

# Version-specific update logic
if semver_gt "v2.0.0" "${PREVIOUS_VERSION}"; then
  # Copy the new scripts
  rsync --backup --checksum --recursive ${ROOT_DIR}/{*,.*} "${PREVIOUS_ROOT_DIR}"
else
  # Unknown version
  echo -e "${ERROR} Unknown previous version: ${PREVIOUS_VERSION}!"
  exit 1
fi

# Print success message
echo -e "${SUCCESS} Updated from ${PREVIOUS_VERSION} to ${CURRENT_VERSION}."
