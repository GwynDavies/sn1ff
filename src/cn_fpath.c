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

#include "cn_fpath.h"
#include "cn_log.h"
#include "sn_cname.h"
#include <libgen.h> // For basename
#include <stdlib.h> // For strdup
#include <string.h> // For strcpy

/**
 * Get the file name, from a file path
 *
 * Params:
 *   I   - fpath - the file path
 *   I/O - name - the file name
 *
 * Return:
 *   0 - Success
 *   1 - fpath or name is NULL
 *   2 - strdup fails (i.e., returns NULL)
 */
int cn_fpath_get_name(const char *fpath, char *name) {
  if (fpath == NULL || name == NULL) {
    return CN_FPATH_GET_FNAME_INVALID_PATH; // Invalid input
  }

  // Duplicate the path to avoid modifying the original string
  char *fpath_copy = strdup(fpath);
  if (fpath_copy == NULL) {
    cn_log_msg(LOG_ERR, __func__,
               "Strdup gave error by returning NULL when duplicating fpath -> "
               "%s <-., strerror(errnon) -> %m <-",
               fpath);

    return CN_FPATH_GET_FNAME_MEM_ALLOC_FAIL; // Memory allocation failure
  }

  // Get the file name using basename
  char *file_name = basename(fpath_copy);
  strncpy(name, file_name, CNAME_NAME_LENGTH);

  // Free the duplicated string
  free(fpath_copy);

  return CN_FPATH_GET_FNAME_SUCCESS; // Success
}
