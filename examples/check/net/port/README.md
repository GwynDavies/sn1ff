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

Check for "unexpected" listening TCP ports.


# CHECKS

## listening.sh

* Check listening ports are expected, using the Linux "ss" command.

### Issues

You may find on Debian, that the "ipp" service is listening. This is the "cups" printing service

You can either allow it in the check script by, adding it as an "expected service/port:

```
  # Check for listening ports, expect "ssh"
  
  result=$(sn_net_tcp_listeners ssh)
                                   ^
                                   |
                               Add additional "expected" services/ports
  exit_code=$?
```


Alternatively, you can remove "cups" if you do not need it:

```
dpkg -l | grep cups
sudo systemctl stop cups
sudo systemctl disable cups
sudo apt purge cups*
```

