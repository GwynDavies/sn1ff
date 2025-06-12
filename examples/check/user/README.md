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

Check for unrecognized users, or attempted unauthorized user activity


# CHECKS

## unknown_login.sh

* Get users having a "login" shell (not /usr/sbin/nologin|/bin/false|/bin/sync)',
* Alert if user not recognized.


## last_login.sh

* Use [last command](https://linux.die.net/man/1/last) to see the user logins/outs, 
* Alert on unrecognized user(s).


## failed_login.sh (Requires SUDO privilege)

* Use [lastb command](https://linux.die.net/man/1/lastb) command, to show failed login attempts.

### Setup

#### Sudo privilege

See list of sudo entries in::
* examples/check/README.md
  * "## Granting sudo privileges"

* sudo entry
```
# <sn1ff installation>/examples/check/user/failed_login.sh
<your user> ALL=(ALL) NOPASSWD: /usr/bin/lastb
```
