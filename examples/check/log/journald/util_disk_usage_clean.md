# Various commands to clean the JournalD disk usage

## Clean by time

* --vacuum-time=<time>: Remove log entries older than a specified date.
  * sudo journalctl --vacuum-time=1d
  * sudo journalctl --vacuum-time=7d
  * sudo journalctl --vacuum-time=1month
  * sudo journalctl --vacuum-time=1years


## Clean by size

* --vacuum-size=<bytes>: Shrink the journal to a desired size.
  * sudo journalctl --vacuum-size=1M
  * sudo journalctl --vacuum-size=500M
  * sudo journalctl --vacuum-size=1G


## Clean by number journal files

* --vacuum-files=<int>: Reduce the number of journal files to <int>.
  * sudo journalctl --vacuum-files=10

