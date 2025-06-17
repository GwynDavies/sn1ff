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

source "$(dirname "$0")/../../lib/sn1ff_lib.sh"

# Override default TTL values (minutes)

sn_set_none_ttl 2

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

CHECKID="DEBUG SN1FF MONITOR"

STATE_FILE_PROC="/var/tmp/sn1ff_mem_last_proc.txt"
STATE_FILE_SVC="/var/tmp/sn1ff_mem_last_svc.txt"
TMP_FILE_PROC="/var/tmp/sn1ff_mem_curr_proc.txt"

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
# | CHECK ON SN1FF SERVER                                          |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_first_header "$CHECKID: CHECK ON SN1FF SERVER" "$SN_FILENAME"

sn_append_message "Check we are on the sn1ff server, exit otherwise ..." "$SN_FILENAME"

if ! sn_is_sn1ff_server; then
  sn_exit_with_message "RESULT: NOT ON SN1FF SERVER - EXITING" "$SN_FILENAME" "NONE" "$SN_ADDR"
fi


# .----------------------------------------------------------------.
# |                                                                |
# | CHECK PROCESSES                                                |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_header "$CHECKID: CHECK PROCESSESS" "$SN_FILENAME"

sn_append_message "Collecting memory usage for sn1ff* processes..." "$SN_FILENAME"

# Clear tmp file

> "$TMP_FILE_PROC"

# Header

printf "%-8s %-10s %-10s %-10s %s\n" "PID" "RSS(KB)" "RSS(MB)" "DELTA(KB)" "CMD" >> "$SN_FILENAME"

# Get matching PIDs

PIDS=$(pgrep -f '^sn1ff')

if [[ -z "$PIDS" ]]; then
    sn_append_message "No matching sn1ff* processes found." "$SN_FILENAME"
else
    for pid in $PIDS; do
        # Get RSS and CMD
        read -r rss cmd <<< $(ps -o rss=,cmd= -p "$pid" --no-headers)
        rss=${rss:-0}
        cmd=${cmd:-unknown}
        rss_mb=$((rss / 1024))

        delta="N/A"
        if [[ -f "$STATE_FILE_PROC" ]]; then
            last_rss=$(grep "^$pid " "$STATE_FILE_PROC" | awk '{print $2}' || true)
            if [[ -n "$last_rss" ]]; then
                delta=$((rss - last_rss))
            else
                delta="new"
            fi
        fi

        printf "%-8s %-10s %-10s %-10s %s\n" "$pid" "$rss" "$rss_mb" "$delta" "$cmd" >> "$SN_FILENAME"

        echo "$pid $rss" >> "$TMP_FILE_PROC"
    done
fi

# .----------------------------------------------------------------.
# |                                                                |
# | CHECK SERVICE                                                  |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_header "$CHECKID: CHECK SERVICE" "$SN_FILENAME"

# Get sn1ff.service memory from systemctl show

sn_append_message "Get sn1ff.service memory from systemctl" "$SN_FILENAME"
svc_mem=$(systemctl show sn1ff.service -p MemoryCurrent | cut -d= -f2)

# Convert bytes to KB

svc_mem_kb=$((svc_mem / 1024))
svc_mem_mb=$((svc_mem_kb / 1024))

delta_svc="N/A"
if [[ -f "$STATE_FILE_SVC" ]]; then
    last_svc_mem=$(cat "$STATE_FILE_SVC")
    delta_svc=$((svc_mem_kb - last_svc_mem))
fi

sn_append_message "sn1ff.service memory: ${svc_mem_kb} KB (${svc_mem_mb} MB)" "$SN_FILENAME"
sn_append_message "sn1ff.service memory delta since last run: ${delta_svc} KB" "$SN_FILENAME"

# Save current states

mv "$TMP_FILE_PROC" "$STATE_FILE_PROC"
echo "$svc_mem_kb" > "$STATE_FILE_SVC"

sn_exit_with_message "RESULT: SN1FF MONITOR" "$SN_FILENAME" "NONE" "$SN_ADDR"

