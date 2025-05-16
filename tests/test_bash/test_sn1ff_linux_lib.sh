#! /bin/bash

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
# | Uses shunit2
# |                                                                |
# '----------------------------------------------------------------'

# sudo apt install shunit2
# https://github.com/koalaman/shellcheck/wiki/

# .----------------------------------------------------------------.
# |                                                                |
# | Source required sn1ff bash libraries                           |
# |                                                                |
# '----------------------------------------------------------------'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../../examples/lib/sn1ff_linux_lib.sh"

# .----------------------------------------------------------------.
# |                                                                |
# | Test expected values                                           |
# |                                                                |
# '----------------------------------------------------------------'

EXISTING_GROUP=$(groups | awk '{print $1}')

EXISTING_COMMAND='bash'
EXISTING_COMMAND_PATH=$(command -v "$EXISTING_COMMAND")

CURRENT_DISTRO=$(grep -e '^ID=' /etc/os-release | sed s/ID=//)

# .----------------------------------------------------------------.
# |                                                                |
# | sn_linux_group_                                                |
# |                                                                |
# '----------------------------------------------------------------'

test_sn_linux_group_exists_1() {
  # T??: Linux group exists - use first group of current user
  output=$(sn_linux_group_exists "$EXISTING_GROUP")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_group_member_1() {
  # T??: Linux group member - use first group of current user
  output=$(sn_linux_group_member "$EXISTING_GROUP")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

# .----------------------------------------------------------------.
# |                                                                |
# | sn_linux_cmd_path                                              |
# |                                                                |
# '----------------------------------------------------------------'

test_sn_linux_cmd_path_1() {
  # T??: Existing linux command path
  output=$(sn_linux_cmd_path "$EXISTING_COMMAND")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
  assertEquals "output == /bin/bash" "$EXISTING_COMMAND_PATH" "$output"
}

test_sn_linux_cmd_path_2() {
  # T??: Non existing linux command path
  output=$(sn_linux_cmd_path bad)
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

# .----------------------------------------------------------------.
# |                                                                |
# | sn_linux_host_eq                                               |
# |                                                                |
# '----------------------------------------------------------------'

test_sn_linux_host_eq_MATCH() {
  output=$(sn_linux_host_eq "$(hostname)")
  exit_code=$?
  echo "output -> $output <-"
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_host_eq_NOT_MATCH() {
  output=$(sn_linux_host_eq bad)
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

# .----------------------------------------------------------------.
# |                                                                |
# | sn_linux_host_in                                               |
# |                                                                |
# '----------------------------------------------------------------'

test_sn_linux_host_in_1() {
  # T2a: Single hostname, no trailing \n
  output=$(sn_linux_host_in "$(hostname)")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_host_in_2() {
  # T2b: NOT found in single hostname, no trailing \n
  output=$(sn_linux_host_in bad)
  exit_code=$?
  assertEquals "exit_code == 0" 1 $exit_code
}

test_sn_linux_host_in_3() {
  # T3a: Found in single hostname, with trailing \n
  output=$(sn_linux_host_in "$(hostname)\n")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_host_in_4() {
  # T3b: NOT found in single hostname, with trailing \n
  output=$(sn_linux_host_in "bad\n")
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

# Double hostnames

test_sn_linux_host_in_5() {
  # T4a: Found in double hostnames, with current first
  output=$(sn_linux_host_in "$(hostname)\nbad\n")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_host_in_6() {
  # T4b: NOT found in double hostnames
  output=$(sn_linux_host_in "bad1\nbad2\n")
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

test_sn_linux_host_in_7() {
  # T5a: Found in double hostnames, with current second
  output=$(sn_linux_host_in "bad\n$(hostname)\n")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_host_in_8() {
  # T5b: Found in double hostnames, with current second and no trailing \n
  output=$(sn_linux_host_in "bad\n$(hostname)")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

# .----------------------------------------------------------------.
# |                                                                |
# | sn_linux_host_nin                                              |
# |                                                                |
# '----------------------------------------------------------------'

test_sn_linux_host_nin_1() {
  # T6a: Not found in single hostname, no trailing \n
  output=$(sn_linux_host_nin bad)
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_host_nin_2() {
  # T6b: FOUND in single hostname, no trailing \\n"
  output=$(sn_linux_host_nin "$(hostname)")
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

# .----------------------------------------------------------------.
# |                                                                |
# | sn_linux_distro_eq                                             |
# |                                                                |
# '----------------------------------------------------------------'

test_sn_linux_distro_eq_1() {
  # T7a: Match for Debian distro
  output=$(sn_linux_distro_eq "$CURRENT_DISTRO")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_distro_eq_2() {
  # T7b: NOT match for Debian distro
  output=$(sn_linux_distro_eq "bad{$CURRENT_DISTRO}")
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

# .----------------------------------------------------------------.
# |                                                                |
# | sn_linux_distro_in                                             |
# |                                                                |
# '----------------------------------------------------------------'

# Single distro

test_sn_linux_distro_in_1() {
  # T8a: Single distro, no trailing \n
  output=$(sn_linux_distro_in "$CURRENT_DISTRO")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_distro_in_2() {
  # T8b: NOT found in distro, no trailing \n
  output=$(sn_linux_distro_in bad)
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

test_sn_linux_distro_in_3() {
  # T8a: Found in single distro, with trailing \n
  output=$(sn_linux_distro_in "${CURRENT_DISTRO}\n")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_distro_in_4() {
  # T8b: NOT found in single distro, with trailing \n
  output=$(sn_linux_distro_in "bad\n")
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

# Double distros

test_sn_linux_distro_in_5() {
  # T8a: Found in double distros, with current first
  output=$(sn_linux_distro_in "${CURRENT_DISTRO}\nbad\n")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_distro_in_6() {
  # T8b: NOT found in double distros
  output=$(sn_linux_distro_in "bad1\nbad2\n")
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

test_sn_linux_distro_in_7() {
  # T8a: Found in double distros, with current second
  output=$(sn_linux_distro_in "bad\n${CURRENT_DISTRO}\n")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_distro_in_8() {
  # T8b: Found in double distros, with current second and no trailing \n
  output=$(sn_linux_distro_in "bad\n${CURRENT_DISTRO}")
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

# .----------------------------------------------------------------.
# |                                                                |
# | sn_linux_srvc_*                                                |
# |                                                                |
# '----------------------------------------------------------------'

test_sn_linux_srvc_installed_1() {
  # T??: systemd service exists
  output=$(sn_linux_srvc_installed systemd-journald)
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_srvc_installed_2() {
  # T??: systemd service not exist
  output=$(sn_linux_srvc_installed bad-systemd-journald)
  exit_code=$?
  assertEquals "exit_code == 1" 1 $exit_code
}

test_sn_linux_srvc_enabled_1() {
  # T??: systemd service enabled
  output=$(sn_linux_srvc_enabled systemd-journald)
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

test_sn_linux_srvc_active_1() {
  # T??: systemd service active
  output=$(sn_linux_srvc_active systemd-journald)
  exit_code=$?
  assertEquals "exit_code == 0" 0 $exit_code
}

# .----------------------------------------------------------------.
# |                                                                |
# | Load shunit2                                                   |
# |                                                                |
# '----------------------------------------------------------------'

# Ignore warning from shellcheck that shunit2 is not found
# shellcheck disable=SC1091
. shunit2
