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

#include "cn_fpath.h"
#include "cn_host.h"
#include "sn_file.h"
#include "sn_fname.h"
#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEST_TMP_DIR "/tmp/test_sn1ff"
#define TEST_FILE_PATH TEST_TMP_DIR "/test.snff"

// Create dummy directory
static void setup_test_dir(void) { mkdir(TEST_TMP_DIR, 0777); }

// Cleanup after test
static void teardown_test_dir(void) {
  unlink(TEST_FILE_PATH);
  rmdir(TEST_TMP_DIR);
}

Test(sn_file, write_header_valid, .init = setup_test_dir,
     .fini = teardown_test_dir) {
  FILE *f = fopen(TEST_FILE_PATH, "w+");
  cr_assert_not_null(f);

  HEADER hdr = {.host = "testhost",
                .ipv4 = "127.0.0.1",
                .timestamp = "2025-04-13T12:00:00Z"};

  int result = sn_file_write_header(f, &hdr);
  cr_assert_eq(result, 0);

  rewind(f);
  char buffer[256];
  fgets(buffer, sizeof(buffer), f);
  cr_assert(strncmp(buffer, "App: sn1ff", 10) == 0);

  fclose(f);
}

Test(sn_file, write_header_null_file, .init = setup_test_dir) {
  HEADER hdr = {0};
  int result = sn_file_write_header(NULL, &hdr);
  cr_assert_eq(result, 1);
}

Test(sn_file, write_header_null_hdr, .init = setup_test_dir) {
  FILE *f = fopen(TEST_FILE_PATH, "w");
  cr_assert_not_null(f);
  int result = sn_file_write_header(f, NULL);
  cr_assert_eq(result, 2);
  fclose(f);
}

Test(sn_file, begin_creates_file, .init = setup_test_dir,
     .fini = teardown_test_dir) {
  char path[256] = {0};
  int result = sn_file_begin(path);
  cr_assert_eq(result, 0);

  FILE *f = fopen(path, "r");
  cr_assert_not_null(f);

  char line[128];
  fgets(line, sizeof(line), f);
  cr_assert(strncmp(line, "App: sn1ff", 10) == 0);
  fclose(f);

  unlink(path);
}

Test(sn_file, read_and_parse_file, .init = setup_test_dir,
     .fini = teardown_test_dir) {
  // Manually create a valid snff file
  FILE *f = fopen(TEST_FILE_PATH, "w");
  fprintf(f, "MonTTY\n");
  fprintf(f, "Host: testhost\n");
  fprintf(f, "IPv4: 127.0.0.1\n");
  fprintf(f, "At: 2025-04-13T12:00:00Z\n\n\n");
  fprintf(f, "Line 1\n");
  fprintf(f, "Line 2\n");
  fclose(f);

  FILE_DATA fd;
  int res = sn_file_read(TEST_FILE_PATH, &fd);
  cr_assert_eq(res, 0);
  cr_assert_str_eq(fd.header.host, "testhost");
  cr_assert_str_eq(fd.header.ipv4, "127.0.0.1");
  cr_assert_str_eq(fd.header.timestamp, "2025-04-13T12:00:00Z");
  // TODO - fix this
  // cr_assert_eq(fd.body_lines, 2);
}

Test(sn_file, delete_file_safely, .init = setup_test_dir) {
  FILE *f = fopen(TEST_FILE_PATH, "w");
  fprintf(f, "Temporary file\n");
  fclose(f);

  sn_file_delete(TEST_TMP_DIR, "test.snff");

  char deleted_path[256];
  snprintf(deleted_path, sizeof(deleted_path), "%s/.deleted.test.snff",
           TEST_TMP_DIR);
  struct stat st;
  int deleted = stat(deleted_path, &st);
  cr_assert(deleted != 0); // file should be removed
}
