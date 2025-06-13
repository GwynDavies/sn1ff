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

Check some SSHD settings for login, and password authentication.


# CHECKS

## settings.sh (Requires SUDO privilege)

Check:
* User has required sudo ability to perform check,
* Check SSHD settings: 
  * PermitRootLogin         = "no" ... root login is not allowed
  * PasswordAuthentication. = "no" ... authentication by public key

### Setup

#### Editing /etc/ssh/sshd_config

If you want this check script to pass "as is", you would need to have the following settings:

edit /etc/ssh/sshd_config

  * PermitRootLogin         ... set to **no**
  * PasswordAuthentication. ... set to **no**

If you do make changes to /etc/ssh/sshd_config (not ssh_config), you need to restart or reload the SSH daemon for the changes to take effect


* Reload (preferred if available):

```
sudo systemctl reload sshd
```

This reloads the configuration without dropping current connections


* Restart (fallback):

If for some reason "reload" above does not work or is not available:

```
sudo systemctl restart sshd
```

This restarts the SSH service, which briefly closes all SSH connections (use with care if you're logged in remotely)


#### Sudo privilege

See list of sudo entries in::
* examples/check/README.md
  * "## Granting sudo privileges" 

* sudo entry
```
# <sn1ff installation>/examples/check/net/sshd/settings.sh
<your user> ALL=(ALL) NOPASSWD: /usr/sbin/sshd -T  
```
