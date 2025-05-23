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

[[ -n "${_SN1FF_FIREWALL_UFW_LIB_LOADED:-}" ]] && return
_SN1FF_FIREWALL_UFW_LIB_LOADED=1

# .----------------------------------------------------------------.
# |                                                                |
# | Functions                                                      |
# |                                                                |
# '----------------------------------------------------------------'

sn_ufw_installed() {
  # Is UFW installed
  /usr/bin/systemctl status ufw
  return $?
}

sn_ufw_sudo_ability() {
  # Do I have sudo access to its command so I can check
  # its full status to see the rules configured
  /usr/bin/sudo -l | /usr/bin/grep "/usr/sbin/ufw status verbose"
  return $?
}

sn_ufw_rules() {
  # Check the full status and see if the expected rules
  # have been configured
  #
  # The user must have sudo ability in order to see the
  # current rules set on UFW
  /usr/bin/sudo /usr/sbin/ufw status verbose
  return $?
}
