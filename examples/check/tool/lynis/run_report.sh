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
# 1. Requires sudo setup
#

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
  SN_ADDR=$1
  echo ""
  echo "$0"
  echo "$0 <$SN_ADDR>"
  echo ""
else
  echo "EXPECTED 0 OR 1 ARGUMENTS - $0  || $0 <ADDR>"
  exit 1
fi

# .----------------------------------------------------------------.
# |                                                                |
# | SETTINGS                                                       |
# |                                                                |
# '----------------------------------------------------------------'

CHECKID="LYNIS"
RUN_LYNIS_CMD="/usr/sbin/lynis audit system"
EXAMINE_LYNIS_RPT_CMD="/bin/cat /var/log/lynis-report.dat"

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

SN_FILENAME=$(sn1ff_client -b)
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
# | CHECK HAVE REQUIRED SUDO ABILITY TO RUN LYNIS                  |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_first_header "$CHECKID: CHECK HAVE REUIRED SUDO ABILITY TO RUN LYNIS" "$SN_FILENAME"

sn_linux_can_sudo_cmd "$RUN_LYNIS_CMD"
exit_code=$?

if [ $exit_code -ne 0 ]; then
  sn_exit_with_message "FAILED: CHECK HAVE REUIRED SUDO ABILITY TO RUN LYNIS" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | CHECK HAVE REQUIRED SUDO ABILITY TO VIEW REPORT                |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_first_header "$CHECKID: CHECK REUIRED HAVE SUDO ABILITY TO VIEW REPORT" "$SN_FILENAME"

sn_linux_can_sudo_cmd "$EXAMINE_LYNIS_REPORT_CMD"
exit_code=$?

if [ $exit_code -ne 0 ]; then
  sn_exit_with_message "FAILED: CHECK HAVE REQUIRED SUDO ABILITY TO VIEW REPORT" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | RUN SECURITY SCAN                                              |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_header "$CHECKID: RUN SECURITY SCAN" "$SN_FILENAME"

echo
echo "Running Lynis ..."
echo

(sudo $RUN_LYNIS_CMD >/dev/null 2>&1)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  sn_exit_with_message "FAILED: RUN SECURITY SCAN" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | CHECK SECURITY REPORT                                          |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_header "LYNIS: CHECK SECURITY REPORT" "$SN_FILENAME"

# Initialize arrays and variable

warnings=()
suggestions=()
hardening_index=""

# Avoid subshell: read via process substitution

while IFS= read -r line; do
  case "$line" in
    warning*)
      warnings+=("${line#warning=}")
      ;;
    suggestion*)
      suggestions+=("${line#suggestion=}")
      ;;
    hardening_index=*)
      hardening_index="${line#hardening_index=}"
      ;;
  esac
done < <(sudo $EXAMINE_LYNIS_RPT_CMD)

# Print summary

echo "===========================" >>"$SN_FILENAME"
echo "  Lynis Security Summary" >>"$SN_FILENAME"
echo "===========================" >>"$SN_FILENAME"

echo -e "\n Warnings:" >>"$SN_FILENAME"
for w in "${warnings[@]}"; do
  echo "- $w" >>"$SN_FILENAME"
done

echo -e "\nSuggestions:" >>"$SN_FILENAME"
for s in "${suggestions[@]}"; do
  echo "- $s" >>"$SN_FILENAME"
done

echo -e "\nSummary:" >>"$SN_FILENAME"
echo "Total Warnings   : ${#warnings[@]}" >>"$SN_FILENAME"
echo "Total Suggestions: ${#suggestions[@]}" >>"$SN_FILENAME"

# Check "hardening index > 50

if [[ -n "$hardening_index" ]]; then
  echo -e "\nHardening Index: $hardening_index" >>"$SN_FILENAME"
  if ((hardening_index <= 50)); then
    echo "ERROR: Hardening index is too low (≤ 50)." >>"$SN_FILENAME"
    sn_exit_with_message "FAILED: CHECK HARDENING INDEX > 50" "$SN_FILENAME" "ALRT" "$SN_ADDR"
  fi
else
  echo "Hardening index not found in report." >>"$SN_FILENAME"
  sn_exit_with_message "FAILED: CHECK HARDENING INDEX - NOT FOUND" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# Check there are no warnings

if ((${#warnings[@]} > 0)); then
  sn_exit_with_message "FAILED: CHECK NO WARNINGS" "$SN_FILENAME" "WARN" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | ALL CHECKS PASSED                                              |
# |                                                                |
# '----------------------------------------------------------------'

sn_exit_with_message 'ALL CHECKS PASSED' "$SN_FILENAME" "OKAY" "$SN_ADDR"
