#!/usr/bin/python3

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

import subprocess
import os

def begin_chk_file():
  try:
    program_command = ['sn1ff_client', '-b']
    result = subprocess.run(program_command, capture_output=True, text=True, check=True)
    sn1ff_file = result.stdout.strip()  # Get the output and remove trailing newlines
    exit_status = result.returncode
    return exit_status, sn1ff_file
  except subprocess.CalledProcessError as e:
    exit_status = e.returncode
    sn1ff_file = e.stdout.strip() if e.stdout else e.stderr.strip()

def end_local_chk_file(sn1ff_file, status, ttl):
  try:
    program_command = ['sn1ff_client', '-e', '-f', sn1ff_file, '-s', status, '-t', str(ttl)]
    result = subprocess.run(program_command, capture_output=True, text=True, check=True)
    program_output = result.stdout.strip()  # Get the output and remove trailing newlines
    exit_status = result.returncode
    print(f"Successfully completed sn1ff check results file and sent to local sn1ff server\n  -> ${sn1ff_file}\n")
    return exit_status, sn1ff_file
  except subprocess.CalledProcessError as e:
    exit_status = e.returncode
    program_output = e.stdout.strip() if e.stdout else e.stderr.strip()

# .----------------------------------------------------------------.
# |                                                                |
# | Begin sn1ff check results file                                 |
# |                                                                |
# '----------------------------------------------------------------'

exit_status, sn1ff_file = begin_chk_file()

if exit_status != 0:
  print(f"ERROR: Cannot begin sn1ff check results file - status: {exit_status}")
  exit(exit_status)
else:
  print(f"sn1ff check results file created\n  -> ${sn1ff_file}\n")

# .----------------------------------------------------------------.
# |                                                                |
# | Append check output to sn1ff check results file                |
# |                                                                |
# '----------------------------------------------------------------'

with open(sn1ff_file, 'a') as file:
  file.write("PYTHON check -> CHECK TEST TEXT HERE\n")
        
# .----------------------------------------------------------------.
# |                                                                |
# | Rend sn1ff check result file, send to 'local host'             |
# |                                                                |
# '----------------------------------------------------------------'

status = 'OKAY'
ttl = 300
exit_status = end_local_chk_file(sn1ff_file, status, ttl)
