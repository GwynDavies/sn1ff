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
#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>

Test(cn_fpath_get_name, test_with_full_path) {
  const char *path = "/home/user/documents/file.txt";
  char name[256];

  int result = cn_fpath_get_name(path, name);
  cr_assert_eq(result, CN_FPATH_GET_FNAME_SUCCESS,
               "Expected success, got error code %d", result);
  cr_assert_str_eq(name, "file.txt", "Expected file name 'file.txt', got '%s'",
                   name);
}

Test(cn_fpath_get_name, test_with_file_name_only) {
  const char *path = "file.txt";
  char name[256];

  int result = cn_fpath_get_name(path, name);
  cr_assert_eq(result, CN_FPATH_GET_FNAME_SUCCESS,
               "Expected success, got error code %d", result);
  cr_assert_str_eq(name, "file.txt", "Expected file name 'file.txt', got '%s'",
                   name);
}

Test(cn_fpath_get_name, test_with_trailing_slash) {
  const char *path = "/home/user/documents/";
  char name[256];

  int result = cn_fpath_get_name(path, name);
  cr_assert_eq(result, CN_FPATH_GET_FNAME_SUCCESS,
               "Expected success, got error code %d", result);
  cr_assert_str_eq(name, "documents",
                   "Expected file name 'documents', got '%s'", name);
}

Test(cn_fpath_get_name, test_with_multiple_nested_dirs) {
  const char *path = "/home/user/docs/anotherfolder/filename.txt";
  char name[256];

  int result = cn_fpath_get_name(path, name);
  cr_assert_eq(result, CN_FPATH_GET_FNAME_SUCCESS,
               "Expected success, got error code %d", result);
  cr_assert_str_eq(name, "filename.txt",
                   "Expected file name 'filename.txt', got '%s'", name);
}

Test(cn_fpath_get_name, test_with_null_path) {
  const char *path = NULL;
  char name[256];

  int result = cn_fpath_get_name(path, name);
  cr_assert_eq(result, CN_FPATH_GET_FNAME_INVALID_PATH,
               "Expected error code %d for NULL path", result);
}

Test(cn_fpath_get_name, test_with_null_name) {
  const char *path = "/home/user/docs/filename.txt";
  char *name = NULL;

  int result = cn_fpath_get_name(path, name);
  cr_assert_eq(result, CN_FPATH_GET_FNAME_INVALID_PATH,
               "Expected error code %d for NULL name", result);
}
