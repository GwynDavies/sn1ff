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

#include "cn_multistr.h"
#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>

Test(cn_multistr, init_and_free) {
  MultiString ms;
  cn_multistr_init(&ms);

  cr_assert_not_null(ms.strings);
  cr_assert_eq(ms.total_length, 0);
  cr_assert_eq(ms.num_strings, 0);
  cr_assert(ms.capacity >= 256);

  cn_multistr_free(&ms);
  cr_assert_null(ms.strings);
  cr_assert_eq(ms.total_length, 0);
  cr_assert_eq(ms.num_strings, 0);
  cr_assert_eq(ms.capacity, 0);
}

Test(cn_multistr, append_and_getstr) {
  MultiString ms;
  cn_multistr_init(&ms);

  cn_multistr_append(&ms, "foo");
  cn_multistr_append(&ms, "bar");
  cn_multistr_append(&ms, "baz");

  cr_assert_eq(ms.num_strings, 3);
  cr_assert_str_eq(cn_multistr_getstr(&ms, 0), "foo");
  cr_assert_str_eq(cn_multistr_getstr(&ms, 1), "bar");
  cr_assert_str_eq(cn_multistr_getstr(&ms, 2), "baz");
  cr_assert_null(cn_multistr_getstr(&ms, 3));

  cn_multistr_free(&ms);
}

Test(cn_multistr, serialize_and_deserialize) {
  MultiString ms1, ms2;
  cn_multistr_init(&ms1);
  cn_multistr_append(&ms1, "one");
  cn_multistr_append(&ms1, "two");
  cn_multistr_append(&ms1, "three");

  size_t buffer_size = cn_multistr_reqd_buffsize(&ms1);
  char *buffer = malloc(buffer_size);

  size_t written = cn_multistr_serialize(&ms1, buffer);
  cr_assert_eq(written, buffer_size);

  cn_multistr_deserialize(&ms2, buffer);

  cr_assert_eq(ms2.num_strings, ms1.num_strings);
  cr_assert_eq(ms2.total_length, ms1.total_length);
  for (size_t i = 0; i < ms1.num_strings; ++i) {
    cr_assert_str_eq(cn_multistr_getstr(&ms1, i), cn_multistr_getstr(&ms2, i));
  }

  free(buffer);
  cn_multistr_free(&ms1);
  cn_multistr_free(&ms2);
}

Test(cn_multistr, required_buffer_size) {
  MultiString ms;
  cn_multistr_init(&ms);

  cn_multistr_append(&ms, "alpha");
  cn_multistr_append(&ms, "beta");

  size_t expected =
      sizeof(ms.total_length) + sizeof(ms.num_strings) + ms.total_length;
  cr_assert_eq(cn_multistr_reqd_buffsize(&ms), expected);

  cn_multistr_free(&ms);
}
