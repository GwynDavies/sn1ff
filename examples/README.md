# License

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


# 1.0 OVERVIEW

## 1.1 Bash script examples

This directory provides *examples* of sn1ff check (bash) scripts.

They are:

* ... named with prefix 'chk_', just as a convention,
* ... incomplete, just to demonstrate the *type* of functionality, sn1ff offers.



## 1.2 Non bash script examples

You are not limited to using bash to write checks. In the sub-directory *non_bash_examples*, there are a few **"'hello world'" "'skeleton'"** type examples - for *Perl*, *Python* and *Ruby* languages.


---


# 2.0 RUN_CHK.SH

To run your sn1ff checks, you can use the bash script:

* **run_chk.sh**


## 2.1 Modes of operation

This offers 3 modes, of running your check scripts:

* **Menu driven** execution,
* **Direct** execution,
* **Cron** execution.


## Getting help

Brief help is available by running this script with *-h|--help*:

```
$ ./run_chk.sh -h

  Usage: ./run_chk.sh [OPTIONS]

  Options:
    -n, --name SCRIPT_NAME    Run the specified script.
    -h, --help               Show this help message.

```


---


# 3.0 MENU DRIVEN EXECUTION

When run without arguments, *run_chk.sh* displays a menu of the check scripts, you have in the current directory (chk*.sh).

You then just enter the number of the check script:

```
  ┌──────────────────────────────────────┐
  │     Available sn1ff checks to run    │
  └──────────────────────────────────────┘

     1.  chk_debug_fail.sh
     2.  chk_debug_pass.sh
     3.  chk_firewall_ufw.sh
     4.  chk_listening_ports.sh

    Enter number of check to run (or e to exit): 

```


---


# 4.9 DIRECT EXECUTION

## 4.1 Single check script

The *run_chk.sh* script can also execute check scripts directly, without the menu.

To do this, start the script with the **-n** (name) option, specifying the check script to run:

```
$ ./run_chk.sh -n chk_debug_pass.sh

```


## 4.2 Multiple check scripts

To run multiple check scripts directly, simply place multiple invocations of "*run_chk -n*", into a bash script:

```
#!/bin/bash

./run_chk.sh -n chk_debug_pass.sh
./run_chk.sh -n chk_debug_fail.sh
./run_chk.sh -n chk_firewall_ufw.sh
./run_chk.sh -n chk_listening_ports.sh

```


---


# 5.0 CRON EXECUTION

Finally automate the running of your check scripts, using for instance using the Linux **cron** utility.

Cron is controlled by a crontab (cron table) file, that specifies shell commands to run on a given periodic schedule.


## 5.1 Single check script

To create/edit a crontab file for the current user, you can perform the following command:

```
$ crontab -e
```

An editor opens, where you specify the command to run. For example to run a check script every 10 minutes:

```
*/10 * * * *   cd <cloned/copied sn1ff github repo>/examples; ./run_chk.sh -n chk_debug_pass.sh  > cron_sn1ff_checks.log 2>&1

e.g.

*/10 * * * *   cd /home/youruser/github/sn1ff/examples; ./run_chk.sh -n chk_debug_pass.sh  > cron_sn1ff_checks.log 2>&1
```


## 5.2 Multiple check scripts

To run multiple check scripts, specify a bash script containing the desired invocations of "run_chk.sh":

```
$ crontab -e
```

Again to run every 10 minutes:

```
*/10 * * * *   cd cd <cloned/copied sn1ff github repo>/examples; ./run_by_cron.sh > cron_sn1ff_checks.log 2>&1

e.g.

*/10 * * * *   cd /home/youruser/gitlab/sn1ff/examples; ./run_by_cron.sh > cron_sn1ff_checks.log 2>&1

```


## 5.3 Crontab quick reference

The following is a simplified crontab *quick reference*:

```
      * * * * * <command to execute>

    # | | | | |
    # | | | | day of the week (0–6) (Sunday to Saturday; 
    # | | | month (1–12)             7 is also Sunday on some systems)
    # | | day of the month (1–31)
    # | hour (0–23)
    # minute (0–59)

``` 

Please refer to your Linux distributions man pages for further information:

```
$ man cron

$ man 8 cron

```


---


# 6.0 WRITING YOUR OWN CHECKS

sn1ff is designed to be *simple* for the user to write their own checks.

