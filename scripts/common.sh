#!/usr/bin/env bash

# Common functions and variables for scripts

################################################################################
#                                  Internals                                   #
################################################################################

# Exit on undefined variables and pipe failures
set -uo pipefail

# ANSI color codes
GRAY="\033[0;37m"
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RESET="\033[0m"

# Logging prefixes
DEBUG="${GRAY}[DEBUG]${RESET}"
SUCCESS="${GREEN}[SUCCESS]${RESET}"
WARNING="${YELLOW}[WARNING]${RESET}"
ERROR="${RED}[ERROR]${RESET}"

# Get the root directory of the dev container assets
ROOT_DIR="$(dirname "$(dirname "$(readlink -f "${0}")")")"

# The repository path on GitHub
REPOSITORY="csci-442-mines/student-env"

# Current release version
CURRENT_VERSION="v1.0.3"

# Latest release API URL
LATEST_URL="https://api.github.com/repos/${REPOSITORY}/releases/latest"

# Latest release version (Fallback to the current version)
LATEST_VERSION=$(curl -s "${LATEST_URL}" | jq --raw-output ".tag_name // \"${CURRENT_VERSION}\"")

# Compare two semantic versions
# Arguments:
#   - $1: The first version
#   - $2: The second version
# Returns:
#   - 0: If the first version is greater than the second version
#   - 1: If the first version is less than or equal to the second version
function semver_gt {
  if $(echo -e "$1\n$2" | sort -C -V); then
    return 1
  else
    return 0
  fi
}

# Common preflight checks
# Arguments:
#   - skip-check-for-update: Skip the check for a newer version of the script
# Returns:
#   None
function preflight_checks {
  # Check if the root directory is the workspace directory
  if [ "${ROOT_DIR}" != "/workspace" ]; then
      echo -e "${ERROR} This script appears to be in the wrong directory. This indicates that your student environment is misconfigured/corrupted. Please contact course staff for assistance."
      exit 1
  fi

  # Check for a newer version of the script
  if [ "${1}" != "skip-check-for-update" ] && semver_gt "${LATEST_VERSION}" "${CURRENT_VERSION}"; then
    echo -e "${WARNING} A newer version of the script is available (${CURRENT_VERSION} -> ${LATEST_VERSION}). Please run the download-update.sh script to update."
  fi
}
