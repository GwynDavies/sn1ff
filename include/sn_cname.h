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

#ifndef SN_CNAME_H
#define SN_CNAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h> // Requires package

/*
 * sn1ff fname:
 *   - dir
 *   - cname
 *       - components:
 *           <guid>_<status>_<epoch>
 *   - path (not in struct):
 *       dir / cname
 */

#define CNAME_PREFIX_LENGTH 10
#define CNAME_PREFIX_LENGTH_D (CNAME_PREFIX_LENGTH + 1)

// On Linux, a GUID (Globally Unique Identifier), typically represented as
// a UUID (Universally Unique Identifier), is 36 characters long.
//
// This includes the hyphens that separate the different sections of the UUID.
//
// The general format looks like this:
//
//     xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
//
// Where each "x" is a hexadecimal character (0-9, a-f). So, including the 4
// hyphens, the total length is 36 characters.
//
#define CNAME_GUID_LENGTH 36
#define CNAME_GUID_LENGTH_D (CNAME_GUID_LENGTH + 1)

#define CNAME_STATUS_LENGTH 4
#define CNAME_STATUS_LENGTH_D (CNAME_STATUS_LENGTH + 1)

#define CNAME_EPOCH_LENGTH 10
#define CNAME_EPOCH_LENGTH_D (CNAME_EPOCH_LENGTH + 1)

#define CNAME_NAME_LENGTH                                                      \
  (CNAME_PREFIX_LENGTH + CNAME_GUID_LENGTH + CNAME_STATUS_LENGTH +             \
   CNAME_EPOCH_LENGTH)
#define CNAME_NAME_LENGTH_D (CNAME_NAME_LENGTH + 1)

typedef struct {
  char str[CNAME_GUID_LENGTH_D];
  uuid_t bin;
} Guid;

typedef struct {
  char str[CNAME_EPOCH_LENGTH_D];
  time_t bin;
} Epoch;

typedef struct {
  char prefix[CNAME_PREFIX_LENGTH_D];
  Guid guid;
  char status[CNAME_STATUS_LENGTH_D];
  Epoch epoch;
} CName;

// prefix

void sn_cname_set_prefix(CName *cname, const char *prefix);
void sn_cname_get_prefix(CName *cname, char *prefix);

// guid

void sn_cname_get_guid_str(CName *cname, char *guid_str);
void sn_cname_get_guid_bin(CName *cname, uuid_t *guid_bin);

// status

void sn_cname_set_status(CName *cname, const char *status);
void sn_cname_get_status(CName *cname, char *status);

// epoch

void sn_cname_get_epoch_str(CName *cname, char *epoch_str);
void sn_cname_get_epoch_bin(CName *cname, time_t *epoch_bin);
void sn_cname_incr_epoch_bin(CName *cname, int incr_minutes);

// parse

int sn_cname_parse_name(const char *name_str, CName *name);
int sn_cname_parse_guid_name(const char *guid_name_str, const char *status_str,
                             CName *name);

#endif