The focus is primarily on writing checks using [https://www.gnu.org/software/bash/](GNU Bash).


## 6.1 sn1ff in a nutshell

sn1ff is a simple utility, that allows you to:

* Create or start, a sn1ff check results file,
* Append check output to this results file,
* Complete the file with a:
    * Status [ALRT|WARN|OKAY|NONE],
    * Time-to-live (TTL).
* Send the completed file to the sn1ff server

sn1ff ships with a bash *library* to make these steps as easy-as-possible.


## 6.2 Bash libraries

sn1ff comes with some basic pre-written bash functionality, made available as *libraries*.

The main library is *"./lib/sn1ff_lib.sh"*.

To use this library, you *"source"* it from you check script:

```
#!/bin/bash

# .----------------------------------------------------------------.
# |                                                                |
# | SOURCE SN1FF BASH LIBRARIES                                    |
# |                                                                |
# '----------------------------------------------------------------'

source "$(dirname "$0")/lib/sn1ff_lib.sh"

```


## 6.3 Check result TTLs

sn1ff requires a TTL (time-to-live) value to *complete* a script. This allows the sn1ff-server to remove check results files, that have expired their TTL automatically.

How long a TTL should be for a particular check results status, is for you to decide.

When sn1ff is installed, default TTL values for client installation can be specified. You can also *override* the TTL values for each check script, by using the sn1ff_lib.sh functions:

* **sn_set_alrt_ttl <TTL value in minutes>**
* **sn_set_warn_ttl <TTL value in minutes>**
* **sn_set_okay_ttl <TTL value in minutes>**
* **sn_set_none_ttl <TTL value in minutes**

You can display the current TTL values with the **sn_show_ttls* function.

```
# Override default TTL values
#
sn_set_alrt_ttl 1
sn_set_warn_ttl 1
sn_set_okay_ttl 1
sn_set_none_ttl 1
sn_show_ttls
```


## 6.4 Local functions

Depending on the complexity of you check script, it may make sense to write some of its functionality as functions. This portion of the script is a good place to do that.

```
# .----------------------------------------------------------------.
# |                                                                |
# | LOCAL FUNCTIONS                                                |
# |                                                                |
# '----------------------------------------------------------------'

# None

```


## 6.5 Processing arguments

```
# .----------------------------------------------------------------.
# |                                                                |
# | PROCESS ARGUMENTS                                              |
# |                                                                |
# '----------------------------------------------------------------'

if [[ $# -eq 0 || $# -eq 1 ]]; then
  SN_ADDR=$1
  echo ""
  echo "$0"
  echo "$0 <$SN_ADDR>"
  echo ""
else
  echo "EXPECTED 0 OR 1 ARGUMENTS - $0 || $0 <ADDR>"
  exit 1
fi

```


## 6.6 Beginning the check results file

The sn1ff command **"sn1ff_client -b"** command, creates a new sn1ff check results file. You can now proceed to append output:

```
# .----------------------------------------------------------------.
# |                                                                |
# | BEGIN SN1FF FILE                                               |
# |                                                                |
# '----------------------------------------------------------------'

SN_FILENAME=$(sn1ff_client -b)
exit_code=$?

if [[ $exit_code -ne 0 ]]; then
  echo "ERROR: FAILED TO BEGIN SN1FF FILE - STATUS -> $exit_code SN_FILENAME -> SN_FILENAME"
  exit 1
fi

```


## 6.7 Check script steps and format

Checks often go through multiple steps, for example:
* Step: Is <feature> installed
* Step: Is <feature> enabled
* Step: Is <feature> configured/operating as expected
* etc.

This can be realized as a script with a generalized format:

```
if <First step is not true or valid> then
    exit with check FAILING

if <second step is not true or valid> then
    exit with check FAILING
 ...

if <n'th step is not true or valid> then
    exit with check FAILING

exit with check PASSING

```

## 6.8 Recording check output

As you proceed through the check steps, you may want to record output from the checking activities. This is done, by appending this output to the check results file, created earlier in the script.

This 'appending' takes 2 forms:

* Appending "headers" to format the output using **"sn_append_first_header"** or **"sn_append_header"**,
* Appending output text from commands performing the checking, using the bash *append* operator **>>**. 


For example, here is an excerpt from script chk_debug_pass,sh:

```
  # .----------------------------------------------------------------.
  # |                                                                |
  # | CHECK INITIAL PART                                             |
  # |                                                                |
  # '----------------------------------------------------------------'
  
  sn_append_first_header "DEBUG PASS: CHECK INITIAL PART" "$SN_FILENAME"
  
  # <Some checking code here>
  echo "Check who the checker is -> $(whoami)" >> "$SN_FILENAME"
  fake_exit_code=0
 
   ...

  # .----------------------------------------------------------------.
  # |                                                                |
  # | CHECK FINAL PART                                               |
  # |                                                                |
  # '----------------------------------------------------------------'

  sn_append_header "DEBUG PASS: CHECK FINAL PART" "$SN_FILENAME"

  # <Some more checking code here>
  echo -e  "Check what the Linux release is ->\n$(cat /etc/os-release)\n"  >> "$SN_FILENAME"
  fake_exit_code=0

   ...
```

This would give output like the following, when the check results file is displayed on the sn1ff server:

```
  DEBUG PASS: CHECK INITIAL PART
                                                                                     
  Check who the checker is -> tux
                                                                                     
                                                                                     
  DEBUG PASS: CHECK FINAL PART
                                                                                     
  Check what the Linux release is ->
  PRETTY_NAME="Debian GNU/Linux 12 (bookworm)"
  NAME="Debian GNU/Linux"
  VERSION_ID="12"
  VERSION="12 (bookworm)"
  VERSION_CODENAME=bookworm
  ID=debian
  HOME_URL="https://www.debian.org/"
  SUPPORT_URL="https://www.debian.org/support"
  BUG_REPORT_URL="https://bugs.debian.org/"
                                          
```


## 6.9 Completing the check

Complete the check results file, by calling function **"sn_exit_with_message"**:

```
  sn_exit_with_message "<summary message>" "<check results file>" "<status>" "<address of sn1ff server>"

    <summary message> : This message is displayed at the end of the check results on the sn1ff
                        server. 
  
    <check results file> : The check results file, created earlier in the script by the **"sn_client -b"**

    <status> : [ALRT|WARN|OKAY|NONE]. The interpretation of the statuses is up to you. 
               Typical interpretations are:
                 ALRT - Potential or actual system/cybersecurity issue detectedA,
                 WARN - Anomyly has been detected, but not indicating an ALRT, 
                 OKAY - No issues found, but shows that the check is being run,
                 NONE - No status to report. Potentially use to show a check was
                          run, but ifound to be **not applicable** for the system 
                          type, configuration or other criteria.

    <address of sn1ff server> : IP or hostname of sn1ff server to send the check 
                                results file to. Do not set if you are running
                                the check **on** the sn1ff server (local check).
```

Here are some examples, and the resulting check results file extracts that appear on the sn1ff server:

```
  sn_exit_with_message 'ALL CHECKS PASSED' "$SN_FILENAME" "OKAY" "$SN_ADDR"
                                                             |
              .----------------------------------------------'
              |
              V

  sn1ff     OKAY   banshee  No ip for eth0      Sun May 11, 2025 03:10:01 UTC
  1.0-1     [q]quit [f]faster [s]slower [^s]pause [^q]resume [d]delete
            688d2c5b-afd3-45e8-9372-7e6761983a23_OKAY_1746933061.snff
                                                                                     
  DEBUG PASS: CHECK INITIAL PART
                                                                                     
  Check who the checker is -> tux
                                                                                     
                                                                                     
  DEBUG PASS: CHECK FINAL PART
                                                                                     
  Check what the Linux release is ->
  PRETTY_NAME="Debian GNU/Linux 12 (bookworm)"
  NAME="Debian GNU/Linux"
   ...
                                                                                     
  --------------------------------------------------------------------------------     
                                                                                       
                           .---> ALL CHECKS PASSED
                           |
               . . . . ----'

```

Also for failing with *"ALRT"* status:

```
  sn_exit_with_message "FAILED: CHECK FINAL PART" "$SN_FILENAME" "ALRT" "$SN_ADDR"
                                                                    |
              .-----------------------------------------------------'
              |
              V

  sn1ff     ALRT   banshee  No ip for eth0      Sun May 11, 2025 04:40:02 UTC
  1.0-1     [q]quit [f]faster [s]slower [^s]pause [^q]resume [d]delete
            80d14248-c826-44fb-8743-36ce1e12b09c_ALRT_1746938702.snff
                                                                                     
  DEBUG FAIL: CHECK INITIAL PART
                                                                                       
  Check who the checker is -> gduser
                                                                                     
                                                                                     
  DEBUG FAIL: CHECK FINAL PART
                                                                                     
  Check what the Linux release is ->
  PRETTY_NAME="Debian GNU/Linux 12 (bookworm)"
  NAME="Debian GNU/Linux"
   ...
                                                                                     
  --------------------------------------------------------------------------------     
                                                                                     
                        .---> FAILED: CHECK FINAL PART
                        |
            . . . . ----'

```

