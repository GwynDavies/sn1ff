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

[[ -n "${_SN1FF_LINUX_LIB_LOADED:-}" ]] && return
_SN1FF_LINUX_LIB_LOADED=1

# .----------------------------------------------------------------.
# |                                                                |
# | Groups                                                         |
# |                                                                |
# '----------------------------------------------------------------'

# Check a Linux group exists
#
# @param $1  the group to check exists
#
# @return  0 if the group exists
#          1 if not
#
sn_linux_group_exists() {
  local the_group="$1"

  if getent group "$the_group" >/dev/null 2>&1; then
    # Group exists
    return 0
  else
    # Group does not exist
    return 1
  fi
}

## Check current user is a member of a group
#
# @param $1  the group to check user is a member of
#
# @return  0 if the user is a member
#          1 if not
sn_linux_group_member() {
  local the_group="$1"

  if groups | grep "$REQUIRED_GROUP" >/dev/null 2>&1; then
    # Current user is a member of group
    return 0
  else
    # Current user is not a member of group
    return 1
  fi
}

# .----------------------------------------------------------------.
# |                                                                |
# | Commands                                                       |
# |                                                                |
# '----------------------------------------------------------------'

# Get the full path of a command, using linux 'command'
#
# Example:
#   $ command -v sed
#   /usr/bin/sed
#
# @param the_command  is the command to get the full path for
#
# @output the full path of the command if found, else nothing
#
# @return 0  if the command was found
#         1  if the command was not found
#
sn_linux_cmd_path() {
  local the_command="$1"
  command_full_path=$(command -v "$the_command")

  # Check if the command was found
  if [ -z "$command_full_path" ]; then
    return 1
  else
    echo "$command_full_path"
  fi
  return 0
}

# .----------------------------------------------------------------.
# |                                                                |
# | Host                                                           |
# |                                                                |
# '----------------------------------------------------------------'

sn_linux_host_eq() {
  local to_hostname=$1
  echo "-> Test host_eq ${to_hostname}"
  [ "$(hostname)" = "${to_hostname}" ] && return 0 || return 1
}

# host_not_eq <to hostname>
sn_linux_host_neq() {
  local to_hostname=$1
  echo "-> Test host_not_eq ${to_hostname}"
  [ "$(hostname)" != "${to_hostname}" ] && return 0 || return 1
}

# host_in <to hostnames>
#   to_hostnames ... list of hostnames to check against, separate names by \n
#     hostname1
#     hostname1\n
#     hostname1\nhostname2\n
#     hostname1\nhostname2
sn_linux_host_in() {
  local to_hostnames=$1
  echo "-> Test host_in $to_hostnames"
  hostname | /usr/bin/grep -q -w -F -f <(echo -e "${to_hostnames}") && return 0 || return 1
}

# sn_host_nin <to hostnames>
#   to_hostnames ... list of hostnames to check against, separate names by \n
#     hostname1
#     hostname1\n
#     hostname1\nhostname2\n
#     hostname1\nhostname2
sn_linux_host_nin() {
  local to_hostnames=$1
  echo "-> Test host_not_in $to_hostnames"
  hostname | /usr/bin/grep -q -w -F -f <(echo -e "${to_hostnames}") && return 1 || return 0
}

# .----------------------------------------------------------------.
# |                                                                |
# | Distribution [Debian, Ubuntu etc.]                             |
# |                                                                |
# '----------------------------------------------------------------'

sn_linux_distro_detect() {
  if [ -f /etc/os-release ]; then
    . /etc/os-release
    echo "$ID"
  else
    echo "unknown"
  fi
}

sn_linux_distro_eq() {
  current_distro=$(sn_linux_distro_detect)
  local to_distro=$1
  echo "-> Test distro_eq ${to_distro} = $current_distro"
  [ "${current_distro}" = "${to_distro}" ] && return 0 || return 1
}

sn_linux_distro_neq() {
  current_distro=$(sn_linux_distro_detect)
  local to_distro=$1
  echo "-> Test distro_not_eq ${to_distro} = $current_distro"
  [ "${current_distro}" != "${to_distro}" ] && return 0 || return 1
}

# distro_in <to distros>
#   to_distros ... list of distros to check against, separate names by \n
#     distro1
#     distro1\n
#     distro1\ndistro2\n
#     distro1\ndistro2
sn_linux_distro_in() {
  local to_distros=$1
  echo "-> Test distro_in $to_distros"
  sn_linux_distro_detect | /usr/bin/grep -q -w -F -f <(echo -e "${to_distros}") && return 0 || return 1
}

# distro_not_in <to distros>
#   to_distros ... list of distros to check against, separate names by \n
#     distro1
#     distro1\n
#     distro1\ndistro2\n
#     distro1\ndistro2
sn_linux_distro_nin() {
  local to_distros=$1
  echo "-> Test distro_not_in $to_distros"
  sn_linux_distro_detect | /usr/bin/grep -q -w -F -f <(echo -e "${to_distros}") && return 1 || return 0
}

# .----------------------------------------------------------------.
# |                                                                |
# | systemd                                                        |
# |                                                                |
# '----------------------------------------------------------------'

# Checks the service exists, and is running
sn_linux_srvc_exists_and_running() {
  local service_name=$1
  if ! systemctl status "$service_name" &>/dev/null; then
    echo "Service $service_name does NOT exist or is NOT running"
    return 1
  else
    echo "Service $service_name EXISTS and IS running"
    return 0
  fi
}

sn_linux_srvc_enabled() {
  local service_name=$1
  if ! systemctl is-enabled "$service_name" &>/dev/null; then
    echo "Service $service_name is NOT enabled"
    return 1
  else
    echo "Service $service_name IS enabled"
    return 0
  fi
}

sn_linux_srvc_active() {
  local service_name=$1
  if ! systemctl is-active "$service_name" &>/dev/null; then
    echo "Service $service_name is NOT active"
    return 1
  else
    echo "Service $service_name IS active"
    return 0
  fi
}

sn_linux_srvc_not_failed() {
  local service_name=$1
  if systemctl is-failed "$service_name" &>/dev/null; then
    echo "Service $service_name IS failed"
    return 1
  else
    echo "Service $service_name is NOT failed"
    return 0
  fi
}

# .----------------------------------------------------------------.
# |                                                                |
# | sudo                                                           |
# |                                                                |
# '----------------------------------------------------------------'

# Check if the user has "no password" access to sudo
#
# @return 0 if user has passwordless access to sudo
#         1 if not
sn_linux_has_sudo_nopasswd() {
  if sudo -n true >/dev/null; then
    #echo "User has passwordless sudo access."
    return 0
  else
    #echo "User does NOT have passwordless sudo access."
    return 1
  fi
}

# Check if the user can "sudo" a command, without having to
# give a password
#
# @param  'cmd' is the (full path) command
# @return 0 if user can run the command with having to give a password
#         1 if not
sn_linux_can_sudo_cmd() {
  local cmd=$1
  echo "cmd->${cmd}<-"
  #if (sudo -n -l $cmd > /dev/null); then
  if (sudo -l | grep "$cmd" >/dev/null); then
    echo "You can run sudo cmd without a password."
    return 0
  else
    echo "You cannot run cmd without a password."
    return 1
  fi
}
