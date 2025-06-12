# LICENSE

MIT License

Copyright (c) 2025 Gwyn Davies

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights  
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      
copies of the Software, and to permit persons to whom the Software is          
furnished to do so, subject to the following conditions:                        

The above copyright notice and this permission notice shall be included in    
all copies or substantial portions of the Software.                            

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN      
THE SOFTWARE.


# PURPOSE

JournalD disk usage, and failed ssh/sudo authentication attempts in the last 24 hours.


# CHECKS

## disk_usage.sh (Requires GROUP membership for systemd-journal)

* Check user is a member of group systemd-journal, to perform the check,
* Check disk usage by JournalD,
  * Run journalctl and extract disk usage,
  * Check the size unit is not GB or TB - else consider usage too large,
  * Check the size if in MB, is less than 250.


## ssh_fail.sh (Requires GROUP membership for systemd-journal)

* Check user is a member of group systemd-journal, to perform the check,
* Check for failed ssh authentication attempts, in the last 24 hours.


## sudo_fail.sh (Requires GROUP membership for systemd-journal)

* Check user is a member of group systemd-journal, to perform the check,
* Check for failed sudo authentication attempts, in the last 24 hours.

# Setup

Add to group:
* sudo usermod -aG systemd-journal ```<your user>```
* The user will need to "log out/back in" for the group membership to take effect
