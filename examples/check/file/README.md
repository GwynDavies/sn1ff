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

Help detect files with "Set User ID" (SUID),  "Set Group ID" (SGID) set, or "world writable" permissions set - that could potentially pose privilege escalation risks


# CHECKS

## suid_sgid.sh

Check for files that have the Set UID (SUID) or Set GID (SGID) permission set.

### Issues

On Debian, there are fails for files in /usr/games and /usr/lib/games
* sudo dpkg -l game
  * See if all the "game" files can be removed ...
* sudo apt purge game*

* sudo apt purge circuslinux
* sudo apt purge bsdgames

* sudo apt autoremove


## world_writeable.sh

Check for files that have "world writeable" permissions set.

