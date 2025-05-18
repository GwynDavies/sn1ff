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

#include "sn_fname.h"
#include "cn_log.h"

/*
 * Sn1ff fname:
 *   - dir
 *   - cname
 *       - components:
 *           <guid>_<status>_<epoch>
 *   - path:
 *       dir / cname
 */

/*----------------------------------------------------------------.
 |                                                                |
 | Object                                                         |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Initial FName object for use
 *
 * @param  fname  is the FName to initialise
 * @return  0 success
 *         -1 error initialising fname object epoch.bin
 */
int sn_fname_init(FName *fname) {
  // dir

  memset(fname->dir, '\0', FNAME_DIR_LENGTH_D * sizeof(char));

  // name -> prefix

  memset(fname->cname.prefix, '\0', CNAME_PREFIX_LENGTH_D * sizeof(char));

  // name -> guid

  memset(fname->cname.guid.str, '\0', CNAME_GUID_LENGTH_D * sizeof(char));

  uuid_generate(fname->cname.guid.bin);
  uuid_unparse(fname->cname.guid.bin, fname->cname.guid.str); // Convert to str

  // name -> status

  memset(fname->cname.status, '\0', CNAME_STATUS_LENGTH_D * sizeof(char));

  // name -> epoch

  fname->cname.epoch.str[0] = '\0';
  memset(fname->cname.epoch.str, '\0',
         CNAME_EPOCH_LENGTH_D * sizeof(char)); // safer

  fname->cname.epoch.bin = time(NULL);
  if (fname->cname.epoch.bin == -1) {
    // Warning - man page does not state if errno is set or not

    cn_log_msg(LOG_ERR, __func__,
               "'time_t' returned error (-1), strerror(errno) -> %m <-");
    return -1;
  }
  sprintf(fname->cname.epoch.str, "%ld", fname->cname.epoch.bin);

  return 0;
}

void sn_fname_dump(FName *fname) {
  cn_log_msg(LOG_DEBUG,
             "sn_fname: sn_fname_dump:\n"
             "    dir              ->%s<-\n"
             "    cname.prefix     ->%s<-\n"
             "    cname.guid.str   ->%s<-\n"
             "    cname.status     ->%s<-\n"
             "    cname.epoch.str  ->%s<-\n"
             "\n",
             fname->dir, fname->cname.prefix, fname->cname.guid.str,
             fname->cname.status, fname->cname.epoch.str);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Dir                                                            |
 |                                                                |
 '----------------------------------------------------------------*/

void sn_fname_set_dir(FName *fname, const char *dir) {
  strncpy(fname->dir, dir, FNAME_DIR_LENGTH);
}

void sn_fname_get_dir(const FName *fname, char *dir) {
  strncpy(dir, fname->dir, FNAME_DIR_LENGTH);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Path                                                           |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Just have dir and guid, this gives the temporary path used
 * when beginning a sn1ff file
 *
 * <dir>/<guid>.snff
 */
void sn_fname_get_tmp_path(const FName *fname, char *tmp_path) {
  snprintf(tmp_path, FNAME_PATH_LENGTH, "%s/%s.snff", fname->dir,
           fname->cname.guid.str);
}

/**
 * Returns two different paths:
 *
 * If there is no prefix - <dir>/<guid>_<status>_<epoch>.snff
 * If there is a prefix  - <dir>/<prefix><guid>_<status>_<epoch>.snff
 */
void sn_fname_get_path(const FName *fname, char *path) {
  if (fname->cname.prefix[0] != '\0') {
    // prefix set
    snprintf(path, FNAME_PATH_LENGTH, "%s/%s%s_%s_%s.snff", fname->dir,
             fname->cname.prefix, fname->cname.guid.str, fname->cname.status,
             fname->cname.epoch.str);
  } else {
    // no prefix
    // Valgrind gives an error for this, but it is in fact okay
    snprintf(path, FNAME_PATH_LENGTH, "%s/%s_%s_%s.snff", fname->dir,
             fname->cname.guid.str, fname->cname.status,
             fname->cname.epoch.str);
  }
}
