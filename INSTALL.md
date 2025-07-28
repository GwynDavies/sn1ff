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


# INSTALL

<br><br>

## 1.0 AVAILABILITY

sn1ff is currently only available for:

<br>

### 1.1 Operating system

* GNU/Linux.

<br>

### 1.2 Distributions

* **Debian** based GNU/Linux distributions:
    * **Intel x86_64**.

<br>

### 1.3 Format

* Debian **.deb** file packages:
    * Only at this github site,
    * Not currently available from the Debian distributions directly.

<br><br>

---

<br>

## 2.0 OVERVIEW

sn1ff is installed as a 'server', and a number of 'clients'.

<br>

### 2.1 sn1ff server

You will need to decide:

* Which host on your network, will be the **sn1ff server**:
    * Receiving check results files from the sn1ff clients, 
    * Allowing users to view the check results with the sn1ff monitor, 

<br>

### 2.2 sn1ff clients

You will also need to decide:

* Which other hosts will be the **sn1ff clients**:

    * Running the checks you write, and sending the results to the sn1ff server.

<br>

### 2.3 sn1ff architecture

```
        Debian based GNU/Linux distributions
  
  
                                                                     .--------------.
                            .---------------------------.      .-----| sn1ff client |
                            |       sn1ff server        |      |     '--------------'
                            |___________________________|   .-----.  .--------------.
                            |   sn1ff systemd service <-|---| SCP |--| sn1ff client |
   O      .-------------.   |   sn1ff local client      |   '-----'  '--------------'
  /'\  ---| terminal or |---|-> sn1ff monitor           |      |     .--------------.
 ' ^ `    | SSH         |   '---------------------------'      '-----| sn1ff client |
  / \     '-------------'                                            '--------------'
    
                                         ^                                  ^
                                         |                                  |
                                         |                                  |
  
                                   Debian package                     Debian package
                                       (.deb)                             (.deb)
```

<br><br>

---

<br>

## 3.0 INSTALLATION PREREQUISITES - SN1FF SERVER

<br>

### 3.1 sudo privilege

You **need** to have **sudo** privilege on the sn1ff server (and clients) to perform installation.

<br>

### 3.2 Github repo files

You **need** to *clone* or *download* the sn1ff Github repo files:

* [Github sn1ff repo](https://github.com/GwynDavies/sn1ff)

<br>

### 3.3 Ability to generate key-pair(s) for SCP/SSH communication

You **need** to be able to configure SSH/SCP key based authentication, for sn1ff to transfer the check results files, from the sn1ff client- to the sn1ff server.

You can check out URLs like some of these or similar (**not endorsements**):

* [How to generate and manage ssh keys on Linux](https://linuxconfig.org/how-to-generate-and-manage-ssh-keys-on-linux)


* [How To Generate SSH Key With ssh-keygen In Linux?](https://www.geeksforgeeks.org/how-to-generate-ssh-key-with-ssh-keygen-in-linux/)


* [How to Create an SSH Key in Linux: Easy Step-by-Step Guide](https://www.digitalocean.com/community/tutorials/how-to-configure-ssh-key-based-authentication-on-a-linux-server)

<br>

### 3.4  SSH server installed and running

#### 3.4.1 Check SSH server installed

On the sn1ff server, you **need** to have SSH server installed, if you are not sure, you can check with the following command:

* **dpkg -l openssh-server**

You should see output similar to the following:

```
        ii  openssh-server 1:8.x.x-x  amd64  secure shell (SSH) server, for secure access from remote machines
```

<br>

#### 3.4.2 Install SSH server

If the SSH server is not installed, you can install it with the following commands:

* **sudo apt update**

* **sudo apt install openssh-server**

<br>

#### 3.4.3 Check the SSH server is started, and enabled on boot

You also **need** to check that the SSH server is started, you can check with the following command:

* **sudo systemctl status ssh**

You should see output similar to the following:

```
        ● ssh.service - OpenBSD Secure Shell server
           Loaded: loaded (/lib/systemd/system/ssh.service; enabled)
           Active: active (running) since ...
