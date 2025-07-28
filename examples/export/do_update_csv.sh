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

# Check for required input

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <sn1ff_files_dir> <csv_file_path>"
  exit 1
fi

sn1ff_files_dir="$1"
csv_file="$2"

if [[ ! -f "$csv_file" ]]; then
  echo "File not found: $csv_file"
  exit 1
fi

# Process each file in order of oldest first

ls -tr "$sn1ff_files_dir"/*.snff 2>/dev/null | while read -r FILE; do
  BASENAME=$(basename "$FILE")

  # Extract GUID, STATE, TTL from the filename

  IFS='_' read -r GUID STATE TTL_WITH_EXT <<<"$BASENAME"
  TTL="${TTL_WITH_EXT%.snff}"

  # Initialize header values

  APP=""
  VER=""
  HOST=""
  IPV4=""
  AT=""
  CHECKID=""

  # Read file headers before the blank line

  while IFS= read -r LINE; do
    [[ -z "$LINE" ]] && break # stop at first blank line

    case "$LINE" in
      App:\ *) APP="${LINE#App: }" ;;
      Ver:\ *) VER="${LINE#Ver: }" ;;
      Host:\ *) HOST="${LINE#Host: }" ;;
      IPv4:\ *) IPV4="${LINE#IPv4: }" ;;
      At:\ *) AT="${LINE#At: }" ;;
      CheckID:\ *) CHECKID="${LINE#CheckID: }" ;;
    esac
  done <"$FILE"

  # Build CSV line

  CSV_LINE="\"$AT\",$HOST,\"$IPV4\",$STATE,\"$CHECKID\",$GUID,$TTL,$BASENAME,$APP,$VER"

  # Append to CSV

  echo "$CSV_LINE" >>"$csv_file"

  # Delete processed file

  rm -f "$FILE"
done
