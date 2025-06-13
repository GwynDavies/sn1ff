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


# WRITING YOUR OWN CHECKS

sn1ff lets you write *your own* check scripts, using [https://www.gnu.org/software/bash/](bash)


## sn1ff bash check scripts

A sn1ff bash check script, performs the following steps:

* Begin a sn1ff check results file
* Append check output, to the check results file
* Complete the check with a:
    * Status [ALRT|WARN|OKAY|NONE]
    * Time-to-live (TTL)
* Send the completed check results file to the sn1ff server

sn1ff ships with some bash script *libraries*, for performing these steps


## Bash script libraries

sn1ff comes with some pre-written bash functionality, made available as *libraries*

These are under the *"<sn1ff>/examples/lib"* directory, with the main library being *"sn1ff_lib.sh"*

To use this libraries, you *"source"* them, in your check scripts - using a relative path:

```
#!/bin/bash

# .----------------------------------------------------------------.
# |                                                                |
# | SOURCE SN1FF BASH LIBRARIES                                    |
# |                                                                |
# '----------------------------------------------------------------'

source "$(dirname "$0")/../../lib/sn1ff_lib.sh"
```


## Check result file TTLs

sn1ff requires a TTL *(time-to-live)* value to *complete* a script. 

This allows the sn1ff-server, to remove check results files - that have exceeded their TTL

How long a TTL should be, for a particular check results status, is for you to decide

You can see the default TTL values, by running command:

```
sn1ff_conf -t
```

The values are defined on file /etc/sn1ff/sn1ff.conf 

You can also *override* the TTL values within each check script, by using the library sn1ff_lib.sh functions:

* **sn_set_alrt_ttl <TTL value in minutes>**
* **sn_set_warn_ttl <TTL value in minutes>**
* **sn_set_okay_ttl <TTL value in minutes>**
* **sn_set_none_ttl <TTL value in minutes**

You can access the current TTL values within a check script, with the library function **sn_show_ttls*

```
# Override default TTL values
#
sn_set_alrt_ttl 1
sn_set_warn_ttl 1
sn_set_okay_ttl 1
sn_set_none_ttl 1
sn_show_ttls
```


## Local functions

Depending on the complexity of you check script, you may want to write some of its functionality as functions - within *("local to")* the check script:

```
# .----------------------------------------------------------------.
# |                                                                |
# | LOCAL FUNCTIONS                                                |
# |                                                                |
# '----------------------------------------------------------------'

# None

```


## Processing arguments

When the *"run_chk.sh"* runs a check script, it will pass:
* The *"ADDRess"* of the sn1ff server, if the check **IS NOT** being run on the sn1ff server** (remote)
* Otherwise, it will not pass any argument, if the check script **IS** being run on the sn1ff server (local)

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


## Script settings

Next you can define settings, that apply throughout the script:

```
# .----------------------------------------------------------------.
# |                                                                |
# | SETTINGS                                                       |
# |                                                                |
# '----------------------------------------------------------------'

CHECKID="DEBUG ALRT"
```


## Beginning the check results file

The sn1ff command **"sn1ff_client -b"**, creates a new sn1ff check results file. 

You can then proceed to append the any check output, you wish to capture:

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


## Script output banner

To make check results easier to distinguish, when being displayed by the **sn1ff_monitor,** you can add a banner:
 to be displayed at the start of the check results:

```
# .----------------------------------------------------------------.
# |                                                                |
# | DISPLAY BANNER                                                 |
# |                                                                |
# '----------------------------------------------------------------'

sn_append_titlebox "$CHECKID" "$SN_FILENAME"
```

For example:

```
                            .---------------------.
                            |                     |
                            | D E B U G   A L R T |
                            |                     |
                            '---------------------'
```


## Check scripts with multiple "steps"

Checks can go through multiple "steps", for example:

* First step:  Is <feature> installed
* Second step: Is <feature> enabled
* Third step:  Is <feature> configured/operating as expected
* etc.

This can be realized as a script with a generalized "IF" format:

```
if <First step is not true or valid> then
    exit with check status ALRT|WARN|NONE

if <Second step is not true or valid> then
    exit with check status ALRT|WARN|NONE

if <Nth step is not true or valid> then
    exit with check status ALRT|WARN|NONE

# All "steps" passed checking

exit with check status OKAY
```


## Recording check output

As you proceed through the check steps, you may want to capture output, from the checking activities

This is done, by appending the output to the check results file - created earlier in the script

This can be done as follows:
* Appending "headers" to "section" or "format" the output using **"sn_append_first_header"** or **"sn_append_header"** library functions
* Appending output from commands performing the checking, using the **"sn_append_message"** library function
* You can also just use bash *"echo"* or *" printf"* statements,

For each of these, you *"append"* the output using the bash *append* operator **(>>),** with the check results file:

```
echo "some output text" >> "$SN_FILENAME" 2>&1
```


## Completing the check script

Complete the check script, by calling library function **"sn_exit_with_message"**:

```
  sn_exit_with_message "<summary message>" "<check results file>" "<status>" "<address of sn1ff server>"

    <summary message> : This message is displayed at the end of the check results on the sn1ff
                        server 
  
    <check results file> : The check results file, created earlier in the script by the **"sn_client -b"**

    <status> : [ALRT|WARN|OKAY|NONE]. The interpretation of the statuses is up to you. 
               Typical interpretations are:
                 ALRT - Potential or actual system/cybersecurity issue detectedA,
                 WARN - Anomyly has been detected, but not indicating an ALRT, 
                 OKAY - No issues found, but shows that the check is being run,
                 NONE - No status to report. Potentially use to show a check was
                          run, but ifound to be **not applicable** for the system 
                          type, configuration or other criteria

    <address of sn1ff server> : IP or hostname of sn1ff server to send the check 
                                results file to. This is obtained by the run_chk.sh
                                script for you
```


For example:

```
  sn_exit_with_message 'ALL CHECKS PASSED' "$SN_FILENAME" "OKAY" "$SN_ADDR"
                                                             |
              .----------------------------------------------'
              |
              V

  sn1ff     OKAY   banshee  No ip for eth0      Sun May 11, 2025 03:10:01 UTC
  1.0-2     [q]quit [f]faster [s]slower [^s]pause [^q]resume [d]delete
            688d2c5b-afd3-45e8-9372-7e6761983a23_OKAY_1746933061.snff

                            .---------------------.
                            |                     |
                            | D E B U G   O K A Y |
                            |                     |
                            '---------------------'
                                                                                     
DEBUG OKAY: CHECK SOMETHING
                                                                                     
Current TTL values (minutes):
  ALRT  1440
  WARN   720
  OKAY     1
  NONE    10
                                                                                     
--------------------------------------------------------------------------------     
                                                                                     
                            RESULT: CHECK SOMETHING
```


---

# RUNNING RESTRICTED COMMANDS WITH "SUDO"

Some of the commands used by check scripts, maybe restricted and require "privileged" execution

## sudo command

This can be achieved, by making use of the **sudo** command

In such cases, you can configure your user, who will be running the check scripts to perform the *sudo* command 

This is typically started, by adding the user to the **"sudo group"** (Check your distribution's documentation for details


## Granting sudo privileges

Privileges can be granted with sudo, in varying levels of **"granularity"**

In accordance with security **"best practices",** you should only grant the minimum privilege - required to perform the command(s)

You can edit users privileges, with the **"visudo"** command.

Below is an extract from a **"sudoers"** file, showing privileges being granted explicitly, for the example checks that require them:

```
# Allow sn1ff user to get their sudo permissions
<your user> ALL=(ALL) NOPASSWD: /usr/bin/sudo -l
# <sn1ff installation>/examples/check/net/sshd/settings.sh
<your user> ALL=(ALL) NOPASSWD: /usr/sbin/sshd -T
#<sn1ff installation>/examples/check/package/apt/updates_waiting.sh
<your user> ALL=(ALL) NOPASSWD: /usr/bin/apt update -qq
# <sn1ff installation>/examples/check/user/failed_login.sh
<your user> ALL=(ALL) NOPASSWD: /usr/bin/lastb
# <sn1ff installation>/examples/check/firewall/ufw.sh
<your user> ALL=(ALL) NOPASSWD: /usr/sbin/ufw status verbose
# <sn1ff installation>/examples/check/tool/lynis/run_report.sh
<your user> ALL=(ALL) NOPASSWD: /usr/sbin/lynis audit system
<your user> ALL=(ALL) NOPASSWD: /bin/cat /var/log/lynis-report.dat
```


---

# MORE INFORMATION ON RUNNING EXAMPLE CHECK SCRIPTS

For more information on running the example check scripts, so the **README.md** files in each of their directories

