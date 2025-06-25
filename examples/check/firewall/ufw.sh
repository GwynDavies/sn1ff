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
source "$SCRIPT_DIR/../../lib/sn1ff_firewall_ufw_lib.sh"

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

CHECKID="UFW"

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
# | CHECK UFW IS INSTALLED                                         |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_first_header "$CHECKID: CHECK UFW IS INSTALLED" "$SN_FILENAME"

sn_ufw_installed >>"$SN_FILENAME" 2>&1
exit_code=$?

if [ $exit_code -ne 0 ]; then
  sn_exit_with_message "FAILED: CHECK UFW IS INSTALLED" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | CHECK USER HAS REQUIRED SUDO ABILITY                           |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_header "$CHECKID: CHECK USER HAS REQUIRED SUDO ABILITY" "$SN_FILENAME"

sn_ufw_sudo_ability >>"$SN_FILENAME" 2>&1
exit_code=$?

if [ $exit_code -ne 0 ]; then
  sn_exit_with_message "FAILED: CHECK USER HAS REQUIRED SUDO ABILITY" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | GET UFW RULES DEFINED                                          |
# |                                                                |
# '----------------------------------------------------------------'

# Expected UFW rules

expected_rules="Status: active
Logging: on (low)
Default: deny (incoming), allow (outgoing), disabled (routed)
New profiles: skip

To                         Action      From
--                         ------      ----
22/tcp                     ALLOW IN    Anywhere                  
22/tcp (v6)                ALLOW IN    Anywhere (v6)             "

sn_append_header "$CHECKID: GET UFW RULES DEFINED" "$SN_FILENAME"

actual_rules=$(sn_ufw_rules)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  sn_exit_with_message "FAILED: GET UFW RULES DEFINED" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | CHECK UFW RULES MATCH EXPECTED VALUES                          |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_header "$CHECKID: CHECK RULES MATCH EXPECTED VALUES" "$SN_FILENAME"

diff_result=$(diff <(echo "$expected_rules") <(echo "$actual_rules"))
exit_code=$?

sn_append_message "$diff_result" "$SN_FILENAME"

if [ $exit_code -ne 0 ]; then
  sn_exit_with_message "FAILED: CHECK RULES MATCH EXPECTED VALUES" "$SN_FILENAME" "ALRT" "$SN_ADDR"
fi

# .----------------------------------------------------------------.
# |                                                                |
# | ALL CHECKS PASSED                                              |
# |                                                                |
# '----------------------------------------------------------------'

sn_exit_with_message 'ALL CHECKS PASSED' "$SN_FILENAME" "OKAY" "$SN_ADDR"
