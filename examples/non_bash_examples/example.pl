#!/usr/bin/perl

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

use strict;
use warnings;

# .----------------------------------------------------------------.
# |                                                                |
# | Begin sn1ff check results file                                 |
# |                                                                |
# '----------------------------------------------------------------'

my $sn1ff_file = `sn1ff_client -b`;
my $exit_status = $? >> 8;  # Extract the exit status from $? variable

if ($exit_status == 0) {
  chomp($sn1ff_file);  # Remove the trailing newline from the output
  print "sn1ff check results file created\n  -> $sn1ff_file\n\n";
} else {
  print "ERROR: Cannot begin sn1ff check results file - status: $exit_status";
  exit $exit_status;
}

# .----------------------------------------------------------------.
# |                                                                |
# | Append check output to sn1ff check results file                |
# |                                                                |
# '----------------------------------------------------------------'

open my $fh, '>>', $sn1ff_file or die "Cannot open file '$sn1ff_file': $!\n";
print $fh "PERL check -> CHECK TEST TEXT HERE\n\n";
close $fh;

# .----------------------------------------------------------------.
# |                                                                |
# | Rend sn1ff check result file, send to 'local host'             |
# |                                                                |
# '----------------------------------------------------------------'

my $status = 'OKAY';
my $ttl = 300;

`sn1ff_client -e -f $sn1ff_file -s $status -t $ttl`;
$exit_status = $? >> 8;  # Extract the exit status from $? variable
        
if ($exit_status == 0) {
    print "Successfully compelted sn1ff check results file and sent to local sn1ff server\n  -> $sn1ff_file\n\n";
} else {
    print "The program did not exit successfully. Exit status: $exit_status\n";
    exit $exit_status
}

