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

source "$(dirname "$0")/../../../lib/sn1ff_lib.sh"

# Override default TTL values (minutes)

sn_set_okay_ttl 1

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

# Get script path/name, for the scripts "Check ID"

SCRIPT_PATH="$(realpath "$0")"
MARKER="check"
checkid="$(sn_get_path_name "$SCRIPT_PATH" "$MARKER")"
CHECKID="${checkid^^}"

# .----------------------------------------------------------------.
# |                                                                |
# | BEGIN SN1FF FILE                                               |
# |                                                                |
# '----------------------------------------------------------------'

SN_FILENAME=$(sn1ff_client -b -i "$checkid")
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
# | CHECK DISK USAGE                                               |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_first_header "$CHECKID: CHECK DISK USAGE" "$SN_FILENAME"

# Whitelist of mount points (partial match allowed)

WHITELIST=("/mnt/external" "/dev/lxd" "/opt/google/cros-containers" "/dev/.lxd-mounts" "/run/user" "/dev/shm" "/dev/.cros_milestone" "/dev/kvm")

# Disk usage threshold%

THRESHOLD=95
CHECK_FAILED=false

# Note: size, used and avail are is not used, from the output from "df"

while read -r filesystem _size _used _avail use_perc mount_point; do
  use_num=${use_perc%\%}

  skip=false
  for white in "${WHITELIST[@]}"; do
    if [[ "$mount_point" == *"$white"* ]]; then
      skip=true
      break
    fi
  done

  if ! $skip && ((use_num > THRESHOLD)); then
    echo "ERROR: Mount point $mount_point ($filesystem) is at ${use_perc} usage." >>"$SN_FILENAME"
    CHECK_FAILED=true
  fi
done < <(df -P | tail -n +2)

echo "" >>"$SN_FILENAME"

if [ "$CHECK_FAILED" = true ]; then
  echo "One or more disk usages are over ${THRESHOLD}% usage." >>"$SN_FILENAME"
  sn_exit_with_message "RESULT: CHECK DISK USAGE FAILED" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | ALL CHECKS PASSED                                              |
# |                                                                |
# '----------------------------------------------------------------'

echo "All non-whitelisted mount points are under ${THRESHOLD}% usage." >>"$SN_FILENAME"
sn_exit_with_message "RESULT: ALL CHECKS PASSED" "$SN_FILENAME" "OKAY" "$SN_ADDR"
