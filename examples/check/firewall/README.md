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

Access control.

Make sure the "Uncomplicated FireWall" (UFW) is running and configured as expected.


# CHECKS

## ufw.sh (Requires SUDO privilege)

Checks that:
* UFW is installed,
* User has required sudo ability to perform the checks,
* Get the currently defined UFW rule,
* Compares the defined rules, to expected values.

### Setup

#### Install/configure ufw

* sudo apt install ufw
* sudo systemctl status ufw

* sudo systemctl start ufw
* sudo ufw default deny incoming
* sudo ufw default allow outgoing
* sudo ufw allow ssh

* sudo ufw enable

#### Sudo privilege

See list of sudo entries in::
* examples/check/README.md
  * "## Granting sudo privileges"

* sudo entry
```
# <sn1ff installation>/examples/check/firewall/ufw.sh
<your user> ALL=(ALL) NOPASSWD: /usr/sbin/ufw status verbose
```
