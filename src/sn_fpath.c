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

#include "sn_fpath.h"

/**
 * Build the destination "full path":
 *
 * Take the "tmp" filename in arg_f,
 *    the status in arg_s,
 *    the destination dir in arg_d ...
 *
 *    put the combination in new_dest
 *
 * @param arg_f
 * @param arg_s     state [ALRT|WARN|OKAY|NONE]
 * @param arg_d
 * @param arg_t     ttl in minutes
 * @param new_dest  return of new destination file path
 * @return  0 success
 *         -1 Could not initialise FName
 *         -2 Could not parse guid name or status
 */
int sn_fpath_genfull(const char *arg_f, const char *arg_s, const char *arg_d,
                     int arg_t, char *new_dest) {
  // Extract "file name" from arg_f

  char extracted_filename[CNAME_NAME_LENGTH_D] = {'\0'};
  memset(extracted_filename, '\0', (CNAME_NAME_LENGTH_D) * sizeof(char));

  int result = cn_fpath_get_name(arg_f, extracted_filename);
  if (result != 0) {
    cn_log_msg(LOG_ERR, __func__, "Bad status from cn_fpath_get_name -> %d <-",
               result);
    exit(1);
  }

  cn_log_msg(LOG_DEBUG, __func__, "Good status from cn_fpath_get_name -> %d <-",
             result);
  cn_log_msg(LOG_DEBUG, __func__, "Extracted_filename -> %s <-",
             extracted_filename);

  // Parse filename

  FName fname;
  if (sn_fname_init(&fname) != 0) {
    cn_log_msg(LOG_ERR, __func__, "Could not initialse FName",
               extracted_filename);
    return -1;
  }

  if (sn_cname_parse_guid_name(extracted_filename, arg_s, &fname.cname) != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Could not parse guid name ->%s<- or status ->%s<-",
               extracted_filename, arg_s);
    return -2;
  }

  // Set dir

  sn_fname_set_dir(&fname, arg_d);

  // Apply TTL offset to epoch

  sn_cname_incr_epoch_bin(&fname.cname, arg_t);

  // Build the full destination file path

  sn_fname_get_path(&fname, new_dest);

  cn_log_msg(LOG_DEBUG, __func__, "destination file-path 'new_dest' -> %s <-",
             new_dest);

  return 0;
}
