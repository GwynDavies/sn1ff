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

#include "cn_string.h"
#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>

// Test cn_string_split function
Test(cn_string_split, basic_split) {
  char input[] = "Hello world test string";
  char *output[SPLIT_STRING_MAX_VALUES] = {NULL};

  cn_string_split(input, output);

  // Check that the tokens are split correctly
  cr_assert_str_eq(output[0], "Hello");
  cr_assert_str_eq(output[1], "world");
  cr_assert_str_eq(output[2], "test");
  // cr_assert_str_eq(output[3], "string");
  // cr_assert_null(output[4]);  // Should be NULL after the last token
}

Test(cn_string_split, split_with_limit) {
  char input[] = "This is a test of the split function";
  char *output[SPLIT_STRING_MAX_VALUES] = {NULL};

  cn_string_split(input, output);

  // Check that the function respects the SPLIT_STRING_MAX_VALUES limit
  cr_assert_str_eq(output[0], "This");
  cr_assert_str_eq(output[1], "is");
  cr_assert_str_eq(output[2], "a");
  // cr_assert_str_eq(output[3], "test");
  // cr_assert_null(output[4]);  // Should be NULL if SPLIT_STRING_MAX_VALUES
  // limit is reached
}

Test(cn_string_split, empty_input) {
  char input[] = "";
  char *output[SPLIT_STRING_MAX_VALUES] = {NULL};

  cn_string_split(input, output);

  // No tokens should be found
  cr_assert_null(output[0]);
}

Test(cn_string_split, input_with_only_spaces) {
  char input[] = "     ";
  char *output[SPLIT_STRING_MAX_VALUES] = {NULL};

  cn_string_split(input, output);

  // No tokens should be found as input consists only of spaces
  cr_assert_null(output[0]);
}

// Test cn_string_starts_with function
Test(cn_string_starts_with, starts_with_prefix) {
  const char *str = "Hello, world!";
  const char *prefix = "Hello";

  // Test that the string starts with the given prefix
  cr_assert_eq(cn_string_starts_with(str, prefix), 1);
}

Test(cn_string_starts_with, does_not_start_with_prefix) {
  const char *str = "Hello, world!";
  const char *prefix = "world";

  // Test that the string does not start with the given prefix
  cr_assert_eq(cn_string_starts_with(str, prefix), 0);
}

Test(cn_string_starts_with, empty_string) {
  const char *str = "";
  const char *prefix = "Hello";

  // Test that empty string does not start with any prefix
  cr_assert_eq(cn_string_starts_with(str, prefix), 0);
}

Test(cn_string_starts_with, empty_prefix) {
  const char *str = "Hello, world!";
  const char *prefix = "";

  // Test that an empty prefix matches the beginning of the string
  cr_assert_eq(cn_string_starts_with(str, prefix), 1);
}
