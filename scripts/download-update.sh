#!/usr/bin/env bash

# Downloads the latest version of the dev container utilities. This does not update the dev
# container itself however. To update the dev container, you must rebuild it.

################################################################################
#                                  Internals                                   #
################################################################################

# Get the directory of the script
SCRIPT_DIR="$(dirname "$(readlink -f "${0}")")"

# Common functions and variables for scripts
. "${SCRIPT_DIR}/common.sh"

# Common preflight checks
preflight_checks skip-check-for-update

# Check that we should update the script
if ! semver_gt "${LATEST_VERSION}" "${CURRENT_VERSION}" || [ "${LATEST_VERSION}" == "${CURRENT_VERSION}" ]; then
  echo -e "${ERROR} The current version is already up to date."
  exit 1
fi

# Download the latest release zipball
curl -sSL "https://github.com/${REPOSITORY}/releases/latest/download/student-env.zip" -o /tmp/latest.zip

# Extract the latest release zipball
rm -rf /tmp/latest
unzip -qo /tmp/latest.zip -d /tmp/latest

# Run the new release's apply-update script
/tmp/latest/scripts/apply-update.sh "${CURRENT_VERSION}" "${ROOT_DIR}"