```

Where **Active: active (running)** means the SSH server is started.

If it's not, you can start it with the following command:

* **sudo systemctl start ssh**

<br>

#### 3.4.4 Check SSH server is enabled on boot

To ensure the SSH server starts automatically on boot:

* **sudo systemctl enable ssh**

<br>

#### 3.4.5 Check SSH server is listening

To check if the SSH server is listening, you can use the following commands:

* **sudo ss -tulpn | grep ssh**

You should see output like:

```
        LISTEN  0  128  0.0.0.0:22     0.0.0.0:*    users:(("sshd",pid=1234,fd=3))
        LISTEN  0  128  [::]:22        [::]:*       users:(("sshd",pid=1234,fd=4))
```

<br>

#### 3.4.6 Check firewall access

If you are running a firewall, you will need to allow the SSH server access.

<br><br>

---

<br>

## 4.0 INSTALLATION PREREQUISITES - SN1FF CLIENT(S)

<br>

### 4.1 sudo privilege

You **need** to have **sudo** privilege on the sn1ff clients (and server) to perform installation.

<br>

### 4.2 Github repo files

You **need** to *clone* or *download* the sn1ff Github repo files:

* [Github sn1ff repo](https://github.com/GwynDavies/sn1ff)

<br>

### 4.3  SSH client installed

The SSH client is This is usually installed by default on most Debian systems.

However, you can check using the following command:

* **ssh -V**

If installed, you’ll see similar to the following:

```
        OpenSSH_8.4p1 Debian-5, OpenSSL 1.1.1k  DD MMM YYYY
```

If ssh is not found or not installed, install it with the following commands:

* **sudo apt update**

* **sudo apt install openssh-client**

<br><br>

---

<br>

## 5.0 INSTALL - SN1FF SERVER

**Make sure you have done the PREREQUISITES sections above.**

<br>

### 5.1 Package installation

From the your cloned/downloaded sn1ff git repo directory, perform the following in a command terminal (substituting the actual version numbers for **'N'**, and the computer architecture for **'ARCH'**):

<br>

#### 5.1.1 (OPTIONAL) Verify Debian package

You can optionally, verify the Debian package before you attempt to install it, with the Debian **"lintian"** utility to check for "policy violations and common errors":

* **sudo apt install lintian**
* **lintian sn1ff-server_N.N-N_ARCH.deb**
    * Example ... *lintian sn1ff-server_1.0-5_amd64.deb*

<br>

This command takes approximately 10-20 seconds.

There should be currently 0 errors and 1 expected warning:

```
W: sn1ff-server: initial-upload-closes-no-bugs [usr/share/doc/sn1ff-server/changelog.Debian.gz:1]
```

This is because I have not created a Debian bug report to request sn1ff's inclusion into the Debian available packages.

<br>

#### 5.1.2 Install package

* **sudo dpkg -i sn1ff-server_N.N-N_ARCH.deb**
    * Example ... *sudo dpkg -i sn1ff-server_1.0-5_amd64.deb*

<br>

Check the sn1ff systemd service is install, enable and running:

* **sudo systemctl status sn1ff**

You should see output similar to the following:

```
        ● sn1ff.service - Sn1ff Service
             Loaded: loaded (/lib/systemd/system/sn1ff.service; enabled; preset: enabled)
             Active: active (running) since Wed 2025-01-01 00:00:00 PDT; 3min 14s ago
           Main PID: 1414 (sn1ff_service)
              Tasks: 2 (limit: 4459)
             Memory: 1732.0K
                CPU: 4ms
             CGroup: /system.slice/sn1ff.service
                     ├─3142 /usr/bin/sn1ff_service
                     └─2718 sn1ff_cleaner
```

<br>

### 5.2 Add user(s) to sn1ff group

To be able to use the sn1ff_monitor, and run checks locally, a user must be a member of the sn1ff group.

You can add your user, and other users to this group as follows:

* **sudo usermod -aG sn1ff "$(whoami)"**

* **sudo usermod -aG sn1ff** *OTHER USER ID*

* etc.

<br>

### 5.3 Activate your group membership

Your group membership will activate when you logout/in again, However you can achieve that now in your current terminal by running the following command:

* **newgrp sn1ff**

<br>

### 5.4 Check start/stop of the sn1ff monitor

Start the sn1ff monitor, to make sure it connects to the sn1ff service, by running the following command:

* **sn1ff_monitor**

The sn1ff monitor should start, and display output similar to the following in the terminal:

```
        sn1ff
        1.0-5     [q]quit:

                  No FILES to display

                  $ man sn1ff
