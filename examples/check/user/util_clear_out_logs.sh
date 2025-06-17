#!/bin/bash

# Clear out the logs

sudo truncate -c --size 0 /var/log/wtmp
sudo truncate -c --size 0 /var/log/btmp
sudo truncate -c --size 0 /var/log/lastlog

ls -l /var/log/wtmp
ls -l /var/log/btmp
ls -l /var/log/lastlog

