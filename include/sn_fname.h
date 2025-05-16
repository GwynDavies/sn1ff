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

#ifndef SN_FNAME_H
#define SN_FNAME_H

#include "sn_cname.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h> // Requires package

/*
 * Sn1ff fname:
 *   - dir
 *   - cname
 *       - components:
 *           <id>_<status>_<epoch>
 *   - path (not in struct):
 *       dir / cname
 */

#define FNAME_DIR_LENGTH 512
#define FNAME_DIR_LENGTH_D (FNAME_DIR_LENGTH + 1)

#define FNAME_PATH_LENGTH                                                      \
  (FNAME_DIR_LENGTH + CNAME_NAME_LENGTH + 6 + 3) // 6 for ".sn1ff"
                                                 // 3 for / and _'s
                                                 //   "%s/%s%s_%s_%s.snff"
#define FNAME_PATH_LENGTH_D (FNAME_PATH_LENGTH + 1)

typedef struct {
  char dir[FNAME_DIR_LENGTH_D];
  CName cname;
} FName;

// object

int sn_fname_init(FName *fname);
void sn_fname_dump(FName *fname);

// dir

void sn_fname_set_dir(FName *fname, const char *dir);
void sn_fname_get_dir(const FName *fname, char *dir);

// path

void sn_fname_get_tmp_path(const FName *fname, char *tmp_path);
void sn_fname_get_path(const FName *fname, char *path);

#endif
