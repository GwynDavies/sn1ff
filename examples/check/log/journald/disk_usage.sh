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

# Notes:
#    GROUP - User must be member of "systemd-journal"

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

source "$SCRIPT_DIR/../../../lib/sn1ff_lib.sh"
source "$SCRIPT_DIR/../../../lib/sn1ff_linux_lib.sh"

# Override default TTL values if desired (minutes)

#sn_set_alrt_ttl 120
#sn_set_warn_ttl 120
#sn_set_okay_ttl 120
#sn_set_none_ttl 120
#sn_show_ttls

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

LINUX_CMD="/usr/bin/journalctl --disk-usage"
MAX_DISK_SIZE_MB=250

# .----------------------------------------------------------------.
# |                                                                |
# | LOCAL FUNCTIONS                                                |
# |                                                                |
# '----------------------------------------------------------------'

# None

# .----------------------------------------------------------------.
# |                                                                |
# | BEGIN SN1FF FILE                                               |
# |                                                                |
# '----------------------------------------------------------------'

SN_FILENAME=$(sn1ff_client -b -i "$checkid")
exit_code=$?

if [[ $exit_code -ne 0 ]]; then
  echo "ERROR: FAILED TO BEGIN SN1FF FILES - STATUS -> $exit_code SN_FILENAME -> SN_FILENAME"
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
# | CHECK USER HAS GROUP SYSTEMD-JOURNAL                           |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_first_header "$CHECKID: CHECK USER HAS GROUP SYSTEMD-JOURNAL" "$SN_FILENAME"

sn_linux_group_member "systemd-journal"
exit_code=$?

if [ $exit_code -ne 0 ]; then
  sn_exit_with_message "FAILED: CHECK USER HAS GROUP SYSTEMD-JOURNAL" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | CHECK JOURNALD DISK USAGE                                      |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_header "$CHECKID: CHECK DISK USAGE" "$SN_FILENAME"

# Run journalctl and extract disk usage line

disk_usage=$($LINUX_CMD)

echo "Disk usage ->${disk_usage}<-" >>"$SN_FILENAME"

# Extract the disk size - number and unit (e.g., 107.2 and M)

disk_size=$(echo "$disk_usage" | grep -oP '\d+(\.\d+)?(?=[KMGTP])')
disk_unit=$(echo "$disk_usage" | grep -oP '(?<=\d)([KMGTP])(?=)')

# Check for disallowed units 'G' and 'T' for GB and TB - size too large if in these units

if [[ "$disk_unit" == "G" || "$disk_unit" == "T" ]]; then
  echo "FAIL: journal size is TOO LARGE ($disk_size$disk_unit)" >>"$SN_FILENAME"
  sn_exit_with_message "FAILED: CHECK JOURNAL SIZE IS TOO LARGE ($disk_size$disk_unit)" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# Check size if unit is M

if [[ "$disk_unit" == "M" ]]; then
  size_int=$(printf "%.0f" "$disk_size")
  if ((size_int > MAX_DISK_SIZE_MB)); then
    echo "FAIL: journal size exceeds ${MAX_DISK_SIZE_MB}M ($disk_size$disk_unit)" >>"$SN_FILENAME"
    sn_exit_with_message "FAILED: CHECK JOURNAL SIZE EXCEEDS ${MAX_DISK_SIZE_MB}M ($disk_size$disk_unit)" "$SN_FILENAME" "ALRT" "$SN_ADDR"
  fi
fi

echo "OK: journal size is within limits ($disk_size$disk_unit)" >>"$SN_FILENAME"

# .----------------------------------------------------------------.
# |                                                                |
# | ALL CHECKS PASSED                                              |
# |                                                                |
# '----------------------------------------------------------------'

sn_exit_with_message 'ALL CHECKS PASSED' "$SN_FILENAME" "OKAY" "$SN_ADDR"
