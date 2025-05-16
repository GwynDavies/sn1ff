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

# .----------------------------------------------------------------.
# |                                                                |
# | Guard to avoid double-sourcing                                 |
# |                                                                |
# '----------------------------------------------------------------'

[[ -n "${_SN1FF_LIB_LOADED:-}" ]] && return
_SN1FF_LIB_LOADED=1

# .----------------------------------------------------------------.
# |                                                                |
# | Configuration                                                  |
# |                                                                |
# '----------------------------------------------------------------'

ttl_values=$(sn1ff_conf -t)
if [ $? -ne 0 ]; then
  echo "Error: Failed to retrieve 'client_ttls' value." >&2
  exit 1
fi

# Split ttl_value into array by comma
IFS=',' read -r ALRT_TTL WARN_TTL OKAY_TTL NONE_TTL <<<"$ttl_values"

# .----------------------------------------------------------------.
# |                                                                |
# | Functions                                                      |
# |                                                                |
# '----------------------------------------------------------------'

sn_set_alrt_ttl() {
  local new_ttl="$1"
  if [[ "$new_ttl" =~ ^[0-9]+$ ]]; then
    ALRT_TTL="$new_ttl"
  else
    echo "INVALID TTL VALUE: MUST BE AN INTEGER - EXITING" >&2
    exit 1
  fi
}

sn_set_warn_ttl() {
  local new_ttl="$1"
  if [[ "$new_ttl" =~ ^[0-9]+$ ]]; then
    WARN_TTL="$new_ttl"
  else
    echo "INVALID TTL VALUE: MUST BE AN INTEGER - EXITING" >&2
    exit 1
  fi
}

sn_set_okay_ttl() {
  local new_ttl="$1"
  if [[ "$new_ttl" =~ ^[0-9]+$ ]]; then
    OKAY_TTL="$new_ttl"
  else
    echo "INVALID TTL VALUE: MUST BE AN INTEGER - EXITING" >&2
    exit 1
  fi
}

sn_set_none_ttl() {
  local new_ttl="$1"
  if [[ "$new_ttl" =~ ^[0-9]+$ ]]; then
    NONE_TTL="$new_ttl"
  else
    echo "INVALID TTL VALUE: MUST BE AN INTEGER - EXITING" >&2
    exit 1
  fi
}

sn_show_ttls() {
  echo "ALRT TTL is currently set to -> $ALRT_TTL <- minutes"
  echo "WARN TTL is currently set to -> $WARN_TTL <- minutes"
  echo "OKAY TTL is currently set to -> $OKAY_TTL <- minutes"
  echo "NONE TTL is currently set to -> $NONE_TTL <- minutes"
  echo ""
}

sn_append_first_header() {
  local message="$1"
  local sn_filename="$2"
  printf "%s\n\n" "$message" >>"$sn_filename" 2>&1
}

sn_append_header() {
  local message="$1"
  local sn_filename="$2"
  printf "\n\n%s\n\n" "$message" >>"$sn_filename" 2>&1
}

sn_append_message() {
  local message="$1"
  local sn_filename="$2"
  printf "%s\n" "$message" >>"$sn_filename" 2>&1
}

sn_append_trailer_line() {
  local sn_filename="$1"
  trailer_line="--------------------------------------------------------------------------------"
  printf "\n%s\n\n" "$trailer_line" >>"$sn_filename" 2>&1
}

sn_append_message_center() {
  local message="$1"
  local sn_filename="$2"

  # Calculate padding
  width=80
  pad=$(((width - ${#message}) / 2))

  # Print the padded string
  printf "%*s\n\n\n" $((pad + ${#message})) "$message" >>"$sn_filename" 2>&1
}

sn_exit_with_message() {
  local message="$1"
  local sn_filename="$2"
  local sn_status="$3"
  local sn_addr="$4"

  sn_append_trailer_line "$sn_filename"

  sn_append_message_center "$message" "$sn_filename"

  case "$sn_status" in
    "ALRT")
      sn_ttl=$ALRT_TTL
      ;;

    "WARN")
      sn_ttl=$WARN_TTL
      ;;

    "OKAY")
      sn_ttl=$OKAY_TTL
      ;;

    "NONE")
      sn_ttl=$NONE_TTL
      ;;

    *)
      echo "ERROR in STATUS ARGUMENT -> $sn_status <- EXITING ..."
      exit 1
      ;;
  esac

  if [ -z "$sn_user" ] && [ -z "$sn_addr" ]; then
    # Local client, as there is no user or addr for SCP
    sn1ff_client \
      -e \
      -f "$sn_filename" \
      -s "$sn_status" \
      -t "$sn_ttl"
  else
    # Remote client, as there is a user and addr for SCP
    sn1ff_client \
      -e \
      -f "$sn_filename" \
      -s "$sn_status" \
      -t "$sn_ttl" \
      -a "$sn_addr"
  fi

  # Exit Bash script
  if [ "$sn_status" = "OKAY" ]; then
    exit 0
  else
    exit 1
  fi
}
