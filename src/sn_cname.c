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

#include "sn_cname.h"
#include "cn_log.h"
#include "sn_status.h"

/*
 * Sn1ff fname:
 *   - dir
 *   - cname
 *       - components:
 *           <id>_<status>_<epoch>
 *   - path: (not in struct)
 *       dir / name
 */

/*----------------------------------------------------------------.
 |                                                                |
 | Prefix                                                         |
 |                                                                |
 '----------------------------------------------------------------*/

void sn_cname_set_prefix(CName *cname, const char *prefix) {
  strncpy(cname->prefix, prefix, CNAME_PREFIX_LENGTH);
}

void sn_cname_get_prefix(CName *cname, char *prefix) {
  strncpy(prefix, cname->prefix, CNAME_PREFIX_LENGTH);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Guid                                                           |
 |                                                                |
 '----------------------------------------------------------------*/

void sn_cname_get_guid_str(CName *cname, char *guid_str) {
  strncpy(guid_str, cname->guid.str, CNAME_GUID_LENGTH);
}

void sn_cname_get_guid_bin(CName *cname, uuid_t *guid_bin) {
  memcpy(*guid_bin, cname->guid.bin, sizeof(uuid_t));
}

/*----------------------------------------------------------------.
 |                                                                |
 | Status                                                         |
 |                                                                |
 '----------------------------------------------------------------*/

void sn_cname_set_status(CName *cname, const char *status) {
  strncpy(cname->status, status, CNAME_STATUS_LENGTH);
}

void sn_cname_get_status(CName *cname, char *status) {
  strncpy(status, cname->status, CNAME_STATUS_LENGTH);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Epoch                                                          |
 |                                                                |
 '----------------------------------------------------------------*/

void sn_cname_get_epoch_str(CName *cname, char *epoch_str) {
  strncpy(epoch_str, cname->epoch.str, CNAME_EPOCH_LENGTH);
}

void sn_cname_get_epoch_bin(CName *cname, time_t *epoch_bin) {
  *epoch_bin = cname->epoch.bin;
}

void sn_cname_incr_epoch_bin(CName *cname, int incr_minutes) {
  cname->epoch.bin +=
      incr_minutes * 60; // epoch is in seconds, so * 60 to scale to minutes

  // Now update epoch.str to match epoch.bin

  cname->epoch.str[0] = '\0';
  memset(&cname->epoch.str, '\0', CNAME_EPOCH_LENGTH_D);
  sprintf(cname->epoch.str, "%ld", cname->epoch.bin);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Parse different file name forms                                |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Parse sn1ff filename into an object representation, so the caller
 * can then access, or change the name component values
 *
 * @param name_str    i    sn1ff file name in format -
 * <guid>_<status>_<epoch>.snff
 * @param name        i/o  object representation of the file name
 * @return                 status code:
 *                           0  success
 *                          -1  error parsing file name
 *                          -2  error parsing GUID
 */
int sn_cname_parse_name(const char *name_str, CName *cname) {
  char guid_str[CNAME_GUID_LENGTH_D];
  uuid_t guid_bin;

  char status[CNAME_STATUS_LENGTH_D];

  char epoch_str[CNAME_EPOCH_LENGTH_D];
  time_t epoch_bin;

  int ret = 0;

  // Use sscanf to split the string based on the format
  // <GUID>_<STATUS>_<EPOCH>.snff

  ret = sscanf(name_str, "%36s_%4s_%10s.snff", guid_str, status, epoch_str);
  if (ret != 3) {
    cn_log_msg(LOG_ERR, __func__, "Could not parse name_str -> %s <-",
               name_str);
    return -1;
  }

  // GUID

  strncpy(cname->guid.str, guid_str, CNAME_GUID_LENGTH);
  if (uuid_parse(guid_str, guid_bin) != 0) {
    cn_log_msg(LOG_ERR, __func__, "Could not parse guid_str -> %s <-",
               guid_str);
    return -2;
  }
  memcpy(cname->guid.bin, guid_bin, sizeof(uuid_t));

  // Status

  strncpy(cname->status, status, CNAME_STATUS_LENGTH);

  // Epoch

  strncpy(cname->epoch.str, epoch_str, CNAME_EPOCH_LENGTH);
  epoch_bin = (time_t)strtol(epoch_str, NULL, 10);
  cname->epoch.bin = epoch_bin;

  return 0;
}

/**
 * Parse sn1ff guid only filename:
 *   <GUID>.snff
 * ... into a Name object
 *
 * Params:
 *   guid_name_str - the sn1ff guid only filename
 *   status_str    - the status value to use
 *   cname         - the returned CName object, built from GUID, Status and
 * Epoch set
 *
 * @return  0 success
 *         -1 NULL value in argument
 *         -2 error extracting GUID string
 *         -3 error parsing GUID
 *         -4 error in status
 *         -5 error getting current epoch time
 */
int sn_cname_parse_guid_name(const char *guid_name_str, const char *status_str,
                             CName *cname) {
  char guid_str[CNAME_GUID_LENGTH_D];
  uuid_t guid_bin;

  if (guid_name_str == NULL) {
    cn_log_msg(LOG_ERR, __func__, "Argument guid_name_str is NULL");
    return -1;
  }

  if (status_str == NULL) {
    cn_log_msg(LOG_ERR, __func__, "Argument status_str is NULL");
    return -1;
  }

  if (cname == NULL) {
    cn_log_msg(LOG_ERR, __func__, "Argument cname is NULL");
    return -1;
  }

  // Extract GUID from GUID name string
  //
  // <GUID>.snff

  int result = sscanf(guid_name_str, "%36s.snff", guid_str);
  if (result != 1) {
    cn_log_msg(LOG_ERR, __func__,
               "Could extract GUID from guid_name_str -> %s <-", guid_name_str);
    return -2;
  }

  // Prefix

  cname->prefix[0] = '\0';

  // Parse GUID from guid_str

  strncpy(cname->guid.str, guid_str, CNAME_GUID_LENGTH);
  if (uuid_parse(guid_str, guid_bin) != 0) {
    cn_log_msg(LOG_ERR, __func__, "Could not parse GUID from guid_str -> %s <-",
               guid_str);
    return -3; // Error parsing GUID
  }
  memcpy(cname->guid.bin, guid_bin, sizeof(uuid_t));

  // Extract status  value
  if (sn_status_isvalid(status_str) != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Status value not recognized in 'status_str' ->%s<-",
               status_str);
    return -4;
  }

  strncpy(cname->status, status_str, CNAME_STATUS_LENGTH);

  // Get current epoch time value

  cname->epoch.bin = time(NULL);
  if (cname->epoch.bin == -1) {
    // Warning - man page does not state if errno is set or not

    cn_log_msg(LOG_ERR, __func__,
               "'time_t' returned error (-1), strerror(errno) -> %m <-");
    return -5;
  }
  sprintf(cname->epoch.str, "%ld", cname->epoch.bin);

  return 0;
}
