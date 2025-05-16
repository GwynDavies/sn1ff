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

#include "sn_fname.h"
#include <criterion/criterion.h>
#include <string.h>

/*
 * init
 */
Test(sn_cname, init) {
  FName fname;
  sn_fname_init(&fname);

  cr_assert_str_eq(fname.dir, "");
  cr_assert_str_eq(fname.cname.prefix, "");
  cr_assert_eq(strlen(fname.cname.guid.str), 36,
               "GUID should be 36 chars long");
  cr_assert_str_eq(fname.cname.status, "");
  cr_assert_eq(strlen(fname.cname.epoch.str), 10,
               "Epoch should be 10 chars long");
}

/*
 * prefix
 */

Test(sn_cname, set_and_get_prefix) {
  FName fname;
  sn_fname_init(&fname);

  char str[CNAME_PREFIX_LENGTH_D];
  memset(str, '\0', CNAME_PREFIX_LENGTH_D);

  sn_cname_set_prefix(&fname.cname, ".hidden");
  sn_cname_get_prefix(&fname.cname, str);
  cr_assert_str_eq(str, ".hidden");
}

/*
 * guid
 */

Test(sn_cname, get_guid_str) {
  FName fname;
  sn_fname_init(&fname);

  // Get string guid

  char str[CNAME_GUID_LENGTH_D];
  memset(str, '\0', CNAME_GUID_LENGTH_D);

  sn_cname_get_guid_str(&fname.cname, str);
  cr_assert_eq(strlen(str), 36, "GUID should be 36 chars long");

  // Get binary guid

  uuid_t guid_bin;
  sn_cname_get_guid_bin(&fname.cname, &guid_bin);
  char guid_str[37]; // UUIDs are 36 characters plus a null terminator
  uuid_unparse(guid_bin, guid_str);

  // Compare from string guid and from binary guid

  cr_assert_str_eq(str, guid_str);
}

/*
 * status
 */

Test(sn_cname, set_and_get_status) {
  FName fname;
  sn_fname_init(&fname);

  char str[CNAME_STATUS_LENGTH_D];
  memset(str, '\0', CNAME_STATUS_LENGTH_D);

  sn_cname_set_status(&fname.cname, "ALRT");
  sn_cname_get_status(&fname.cname, str);
  cr_assert_str_eq(str, "ALRT");
}

/*
 * epoch
 */

Test(sn_cname, get_epoch) {
  FName fname;
  sn_fname_init(&fname);

  // Get string epoch

  char str[CNAME_EPOCH_LENGTH_D];
  memset(str, '\0', CNAME_EPOCH_LENGTH_D);

  sn_cname_get_epoch_str(&fname.cname, str);
  cr_assert_eq(strlen(str), 10, "Epoch should be 10 chars long");

  // Get binary guid

  time_t epoch_bin;
  sn_cname_get_epoch_bin(&fname.cname, &epoch_bin);

  char epoch_str[CNAME_EPOCH_LENGTH_D];
  memset(epoch_str, '\0', CNAME_EPOCH_LENGTH_D);
  sprintf(epoch_str, "%ld", epoch_bin);

  // Compare from string epoch and from binary epoch

  cr_assert_str_eq(str, epoch_str);
}

/*
 * name
 */

Test(sn_cname, parse_name) {
  const char *name_str =
      "e2698951-0380-4af3-a6cc-144c82aa31a7_NONE_1742198614.snff";

  FName fname;
  sn_fname_init(&fname);

  int result = sn_cname_parse_name(name_str, &fname.cname);
  cr_assert_eq(result, 0);

  cr_assert_str_eq(fname.cname.prefix, "");
  cr_assert_str_eq(fname.cname.guid.str,
                   "e2698951-0380-4af3-a6cc-144c82aa31a7");
  cr_assert_str_eq(fname.cname.status, "NONE");
  cr_assert_str_eq(fname.cname.epoch.str, "1742198614");
}

Test(sn_cname, parse_guid_name) {
  const char *name_str = "e2698951-0380-4af3-a6cc-144c82aa31a7.snff";

  FName fname;
  sn_fname_init(&fname);

  int result = sn_cname_parse_guid_name(name_str, "NONE", &fname.cname);
  cr_assert_eq(result, 0);

  cr_assert_str_eq(fname.cname.guid.str,
                   "e2698951-0380-4af3-a6cc-144c82aa31a7");
  cr_assert_str_eq(fname.cname.prefix, "");
  cr_assert_str_eq(fname.cname.status, "NONE");
  cr_assert_eq(strlen(fname.cname.epoch.str), 10,
               "Epoch should be 10 chars long");
}
