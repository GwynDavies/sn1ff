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

#define _POSIX_C_SOURCE 200809L // Enable POSIX 2008 (clock_gettime support)

#include "cn_time.h"
#include <criterion/criterion.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// Test case for cn_time_epoch
Test(cn_time, test_epoch) {
  time_t current_time = cn_time_epoch();
  time_t system_time = time(NULL);
  cr_assert_neq(current_time, (time_t)(-1), "Epoch time returned an error");
  cr_assert_eq(
      current_time, system_time,
      "The current epoch time is equal to system time which shouldn't happen");
}

// Test case for cn_time_epoch_expired
Test(cn_time, test_epoch_expired) {
  time_t future_time = cn_time_epoch_offset_snds(
      10); // Get epoch time in the future (current time + 10 seconds)
  time_t past_time = cn_time_epoch_offset_snds(
      -10); // Get epoch time in the past (current time - 10 seconds)

  // Test for future time
  cr_assert_eq(cn_time_epoch_expired(future_time), false,
               "Epoch time in the future should not be expired");

  // Test for past time
  cr_assert_eq(cn_time_epoch_expired(past_time), true,
               "Epoch time in the past should be expired");
}

// Test case for cn_time_epoch_offset_snds
Test(cn_time, test_epoch_offset_snds) {
  time_t current_time = cn_time_epoch();
  long offset = 10;
  long new_time = cn_time_epoch_offset_snds(offset);

  cr_assert_eq(new_time, (long)(current_time + offset),
               "The epoch time with offset is incorrect");
}

// Test case for cn_time_sleep_millis
Test(cn_time, test_sleep_millis) {
  struct timespec start_time, end_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  // Sleep for 500 milliseconds (0.5 seconds)
  cn_time_sleep_millis(500);

  clock_gettime(CLOCK_MONOTONIC, &end_time);

  long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 +
                      (end_time.tv_nsec - start_time.tv_nsec) / 1000000L;

  cr_assert(
      elapsed_time >= 500,
      "The elapsed time should be greater than or equal to 500 milliseconds");
  // cr_assert_gte(elapsed_time, 500, "The elapsed time should be greater than
  // or equal to 500 milliseconds");
}
