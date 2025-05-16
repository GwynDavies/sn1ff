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

#include "cn_dir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int directory_exists(const char *path);

// Helper function to create and remove test directories
static void create_test_dir(const char *path) { mkdir(path, 0700); }

static void remove_test_dir(const char *path) { rmdir(path); }

static void create_test_file(const char *path) {
  FILE *f = fopen(path, "w");
  if (f)
    fclose(f);
}

static void remove_test_file(const char *path) { remove(path); }

Test(directory_exists_suite, returns_0_if_directory_exists) {
  const char *path = "/tmp/test_dir_exists";

  create_test_dir(path);
  int result = cn_dir_exists(path);
  remove_test_dir(path);

  cr_assert_eq(result, 0, "Expected directory_exists() to return 0, got %d",
               result);
}

Test(directory_exists_suite, returns_1_if_directory_does_not_exist) {
  const char *path = "/tmp/this_should_not_exist";

  int result = cn_dir_exists(path);

  cr_assert_eq(
      result, -1,
      "Expected directory_exists() to return 1 for non-existent path, got %d",
      result);
}

Test(directory_exists_suite, returns_2_if_path_is_a_file) {
  const char *path = "/tmp/test_file_exists";

  create_test_file(path);
  int result = cn_dir_exists(path);
  remove_test_file(path);

  cr_assert_eq(result, -2,
               "Expected directory_exists() to return 2 for file, got %d",
               result);
}
