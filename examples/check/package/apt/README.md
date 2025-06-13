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

Check for waiting apt package upgrades, to help ensure systems are kept up-to-date with updates.


# CHECKS

## waiting_upgrades.sh (Requires SUDO privilege)

* Check user has required sudo ability to perform the check,
* Update the list of packages available form apt server
* Check if any of installed packages are now waiting to be upgraded

### Setup

#### Sudo privilege

See list of sudo entries in::
* examples/check/README.md
  * "## Granting sudo privileges"

* sudo entry
```
#<sn1ff installation>/examples/check/package/apt/updates_waiting.sh
<your user> ALL=(ALL) NOPASSWD: /usr/bin/apt update -qq
```
