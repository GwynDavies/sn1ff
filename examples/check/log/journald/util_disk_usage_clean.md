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

