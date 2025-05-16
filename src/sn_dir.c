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

#include "sn_dir.h"
#include "cn_dir.h"
#include "cn_log.h"

/*
 * Check if a filename has the .snff extension
 *
 * @param  filename
 *
 * @return  1  filename has extension
 *          0  filename does not have the extension
 *          0  filename is < extension length
 */
int sn_dir_file_has_ext(const char *filename) {
  size_t len = strlen(filename);
  size_t ext_len = strlen(EXTENSION);
  if (len < ext_len)
    return 0;

  // Case-insensitive check
  return (strcasecmp(filename + len - ext_len, EXTENSION) == 0);
}

/*
 * List files with .snff extension and return them as a string
 *
 * @param dir_path
 * @param ms  Multi string containing the file names
 *
 * @return  0 success
 *          1 error opening directory
 */
int sn_dir_list_files(const char *dir_path, MultiString *ms) {
  DIR *dir = opendir(dir_path);
  if (!dir) {
    cn_log_msg(LOG_ERR, __func__,
               "'opendir' gave error opening directory -> %s <-, "
               "strerror(errno) -> %m <-",
               dir_path);
    return 1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (sn_dir_file_has_ext(entry->d_name)) {
      cn_multistr_append(ms, entry->d_name);
    }
  }

  closedir(dir);

  return 0;
}

/**
 * Get a sn1ff client process's 'sn1ff' dir, for sn1ff files:
 *   <HOME dir>/sn1ff
 *
 * Attempts to create the dir if it does not exist
 *
 * @param sn1ff_dir_path  is to receive the client sn1ff files dir
 * @param sn1ff_dir_path_sz  is the character size of above
 * @return  0 success
 *         -1 could not get sn1ff directory
 *         -2 could not create sn1ff directory
 */
int sn_dir_client(char *sn1ff_dir_path, int sn1ff_dir_path_sz) {
  /*
   * Construct the full path for the "sn1ff" directory
   */

  // Get process user's HOME dir and add "sn1ff"

  char home_dir_path[256];
  int result = cn_dir_home(home_dir_path, 255);
  if (result != 0) {
    cn_log_msg(LOG_ERR, __func__, "Could not get process user's home dir");
    return -1;
  }

  // Add "sn1ff" to HOME dir

  snprintf(sn1ff_dir_path, sn1ff_dir_path_sz, "%s/sn1ff", home_dir_path);

  result = cn_dir_exists(sn1ff_dir_path);

  // sn1ff exists in HOME, but it is not a directory

  if (result == -2) {
    cn_log_msg(LOG_ERR, __func__,
               "File 'sn1ff' exists in HOME DI - but it is not a directory");
    return -1;
  }

  // Does not exist, create it

  else if (result == -1) {
    int result = cn_dir_create(sn1ff_dir_path);
    if (result != 0) {
      cn_log_msg(LOG_ERR, __func__, "Failed to created dir -> %s <-",
                 sn1ff_dir_path);
      return -2;
    }
  }

  return 0;
}
