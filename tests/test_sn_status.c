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

#include "sn_status.h"
#include <criterion/criterion.h>

Test(sn_status, valid_status_values) {
  cr_assert_eq(sn_status_isvalid("ALRT"), 0, "ALRT should be valid");
  cr_assert_eq(sn_status_isvalid("WARN"), 0, "WARN should be valid");
  cr_assert_eq(sn_status_isvalid("OKAY"), 0, "OKAY should be valid");
  cr_assert_eq(sn_status_isvalid("NONE"), 0, "NONE should be valid");
}

Test(sn_status, invalid_status_values) {
  cr_assert_eq(sn_status_isvalid("alert"), -1,
               "'alert' should be invalid (case-sensitive)");
  cr_assert_eq(sn_status_isvalid("WARNING"), -1, "'WARNING' should be invalid");
  cr_assert_eq(sn_status_isvalid(""), -1, "Empty string should be invalid");
  cr_assert_eq(sn_status_isvalid("1234"), -1, "'1234' should be invalid");

  cr_assert_eq(sn_status_isvalid(NULL), -2, "NULL should be invalid");
}
