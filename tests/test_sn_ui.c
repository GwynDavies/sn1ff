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

#include "sn_file.h"
#include "sn_ui.h"
#include <criterion/criterion.h>
#include <criterion/logging.h>

// Dummy test to check basic draw behavior
Test(sn_ui, draw_basic) {
  FILE_DATA file = {0};

  // Fill in some mock data
  strncpy(file.header.host, "test-host", sizeof(file.header.host));
  strncpy(file.header.ipv4, "127.0.0.1", sizeof(file.header.ipv4));
  strncpy(file.header.timestamp, "2025-04-13T12:00:00Z",
          sizeof(file.header.timestamp));
  file.body_lines = 3;
  strncpy(file.body[0], "This is line one.", SN_FILE_MAX_BODY_LENGTH);
  strncpy(file.body[1], "This is line two.", SN_FILE_MAX_BODY_LENGTH);
  strncpy(file.body[2], "This is line three.", SN_FILE_MAX_BODY_LENGTH);

  // Call the function under test
  sn_ui_draw(&file);

  // No assertion: we're just making sure it doesn't crash
  cr_log_info("sn_ui_draw executed without crash.\n");
}
