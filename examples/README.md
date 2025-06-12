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


---

# OVERVIEW

This directory provides **examples** of sn1ff check scripts

They are:

* Primarily written in **bash** script (which sn1ff is mainly focused on)
* Located under the subdirectory **check**
* **Incomplete**, and are **just examples** - to demonstrate the *type* of functionality sn1ff offers

See their accompanying "README.md" files, in accompanying sub-directories


---

# RUNNING THE BASH SCRIPT EXAMPLES


## "run_*" scripts

The "run_*.sh" scripts are the "top level" scripts to run the example checks:
* run_debug.sh
* run_hourly.sh
* run_daily.sh


### "run_debug.sh"

Script "run_debug.sh" runs 4 bash check scripts, that I use for debugging and testing sn1ff

They are extremely simple, but show the basic *skeleton structure* of a sn1ff check script


## "run_chk.sh" script

The above bash scripts (run_debug.sh, run_hourly.sh, run_daily.sh), all rely on a single bash script called "run_chk.sh" - to run the individual check scripts


They do this by specifying:
* The **-n** option
* Followed by the name of the bash script to be run

```
./run_chk.sh -n ./check/debug/alrt.sh
```

Check scripts can be run:
* *remotely* - not on the sn1ff server
* *locally* - on the sn1ff server

The check will run the same, however the run_chk.sh script, will automatically determine, how to send the resulting check results report, to the sn1ff server


## "Big picture"

```
               sn1ff server                         remote host
                    |                                    |
                    '---------------.   .----------------'
                                    |   |
                   "local check"    |   |    "remote check"
                                    '---'
                                      |
                                      |
                       <sn1ff installation>/examples/
                                      |
                                      |
                   .------------------'-------------.
                   |                                |
         ,--> run_debug.sh  --,                     |
  cron --|--> run_hourly.sh --|--> run_chk.sh --> check/
         '--> run_daily.sh  --'                     |
                                                    |
                                                    |--> debug
                                                    |      |
                                                    |      |--> alrt.sh
                                                    |      |--> none.sh  
                                                    |      |--> okay.sh 
                                                    |      '--> warn.sh
                                                    |
                                                    |--> file
                                                    |      |
                                                    |      |--> suid_sgid.sh  
                                                    |      '--> world_writeable.sh
                                                    |
                                                    |--> firewall
                                                    |      |
                                                    |      '--> ufw.sh
                                                    |
                                                    |--> log
                                                    |      |
                                                    |      '--> journald
                                                    |              |
                                                    |              |--> disk_usage.sh  
                                                    |              |--> ssh_fail.sh  
                                                    |              '--> sudo_fail.sh
                                                    |
                                                    |--> mandatory_access_control
                                                    |      |
                                                    |      |--> apparmor
                                                    |              |
                                                    |              '--> installed_running.sh
                                                    |
                                                    |--> net
                                                    |      |
                                                    |      |--> port
                                                    |      |      |
                                                    |      |      '--> listening.sh
                                                    |      |
                                                    |      '--> sshd
                                                    |             |
                                                    |             '--> settings.sh
                                                    |
                                                    |--> package
                                                    |      |
                                                    |      '--> apt
                                                    |            |
                                                    |            '--> upgrades_waiting.sh
                                                    |
                                                    |--> tool
                                                    |      |
                                                    |      '--> lynis
                                                    |             |
                                                    |             '--> run_report.sh
                                                    |
                                                    '--> user
                                                          |
                                                          |--> failed_login.sh  
                                                          |--> last_login.sh 
                                                          '--> unknown_users.sh
```

---

# USING CRON TO AUTOMATE RUNNING THE BASH SCRIPT EXAMPLES

You will want to automate the running of your check scripts, such as the Linux **cron** utility

Cron is controlled by a crontab (cron table) file, that specifies shell commands to run on a given periodic schedule


## Editing crontab

To create/edit a crontab file for the current user, you can perform the following command:

```
$ crontab -e
```

An editor opens, where you specify the command to run. 

For example to run the above *"run_*" bash scripts, you could have the following crontab entries:

```
# Edit this file to introduce tasks to be run by cron.
# 
# ...
# 
# sn1ff
#
# Execute "run_debug.sh"  ... every 10 minutes:
# Execute "run_hourly.sh" ... every hour:
# Execute "run_daily.sh"  ... every day at midnight:

*/10 * * * *   cd <sn1ff directory>/examples; ./run_debug.sh > cron_debug.log 2>&1
0 * * * *   cd <sn1ff directory>/examples; ./run_hourly.sh > cron_hourly.log 2>&1
0 0 * * *   cd <sn1ff directory>/examples; ./run_daily.sh > cron_daily.log 2>&1
```


## Crontab quick reference

The following is a simplified crontab *quick reference*:

```
      * * * * * <command to execute>

    # | | | | |
    # | | | | day of the week (0–6) (Sunday to Saturday; 
    # | | | month (1–12)             7 is also Sunday on some systems)
    # | | day of the month (1–31)
    # | hour (0–23)
    # minute (0–59)

See:

    $ man cron

    $ man 8 cron
``` 


---

# WRITING YOUR OWN CHECK BASH SCRIPTS

Start by copying the contents of this directory (examples), to wherever you want your checks to be:

```
cp -R  ./ <directory wherever you want your checks to be>
```

