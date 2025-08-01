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
# | Source required sn1ff bash libraries                           |
# |                                                                |
# '----------------------------------------------------------------'

source "$(dirname "$0")/lib/sn1ff_linux_lib.sh"

# .----------------------------------------------------------------.
# |                                                                |
# | Functions                                                      |
# |                                                                |
# '----------------------------------------------------------------'

display_help() {
  echo "Usage: $0 <OPTION>"
  echo
  echo "Options:"
  echo "  -n, --name SCRIPT_NAME    Run the specified script."
  echo "  -h, --help                Show this help message."
}

# Function to check if /usr/bin/sn1ff_monitor exists

has_sn1ff_monitor() {
  if [ -f "/usr/bin/sn1ff_monitor" ]; then
    return 0 # File exists, local execution
  else
    return 1 # File doesn't exist, remote execution
  fi
}

has_sn1ff_client() {
  if [ -f "/usr/bin/sn1ff_client" ]; then
    return 0 # File exists, local execution
  else
    return 1 # File doesn't exist, remote execution
  fi
}

is_sn1ff_server() {
  if has_sn1ff_monitor && has_sn1ff_client; then
    echo "IS SN1FF SERVER ..."
    return 0
  else
    echo "IS NOT SN1FF SERVER ..."
    return 1
  fi
}

is_sn1ff_client() {
  if ! has_sn1ff_monitor && has_sn1ff_client; then
    echo "IS SN1FF CLIENT ..."
    return 0
  else
    echo "IS NOT SN1FF CLIENT ..."
    return 1
  fi
}

script_exist() {
  local script_to_run="$1"

  # Check if the script exists

  if [ -f "$script_to_run" ]; then
    return 0
  else
    return 1
  fi
}

run_script_locally() {
  local script_to_run="$1"

  # Check group 'sn1ff' exists, and the user is a member

  REQUIRED_GROUP="sn1ff"

  (sn_linux_group_exists "$REQUIRED_GROUP")
  result=$?
  if [[ $result -ne 0 ]]; then
    echo "ERROR: Required group -> '$REQUIRED_GROUP' <- does not exist"
    exit 1
  fi

  (sn_linux_group_member "$REQUIRED_GROUP")
  result=$?
  if [[ $result -ne 0 ]]; then
    echo "ERROR: User is not a member of required group -> '$REQUIRED_GROUP' <-"
    exit 1
  fi

  # Run the script with no two arguments

  ./"$script_to_run"
}

run_script_remotely() {
  local script_to_run="$1"

  # Get IP or hostname of the remote sn1ff server

  SN_ADDR=$(sn1ff_conf -a)
  exit_code=$?
  if [[ $exit_code -ne 0 ]]; then
    echo "Error: Failed to retrieve 'client_ttls' value." >&2
    exit 1
  fi

  # Run the script

  ./"$script_to_run" "$SN_ADDR"
}

run_script() {
  local script_to_run="$1"

  # Run the script locally or remotely based on the execution type

  if is_sn1ff_server; then
    echo "Running '$script_to_run' locally."
    run_script_locally "$script_to_run"
  elif is_sn1ff_client; then
    echo "Running '$script_to_run' remotely."
    run_script_remotely "$script_to_run"
  else
    echo "ERROR: Could not determine if host is a sn1ff_server or sn1ff_client - exiting"
    exit 1
  fi
}

# .----------------------------------------------------------------.
# |                                                                |
# | Main                                                           |
# |                                                                |
# '----------------------------------------------------------------'

# Check if arguments are passed
if [ $# -eq 0 ]; then
  echo "Error: No aruments supplied"
  display_help
  exit 1
else
  # Check for help flag
  case "$1" in
    -h | --help)
      display_help
      ;;
    -n | --name)
      # Check if script name is provided
      if [ -z "$2" ]; then
        echo "Error: Missing script name after -n/--name"
        display_help
        exit 1
      fi
      run_script "$2"
      ;;
    *)
      echo "Error: Invalid option '$1'"
      display_help
      exit 1
      ;;
  esac
fi