```

**Leave the sn1ff_monitor running**.

<br>

### 5.5 Run a local check

Start another terminal, again in your cloned/downloaded sn1ff git repo directory, perform the following commands:

* **newgrp sn1ff**

* **cd examples**

* **./run_debug.sh**

<br>

You should see output similar to the following:

```
$ ./run_debug.sh 
IS SN1FF SERVER ...
Running './check/debug/none.sh' locally.

././check/debug/none.sh
././check/debug/none.sh <>

IS SN1FF SERVER ...
Running './check/debug/okay.sh' locally.

././check/debug/okay.sh
././check/debug/okay.sh <>

IS SN1FF SERVER ...
Running './check/debug/warn.sh' locally.

././check/debug/warn.sh
././check/debug/warn.sh <>

IS SN1FF SERVER ...
Running './check/debug/alrt.sh' locally.

././check/debug/alrt.sh
././check/debug/alrt.sh <>
```


Switch back to the terminal running the sn1ff_monitor, you should see output similar to the following for ALRT, WARN, OKAY and NONE statuses:

```
        sn1ff     ALRT  somehost  192.0.2.0      Wed January 1, 2025 00:00:00 UTC
        1.0-5     [q]quit [f]faster [s]slower [^s]pause [^q]resume [d]delete
                  123e4567-e89b-12d3-a456-426614174000_ALRT_1745949432.snff

                                                                                     
                            .---------------------.
                            |                     |
                            | D E B U G   A L R T |
                            |                     |
                            '---------------------'
                                                                                     
DEBUG ALRT: CHECK SOMETHING
                                                                                     
Current TTL values (minutes):
  ALRT     1
  WARN   720
  OKAY    10
  NONE    10
                                                                                     
--------------------------------------------------------------------------------     
                                                                                     
                            RESULT: CHECK SOMETHING
```

* Press the **d** key to delete each of the check results file, you should then see when they are all deleted:

```
        sn1ff
        1.0-5     [q]quit:

                  No FILES to display

                  ...
```

* Press the **q** key to quit the sn1ff_monitor and return to the terminal shell prompt

<br><br>

---

<br>

## 6.0 INSTALL PUBLIC SSH KEYS FOR CLIENTS

<br>

### 6.1 Copy public keys

Deploy the sn1ff clients **public keys** to the sn1ff server - into the sn1ff service's user **authorized_keys** file.

This file needs to be located at:

* **/home/chroot/sn1ff/.ssh/authorized_keys**

<br>

Finally check the authorized_keys file, ownership and permissions:

* **sudo chown sn1ff:sn1ff /home/chroot/sn1ff/.ssh/authorized_keys**

* **sudo chmod 600 /home/chroot/sn1ff/.ssh/authorized_keys**

<br><br>

---

<br>

## 7.0 EDIT SSHD_CONFIG

<br>

### 7.1 Update sshd_config file

You now need to edit the sshd_config file (**Not the ssh_config file**). For instance if you use vim to edit:

* **sudo vim /etc/ssh/sshd_config**

Add the following lines to the end of the file *(indentation can be spaces or tabs)*:

```
Match User sn1ff
    ChrootDirectory /home/chroot/sn1ff
    ForceCommand internal-sftp
    AllowTcpForwarding no
    X11Forwarding no

