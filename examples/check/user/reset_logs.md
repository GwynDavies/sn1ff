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

# HowTo: Clear or Remove Last Login History in Linux


From the following article you will learn how to clear or remove the last login information on a Linux server from the command line

On the Linux systems there are three standard commands that show the information about last logged in users: 

* last, 
* lastb, and 
* lastlog

The output of these commands include: login-name, last login time, IP address, port etc


And sometimes, to keep anonymity, it is required to erase last login history.

To check the last login history, including the history of login attempts that failed, run one of the commands from the table below:

```
    Command	Logfile	Description
    last        /var/log/wtmp           Lists successful login/logout history
    lastb       /var/log/btmp           Shows the bad login attempts
    lastlog     /var/log/lastlog        Shows the most recent login

```


# Remove Information About Last Logins

Now you already how to check the last login history in Linux, it is time to learn how to clear it

As you can see from the table, the information about the last logged in users, including failed attempts to login, is stored in the specific binary files that you can’t edit but can easily empty them

To remove all the information about the last logged in users, you have to clear the files where this information is kept:

```
#/bin/bash

sudo echo > /var/log/wtmp
sudo echo > /var/log/btmp
sudo echo > /var/log/lastlog
```


