#!/bin/bash

# MIT License
#
# Copyright (c) 2025 Gwyn Davies
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR,
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Set script safety - enables strict error handling:
#   -e (exit on error),
#   -u (unset variables),
#   -o pipefail (pipe errors)

set -euo pipefail
#set -x
trap 'echo "Script failed at line $LINENO with exit code $?" >&2' ERR
exec > >(tee -i script.log)
exec 2>&1

# .----------------------------------------------------------------.
# |                                                                |
# | SOURCE SN1FF BASH LIBRARIES                                    |
# |                                                                |
# '----------------------------------------------------------------'

# Get the directory of the current script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Source the library scripts from the 'lib' directory in the parent

source "$SCRIPT_DIR/../../lib/sn1ff_lib.sh"
source "$SCRIPT_DIR/../../lib/sn1ff_linux_lib.sh"

# Override default TTL values if desired (minutes)

#sn_set_alrt_ttl 120
#sn_set_warn_ttl 120
#sn_set_okay_ttl 120
#sn_set_none_ttl 120
#sn_show_ttls

# .----------------------------------------------------------------.
# |                                                                |
# | LOCAL FUNCTIONS                                                |
# |                                                                |
# '----------------------------------------------------------------'

# None

# .----------------------------------------------------------------.
# |                                                                |
# | PROCESS ARGUMENTS                                              |
# |                                                                |
# '----------------------------------------------------------------'

if [[ $# -eq 0 || $# -eq 1 ]]; then
  SN_ADDR="${1:-}"
  echo ""
  echo "$0"
  echo "$0 <$SN_ADDR>"
  echo ""
else
  echo "EXPECTED 0 OR 1 ARGUMENTS - $0 || $0 <ADDR>"
  exit 1
fi

# .----------------------------------------------------------------.
# |                                                                |
# | SETTINGS                                                       |
# |                                                                |
# '----------------------------------------------------------------'

CHECKID="FAILED LOGIN"

# .----------------------------------------------------------------.
# |                                                                |
# | BEGIN SN1FF FILE                                               |
# |                                                                |
# '----------------------------------------------------------------'

SN_FILENAME=$(sn1ff_client -b)
exit_code=$?

if [[ $exit_code -ne 0 ]]; then
  echo "ERROR: FAILED TO BEGIN SN1FF FILE - STATUS -> $exit_code SN_FILENAME -> SN_FILENAME"
  exit 1
fi

# .----------------------------------------------------------------.
# |                                                                |
# | DISPLAY BANNER                                                 |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_titlebox "$CHECKID" "$SN_FILENAME"

# .----------------------------------------------------------------.
# |                                                                |
# | CHECK HAS REQUIRED SUDO ABILITY                                |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_first_header "$CHECKID: CHECK HAS REQUIRED SUDO ABILITY" "$SN_FILENAME"

# Do I have sudo access to the lastb command
#/usr/bin/sudo -l | /usr/bin/grep "/usr/bin/lastb" >/dev/null
sn_linux_can_sudo_cmd "/usr/bin/lastb"

exit_code=$?

if [ $exit_code -ne 0 ]; then
  sn_append_message "Cannot perform -> sudo /usr/bin/lastb" "$SN_FILENAME"
  sn_exit_with_message "FAILED: CHECK HAS REQUIRED SUDO ABILITY" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | CHECK UKNOWN 'FAILED LOGIN'                                    |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_first_header "$CHECKID: CHECK UNKNOWN 'FAILED LOGIN'" "$SN_FILENAME"

# Define the exclude list with "reboot" initially

exclude_list=("btmp" "gduser")

# Initialize a flag to track if unexpected users are found

unexpected_users_found=false

# Run the last command and get the list of usernames

failed_output=$(sudo /usr/bin/lastb | awk '{print $1}' | sort | uniq)

# Check each username in the lastb command output

for username in $failed_output; do
  # Flag to check if username is in exclude list
  username_found=false

  # Loop through the exclude list to see if username matches
  for excluded_username in "${exclude_list[@]}"; do
    if [[ "$username" == "$excluded_username" ]]; then
      username_found=true
      break
    fi
  done

  # If the username is not in the exclude list, print the username and set the flag to true
  if ! $username_found; then
    echo "Error: Found unexpected username '$username'." >>"$SN_FILENAME" 2>&1
    unexpected_users_found=true
  fi
done

# Report the result based on the flag

if $unexpected_users_found; then
  echo "Error: Unexpected users found" >>"$SN_FILENAME" 2>&1
  sn_exit_with_message "FAILED: CHECK UNKNOWN 'FAILED LOGIN'" "$SN_FILENAME" "ALRT" "$SN_ADDR"
else
  echo "All usernames found were expected." >>"$SN_FILENAME" 2>&1
fi

# .----------------------------------------------------------------.
# |                                                                |
# | ALL CHECKS PASSED                                              |
# |                                                                |
# '----------------------------------------------------------------'

sn_exit_with_message 'ALL CHECKS PASSED' "$SN_FILENAME" "OKAY" "$SN_ADDR"
