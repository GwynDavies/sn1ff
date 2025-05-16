#!/usr/bin/env ruby

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

require 'open3'

def begin_chk_file
  program_command = ['sn1ff_client', '-b']
  stdout, stderr, status = Open3.capture3(*program_command)

  if status.success?
    sn1ff_file = stdout.strip
    return status.exitstatus, sn1ff_file
  else
    sn1ff_file = stdout.empty? ? stderr.strip : stdout.strip
    return status.exitstatus, sn1ff_file
  end
end

def end_local_chk_file(sn1ff_file, status_str, ttl)
  program_command = ['sn1ff_client', '-e', '-f', sn1ff_file, '-s', status_str, '-t', ttl.to_s]
  stdout, stderr, status = Open3.capture3(*program_command)

  if status.success?
    program_output = stdout.strip
    puts "Successfully completed sn1ff check results file and sent to local sn1ff server\n  -> #{sn1ff_file}\n\n"
    return status.exitstatus, sn1ff_file
  else
    program_output = stdout.empty? ? stderr.strip : stdout.strip
    return status.exitstatus, sn1ff_file
  end
end

# .----------------------------------------------------------------.
# | Begin sn1ff check results file                                 |
# '----------------------------------------------------------------'

exit_status, sn1ff_file = begin_chk_file

if exit_status != 0
  puts "ERROR: Cannot begin sn1ff check results file - status: #{exit_status}"
  exit(exit_status)
else
  puts "sn1ff check results file created\n  -> #{sn1ff_file}\n\n"
end

# .----------------------------------------------------------------.
# | Append check output to sn1ff check results file                |
# '----------------------------------------------------------------'

File.open(sn1ff_file, 'a') do |file|
  file.puts "RUBY check -> CHECK TEST TEXT HERE\n"
end

# .----------------------------------------------------------------.
# | End sn1ff check result file, send to 'local host'              |
# '----------------------------------------------------------------'

status = 'OKAY'
ttl = 300
exit_status, _ = end_local_chk_file(sn1ff_file, status, ttl)

