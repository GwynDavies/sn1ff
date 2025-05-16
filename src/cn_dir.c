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

#include "cn_dir.h"
#include "cn_log.h"
#include "cn_string.h"

/*
 * @return  0 success
 *         -1 does not exist or cannot access
 *         -2 file exists but is not a directory
 */
int cn_dir_exists(const char *path) {
  struct stat statbuf;
  if (stat(path, &statbuf) != 0) {
    cn_log_msg(
        LOG_ERR, __func__,
        "Stat gave error accessing file -> %s <-, strerror(errno) -> %m <-");
    return -1;
  }

  // Return 0 if it's a directory, 2 if it's something else

  return S_ISDIR(statbuf.st_mode) ? 0 : -2;
}

/**
 * Create directory, with mode 0700
 *
 * You need to check if it exists already with cn_dir_exists first
 *
 * @param dir_path  is the full path for the directory to create
 * @ return  0 success
 *          -1 error creating
 */
int cn_dir_create(const char *dir_path) {
  if (mkdir(dir_path, 0700) == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'mkdir' gave error trying to create dir -> %s <-. "
               "strerror(errno) -> %m <-");
    return -1;
  }

  return 0;
}

/**
 * Get home directory full path
 *
 * @param path  Is the returned process user directory, or NULL if it could not
 * be retrieved
 * @return   0 success
 *          -1 error
 */
int cn_dir_home(char *path, size_t sz) {
  int result = cn_string_cp(path, sz, getenv("HOME"));
  if (result != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Could not retrieve or copy - process user's home dir");
    return -1;
  }

  return 0;
}
