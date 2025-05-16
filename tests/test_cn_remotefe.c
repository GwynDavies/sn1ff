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

#include "cn_remotefe.h"
#include <criterion/criterion.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Mocking mode enum
enum { MOCK_NONE, MOCK_SUCCESS, MOCK_TIMEOUT };

int mock_mode = MOCK_NONE;

// Global mock for execvp
int execvp(const char *file __attribute__((unused)),
           char *const argv[] __attribute__((unused))) {
  if (mock_mode == MOCK_SUCCESS) {
    exit(0); // Simulate successful scp
  } else if (mock_mode == MOCK_TIMEOUT) {
    sleep(10); // Simulate hanging scp
    exit(1);
  }
  return -1; // Default
}

// Test: SCP success leads to file deletion
Test(cn_remotefe, scp_success_deletes_file) {
  const char *filename = "/tmp/testfile_scp_success.txt";
  FILE *f = fopen(filename, "w");
  cr_assert_not_null(f, "Failed to create temp file for test");
  fprintf(f, "test content\n");
  fclose(f);

  mock_mode = MOCK_SUCCESS;

  int result = cn_remotefe_scp(filename, "user@host:/remote/path", 5);
  cr_expect_eq(result, 0, "Expected cn_remotefe_scp to return 0");

  cr_expect_eq(access(filename, F_OK), -1, "Expected file to be deleted");
}

// Test: SCP timeout prevents file deletion
Test(cn_remotefe, scp_timeout_kills_child) {
  const char *filename = "/tmp/testfile_scp_timeout.txt";
  FILE *f = fopen(filename, "w");
  cr_assert_not_null(f);
  fprintf(f, "test timeout\n");
  fclose(f);

  mock_mode = MOCK_TIMEOUT;

  int result = cn_remotefe_scp(filename, "user@host:/remote/path", 1);
  cr_expect_eq(result, 0,
               "Expected cn_remotefe_scp to return 0 even on timeout");

  cr_expect_eq(access(filename, F_OK), 0,
               "Expected file NOT to be deleted due to timeout");
}
