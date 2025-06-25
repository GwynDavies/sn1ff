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

# Check if htop is installed
if ! command -v htop >/dev/null 2>&1; then
  echo "Error: htop is not installed."
  exit 1
fi

# Get list of PIDs for processes matching 'sn1ff' and not defunct
pids=$(pgrep -a '^sn1ff' | grep -v defunct | awk '{print $1}' | paste -sd,)

# Exit if no PIDs found
if [[ -z "$pids" ]]; then
  echo "No matching processes found."
  exit 0
fi

# Command to run in the terminal
cmd="htop -p $pids"

# Try common terminal emulators in order
for term in gnome-terminal xfce4-terminal konsole xterm tilix lxterminal mate-terminal; do
  if command -v $term >/dev/null 2>&1; then
    case $term in
      gnome-terminal)
        $term -- bash -c "$cmd; exec bash" &
        ;;
      xfce4-terminal | mate-terminal | tilix | lxterminal)
        $term --command="bash -c '$cmd; exec bash'" &
        ;;
      konsole)
        $term --noclose -e bash -c "$cmd" &
        ;;
      xterm)
        $term -e bash -c "$cmd; exec bash" &
        ;;
    esac
    exit 0
  fi
done

# Fallback if no terminal found
echo "Error: No supported terminal emulator found."
exit 1
