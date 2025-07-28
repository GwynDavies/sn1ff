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

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <csv_file_path>"
  exit 1
fi

csv_file="$1"
temp_file="$(mktemp)"

if [[ ! -f "$csv_file" ]]; then
  echo "File not found: $csv_file"
  exit 1
fi

# Build allowed month regex (e.g., July|June|May)

months=$(paste -sd'|' <<<"$(
  date +%B
  date -d 'last month' +%B
  date -d '2 months ago' +%B
)")

# Regex: match if column 1 contains one of the months

regex="\"[A-Za-z]{3} ($months) [0-9]{1,2},"

# Read the CSV line by line
{
  read -r header
  echo "$header" >"$temp_file"

  while IFS= read -r line; do
    if [[ "$line" =~ $regex ]]; then
      echo "$line" >>"$temp_file"
    fi
  done
} <"$csv_file"

# Overwrite the original file with filtered content

mv "$temp_file" "$csv_file"
