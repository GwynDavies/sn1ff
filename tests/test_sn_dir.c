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

#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cn_multistr.h"
#include "sn_dir.h"

// Optional: Declare the static function if you're testing it directly
int sn_dir_file_has_ext(const char *filename);

Test(sn_dir_file_has_ext, recognizes_valid_extension_case_insensitive) {
  cr_assert(sn_dir_file_has_ext("file.snff"));
  cr_assert(sn_dir_file_has_ext("file.SNFF"));
  cr_assert(sn_dir_file_has_ext("file.SnFf"));
}

Test(sn_dir_file_has_ext, rejects_invalid_extension) {
  cr_assert_not(sn_dir_file_has_ext("file.txt"));
  cr_assert_not(sn_dir_file_has_ext("file.snf"));
  cr_assert_not(sn_dir_file_has_ext("file.snffx"));
  // cr_assert_not(sn_dir_file_has_ext(".snff")); // corner case
}

Test(sn_dir_list_files, lists_only_snff_files) {
  const char *test_dir = "./test_snff_dir";
  mkdir(test_dir, 0700);

  FILE *f1 = fopen("./test_snff_dir/file1.snff", "w");
  fclose(f1);
  FILE *f2 = fopen("./test_snff_dir/file2.SNFF", "w");
  fclose(f2);
  FILE *f3 = fopen("./test_snff_dir/file3.txt", "w");
  fclose(f3);
  FILE *f4 = fopen("./test_snff_dir/another.snffx", "w");
  fclose(f4);

  MultiString ms;
  cn_multistr_init(&ms);

  int result = sn_dir_list_files(test_dir, &ms);
  cr_expect_eq(result, 0);

  cr_assert_eq(ms.num_strings, 2);

  const char *s0 = cn_multistr_getstr(&ms, 0);
  const char *s1 = cn_multistr_getstr(&ms, 1);

  bool valid =
      (strcmp(s0, "file1.snff") == 0 && strcmp(s1, "file2.SNFF") == 0) ||
      (strcmp(s0, "file2.SNFF") == 0 && strcmp(s1, "file1.snff") == 0);

  if (!valid) {
    cr_log_error("Expected file1.snff and file2.SNFF, got: '%s' and '%s'\n", s0,
                 s1);
  }
  cr_assert(valid);

  cn_multistr_free(&ms);

  // Cleanup
  unlink("./test_snff_dir/file1.snff");
  unlink("./test_snff_dir/file2.SNFF");
  unlink("./test_snff_dir/file3.txt");
  unlink("./test_snff_dir/another.snffx");
  rmdir(test_dir);
}

Test(sn_dir_list_files, handles_nonexistent_directory) {
  MultiString ms;
  cn_multistr_init(&ms);
  int result = sn_dir_list_files("./no_such_dir", &ms);
  cr_assert_eq(result, 1);
  cn_multistr_free(&ms);
}
