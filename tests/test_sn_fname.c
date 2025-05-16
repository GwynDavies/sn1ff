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
 * object
 */

Test(sn_fname, init) {
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
 * dir
 */

Test(sn_fname, dir) {
  FName fname;
  sn_fname_init(&fname);

  sn_fname_set_dir(&fname, "/some/dir");

  char dir[FNAME_DIR_LENGTH_D];
  sn_fname_get_dir(&fname, dir);
  cr_assert_str_eq(dir, "/some/dir");
}

/*
 * tmp_path
 */

Test(sn_fname, get_path_tmp) {
  FName fname;
  sn_fname_init(&fname);
  sn_fname_set_dir(&fname, "/tmp/sn1ff");

  // Get guid

  char guid_str[CNAME_GUID_LENGTH_D];
  memset(guid_str, '\0', CNAME_GUID_LENGTH_D * sizeof(char));
  sn_cname_get_guid_str(&fname.cname, guid_str);

  // Get path

  char path[FNAME_PATH_LENGTH_D];
  memset(path, '\0', FNAME_PATH_LENGTH_D * sizeof(char));
  sn_fname_get_tmp_path(&fname, path);

  // Compare

  char expected_path[FNAME_PATH_LENGTH_D];
  memset(expected_path, '\0', FNAME_PATH_LENGTH_D * sizeof(char));
  snprintf(expected_path, FNAME_PATH_LENGTH, "%s/%s.snff", "/tmp/sn1ff",
           guid_str);

  cr_assert_str_eq(path, expected_path);
}

/*
 * get_path_no_prefix
 */

Test(sn_fname, get_path_no_prefix) {
  FName fname;
  sn_fname_init(&fname);
  sn_fname_set_dir(&fname, "/some/dir");
  sn_cname_set_status(&fname.cname, "ALRT");

  // Get guid

  char guid_str[CNAME_GUID_LENGTH_D];
  memset(guid_str, '\0', CNAME_GUID_LENGTH_D * sizeof(char));
  sn_cname_get_guid_str(&fname.cname, guid_str);

  // Get epoch

  char epoch_str[CNAME_EPOCH_LENGTH_D];
  memset(epoch_str, '\0', CNAME_EPOCH_LENGTH_D * sizeof(char));
  sn_cname_get_epoch_str(&fname.cname, epoch_str);

  // Get path

  char path[FNAME_PATH_LENGTH_D];
  memset(path, '\0', FNAME_PATH_LENGTH_D * sizeof(char));
  sn_fname_get_path(&fname, path);

  // Compare

  char expected_path[FNAME_PATH_LENGTH_D];
  memset(expected_path, '\0', FNAME_PATH_LENGTH_D * sizeof(char));
  snprintf(expected_path, FNAME_PATH_LENGTH, "%s/%s_%s_%s.snff", "/some/dir",
           guid_str, "ALRT", epoch_str);

  cr_assert_str_eq(path, expected_path);
}

/*
 * get_path_has_prefix
 */
Test(sn_fname, get_path_has_prefix) {
  FName fname;
  sn_fname_init(&fname);
  sn_fname_set_dir(&fname, "/some/dir");
  sn_cname_set_prefix(&fname.cname, ".hidden");
  sn_cname_set_status(&fname.cname, "WARN");

  // Get guid

  char guid_str[CNAME_GUID_LENGTH_D];
  memset(guid_str, '\0', CNAME_GUID_LENGTH_D * sizeof(char));
  sn_cname_get_guid_str(&fname.cname, guid_str);

  // Get epoch

  char epoch_str[CNAME_EPOCH_LENGTH_D];
  memset(epoch_str, '\0', CNAME_EPOCH_LENGTH_D * sizeof(char));
  sn_cname_get_epoch_str(&fname.cname, epoch_str);

  // Get path

  char actual_path[FNAME_PATH_LENGTH_D];
  memset(actual_path, '\0', FNAME_PATH_LENGTH_D * sizeof(char));
  sn_fname_get_path(&fname, actual_path);

  // Compare

  char expected_path[FNAME_PATH_LENGTH_D];
  memset(expected_path, '\0', FNAME_PATH_LENGTH_D * sizeof(char));
  snprintf(expected_path, FNAME_PATH_LENGTH, "%s/%s%s_%s_%s.snff", "/some/dir",
           ".hidden", guid_str, "WARN", epoch_str);

  cr_assert_str_eq(actual_path, expected_path,
                   "Actual path was not equal to expected path");
}
