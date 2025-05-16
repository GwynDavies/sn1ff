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

#include "sn_fpath.h"
#include <criterion/criterion.h>
#include <regex.h>

Test(sn_fpath, valid_status_values) {
  // char arg_f = "123e4567-e89b-12d3-a456-426614174000_ALRT_1745949432.snff"
  char arg_f[] = "/tmp/sn1ff/123e4567-e89b-12d3-a456-426614174000.snff";
  char arg_s[] = "ALRT";
  char arg_d[] = "/home/chroot/sn1ff/uploads";
  int arg_t = 1;
  char new_dest[256];

  // The expected value will look like this, but the TTL (here 1746739447) is
  // calculated dynamically, using current epoch time plus the arg_t offset
  // added to it:
  //
  //   /home/chroot/sn1ff/uploads/123e4567-e89b-12d3-a456-426614174000_ALRT_1746739447.snff
  //
  // So we will assert using a regex
  //

  sn_fpath_genfull(arg_f, arg_s, arg_d, arg_t, new_dest);

  regex_t regex;
  const char *pattern =
      "^/home/chroot/sn1ff/uploads/"
      "[0-9a-fA-F\\-]+_(ALRT|WARN|OKAY|NONE)_174[0-9]{7}\\.snff$";

  int ret = regcomp(&regex, pattern, REG_EXTENDED);
  cr_assert_eq(ret, 0, "Failed to compile regex");

  ret = regexec(&regex, new_dest, 0, NULL, 0);
  regfree(&regex);

  cr_assert_eq(ret, 0, "The string did not match the expected pattern");
}
