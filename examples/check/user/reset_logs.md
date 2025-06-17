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


