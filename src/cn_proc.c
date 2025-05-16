/*
MIT License

Copyright (c) 2025 Gwyn Davies

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define _POSIX_C_SOURCE 200809L

#include "cn_proc.h"
#include "cn_log.h"

int cn_proc_ingrp(gid_t target_gid) {
  int ngroups = getgroups(0, NULL);
  if (ngroups < 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'getgroups' gave bad count for group id -> %d <-, "
               "strerror(errno) -> %m <-",
               target_gid);
    return -1;
  }

  gid_t groups[ngroups];
  if (getgroups(ngroups, groups) < 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'getgroups' gave bad list for group id -> %d <-, "
               "strerror(errno) -> %m <-",
               target_gid);
    return -1;
  }

  for (int i = 0; i < ngroups; ++i) {
    if (groups[i] == target_gid)
      return 0;
  }

  return -1;
}