```

<br>

### 7.2 Restart ssh

After editing sshd_config, you will need to restart the ssh service, using the following command:

* **sudo service ssh restart**

<br><br>

---

<br>

## 8.0 INSTALL - SN1FF CLIENTS

<br>

**Make sure you have done the PREREQUISITES sections above**

Perform the following steps on **each** sn1ff client.

<br>

### 8.1 Package install

From the your cloned/downloaded sn1ff git repo directory, perform the following in a command terminal (substituting the actual version numbers for **'N'**, and the computer architecture for **'ARCH'**):

<br>

#### 8.1.1 (OPTIONAL) Verify Debian package

You can optionally, verify the Debian package before you attempt to install it, with the Debian **"lintian"** utility to check for "policy violations and common errors":

* **sudo apt install lintian**
*  **lintian sn1ff-client_N.N-N_ARCH.deb**
    * Example ... *lintian sn1ff-client_1.0-5_amd64.deb*

<br>

This command takes approximately 10-20 seconds.

There should be currently 0 errors and 1 expected warning:

```
W: sn1ff-client: initial-upload-closes-no-bugs [usr/share/doc/sn1ff-server/changelog.Debian.gz:1]
```

This is because I have not created a Debian bug report to request sn1ff's inclusion into the Debian available packages.

<br>

#### 8.1.2 Install package

* **sudo dpkg -i sn1ff-client_N.N-N_ARCH.deb**
    * Example ... *sudo dpkg -i sn1ff-client_1.0-5_amd64.deb*

<br>

### 8.2 Configure sn1ff client

Using an editor, edit the sn1ff configuration file at **/etc/sn1ff/sn1ff.conf**.

This file has the following default settings:

```
min_log_level=info
client_ttls=1440,720,10,10
server_address=192.0.2.0

```

You **need** to change the value of *server_address*, from its fake value of *192.0.2.0*, to the IPv4 address, or hostname of your sn1ff server:

```
server_address=192.0.2.0
```

You can optionally also configure the default time-to-live (TTL) values for the check file statuses,

These values are supplied as a comma-separated list with 4 values, where the values represent in-order the TTL value for each of the available states in minutes:

* ALRT,WARN,OKAY,NONE

So for example, to set the check file state TTL values as follows:
* ALRT TTL -> 120 minutes,
* WARN TTL ->  60 minutes,
* OKAY TTL ->   5 minutes,
* NONE TTL ->   1 minute.

You would configure the following value in the configuration file:

```
client_ttls=120,60,5,1
```

<br>

### 8.3 Run check

In another terminal, again in your cloned/downloaded sn1ff git repo directory:

* **cd examples**

* **./run_debug.sh**

You should see output similar to that as described above in:

```
    ### 5.5 Run a local check
```

The check results should again appear in the **sn1ff_monitor** on the sn1ff server.


<br><br>

---

<br>

## 9.0 OTHER INSTALLED ITEMS

After installing either the sn1ff server or client, additional items are automatically installed.

<br>

### 9.1 man pages

Linux *man* pages are installed for sn1ff server and client components. See the overall sn1ff man page, using the following command:

* **man sn1ff**

This should display the sn1ff *man (7)* page:

```
SN1FF(7)                       Miscellaneous Information Manual                      SN1FF(7)

NAME
       sn1ff - enables users to write and monitor "checks"

DESCRIPTION
       sn1ff  enables  Linux  system  users, to write cybersecurity and system "checks" - using 
       familiar scripting or program languages, and monitor the results of these checks - from 
       a terminal or SSH login.

       There are 2 main parts to sn1ff:
       - sn1ff server
       - sn1ff client

      ...
```

The **SEE ALSO** section at the end identifies additional man pages: 

```
SEE ALSO
   Other related pages:
       sn1ff_service(8),  sn1ff_cleaner(8),  sn1ff_monitor(1),  sn1ff_client(1), 
       sn1ff_license(1), sn1ff_conf(1), chroot(8), systemd(1).
```
<br>

### 9.2 License display

You can display the current sn1ff license, by using the following command:

* **sn1ff_license**

<br>

### 9.3 Configuration file display

You can display your current sn1ff *configuration file* settings, by using the following command:

* **sn1ff_conf**

```
$ sn1ff_conf 
Usage: sn1ff_conf [OPTION]
Options:
  -t    Print value of 'client_ttls' from config file
  -a    Print value of 'server_address' from config file
  -h    Show this help message
```

<br><br>

---

<br>

## 10.0 NEXT STEPS

See the **README.md** file in the examples directory.

<br><br>

---

<br>

## 11.0 UNINSTALL - SN1FF SERVER

<br>

On the **sn1ff server**, in a terminal perform the following commands:

* **sudo apt remove sn1ff-server**

* **sudo dpkg --purge sn1ff-server**

* **sudo systemctl daemon-reload**

<br><br>

---

<br>

## 12.0 UNINSTALL - SN1FF CLIENT

On *each* **sn1ff client**, in a terminal perform the following commands:

* **sudo apt remove sn1ff-client**

* **sudo dpkg --purge sn1ff-client**

