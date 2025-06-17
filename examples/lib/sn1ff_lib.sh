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

if ! ttl_values=$(sn1ff_conf -t); then
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
  printf "Current TTL values (minutes):\n"
  printf "  ALRT %5s\n" $ALRT_TTL
  printf "  WARN %5s\n" $WARN_TTL
  printf "  OKAY %5s\n" $OKAY_TTL
  printf "  NONE %5s\n" $NONE_TTL
  printf "\n"
}

sn_append_titlebox() {
  local INPUT="$1"
  local FILE="$2"
  #local TERM_WIDTH=$(tput cols)
  local TERM_WIDTH=80

  # Add spaces between characters
  local SPACED_TEXT
  SPACED_TEXT=$(echo "$INPUT" | sed 's/./& /g' | sed 's/ $//')
  local TEXT_LENGTH=${#SPACED_TEXT}

  # Set box width with padding
  local BOX_WIDTH=$((TEXT_LENGTH + 4))
  if ((BOX_WIDTH > TERM_WIDTH)); then
    BOX_WIDTH=$((TERM_WIDTH - 2))
    SPACED_TEXT=$(echo "$SPACED_TEXT" | cut -c1-$((BOX_WIDTH - 4)))
    TEXT_LENGTH=${#SPACED_TEXT}
  fi

  # Centering calculations
  local H_PADDING=$(((TERM_WIDTH - BOX_WIDTH) / 2))
  local SPACES
  SPACES=$(printf '%*s' "$H_PADDING" "")

  local INNER_WIDTH=$((BOX_WIDTH - 2))
  local PAD_LEFT=$(((INNER_WIDTH - TEXT_LENGTH) / 2))
  local PAD_RIGHT=$((INNER_WIDTH - TEXT_LENGTH - PAD_LEFT))

  # ASCII-only box characters
  local CORNER_TL="."
  local CORNER_TR="."
  local CORNER_BL="'"
  local CORNER_BR="'"
  local HORIZONTAL="-"
  local VERTICAL="|"

  # Top border
  printf "%s%s" "$SPACES" "$CORNER_TL" >>"$FILE"
  printf "%${INNER_WIDTH}s" | tr ' ' "$HORIZONTAL" >>"$FILE"
  printf "%s\n" "$CORNER_TR" >>"$FILE"

  # Empty line
  printf "%s%s%${INNER_WIDTH}s%s\n" "$SPACES" "$VERTICAL" "" "$VERTICAL" >>"$FILE"

  # Centered text line
  printf "%s%s%*s%s%*s%s\n" \
    "$SPACES" "$VERTICAL" \
    "$PAD_LEFT" "" "$SPACED_TEXT" "$PAD_RIGHT" "" "$VERTICAL" >>"$FILE"

  # Empty line
  printf "%s%s%${INNER_WIDTH}s%s\n" "$SPACES" "$VERTICAL" "" "$VERTICAL" >>"$FILE"

  # Bottom border
  printf "%s%s" "$SPACES" "$CORNER_BL" >>"$FILE"
  printf "%${INNER_WIDTH}s" | tr ' ' "$HORIZONTAL" >>"$FILE"
  printf "%s\n\n" "$CORNER_BR" >>"$FILE"
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

  if [ -z "$sn_addr" ]; then
    # Local client, as there is no addr for SCP
    sn1ff_client \
      -e \
      -f "$sn_filename" \
      -s "$sn_status" \
      -t "$sn_ttl"
  else
    # Remote client, as there is an addr for SCP
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

# Functions to check if iwe are running on the sn1ff server, or a remote client

sn_has_sn1ff_monitor() {
if [ -f "/usr/bin/sn1ff_monitor" ]; then
  return 0 # File exists, local execution
else
  return 1 # File doesn't exist, remote execution
fi
}

sn_has_sn1ff_client() {
if [ -f "/usr/bin/sn1ff_client" ]; then
  return 0 # File exists, local execution
else
  return 1 # File doesn't exist, remote execution
fi
}

sn_is_sn1ff_server() {
if sn_has_sn1ff_monitor && sn_has_sn1ff_client; then
  echo "IS SN1FF SERVER ..."
  return 0
else
  echo "IS NOT SN1FF SERVER ..."
  return 1
fi
}

sn_is_sn1ff_client() {
if ! sn_has_sn1ff_monitor && sn_has_sn1ff_client; then
  echo "IS SN1FF CLIENT ..."
  return 0
else
  echo "IS NOT SN1FF CLIENT ..."
  return 1
fi
}

