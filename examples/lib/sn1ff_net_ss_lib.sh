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

[[ -n "${_SN1FF_NET_LIB_LOADED:-}" ]] && return
_SN1FF_NET_LIB_LOADED=1

# .----------------------------------------------------------------.
# |                                                                |
# | Functions                                                      |
# |                                                                |
# '----------------------------------------------------------------'

# Listening ports

# Use Linux 'ss' command, to find any unexpected listening TCP service(s)
# or port(s)
#
# Usage:
#
#   # Allow common services by service name (e.g. ssh) or port (e.g. :22)
#
#   if ! check_unexpected_tcp_listeners domain ipp ssh :80 :443; then
#     echo "Unexpected listening TCP service(s)/port(s) found"
#     exit 1
#   else
#     echo "No unexpected listening TCP services/ports found"
#   fi
#
#
#  # For no allowed service names or ports, call with no argument
#
#  if ! check_unexpected_tcp_listeners; then
#    echo "Unexpected listening TCP service(s)/port(s) found"
#    exit 1
#  else
#    echo "No unexpected listening TCP services/ports found"
#  fi`
#
#
# @param allowed_patters   are any allowed listening tcp service
#                          names or port numbers
#
# @ return  0 all listeners are allowed
# return    1 unexpected listeners found
#
sn_net_tcp_listeners() {
  local allowed_patterns=("$@")
  local pattern
  local sed_filter=''

  # Build a sed command to delete allowed lines

  for pattern in "${allowed_patterns[@]}"; do
    # Escape colons if needed (for port filters like ":80")
    sed_filter+="/${pattern//\//\\/}/d;"
  done

  # Run ss, filter out allowed patterns:
  #
  #  `ss`: Displays socket statistics
  #  `-H`: Suppress the header line
  #  `-l`: Show only listening sockets
  #  `-t`: Show TCP sockets
  #  `-p`: Show process using the socket
  #
  # So this lists all listening TCP sockets and the processes using them, no header
  #

  local output
  output=$(ss -Hltp 2>/dev/null | sed "${sed_filter}")

  # Output remaining lines for function caller to access

  if [[ -n "$output" ]]; then
    echo "$output"
    return 1 # Unexpected listeners found
  else
    return 0 # All listeners are allowed
  fi
}
