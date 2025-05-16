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

#define _POSIX_C_SOURCE 200809L

#include "cn_time.h"
#include "cn_log.h"

/**
 * Get current epoch time
 *
 * @return  current epoch time
 *         -1 on error
 */
time_t cn_time_epoch(void) {
  // Get current epoch time

  time_t current_time = time(NULL);

  // Check if time retrieval was successful

  if (current_time == (time_t)(-1)) {
    // Warning - man page does not state if errno is set or not

    cn_log_msg(LOG_ERR, __func__,
               "'time_t' returned error (-1), strerror(errno) -> %m <-");
    // return 1;
    return -1;
  }

  return current_time;
}

/**
 * Check if the supplied epoch time value has expired (in the past)
 *
 * @param [i] epoch_time  epoch time value to check if its in the past
 * @return                true if the value is in the past, false otherwise
 */
bool cn_time_epoch_expired(const time_t epoch_time) {
  return (epoch_time < cn_time_epoch()) ? true : false;
}

/**
 * Get current epoch time, offset by a number of seconds. Typically
 * used, to get an epoch time in the future (e.g. for expiration
 * checking logic)
 *
 * @param [i] seconds  number of milliseconds to sleep
 *
 * @return  current epoch time offset by "seconds"
 *         -1 on error
 */
long cn_time_epoch_offset_snds(const int seconds) {
  time_t current_time = cn_time_epoch();

  if (current_time < 0) {
    return -1;
  }

  // Apply offset in seconds

  time_t time_with_offset = current_time + seconds;
  return (long)time_with_offset;
}

/**
 * Sleep for a number of milliseconds
 *
 * For example, 500 milliseconds will sleep for 1/2 a second
 *
 * @param milliseconds  Number of milliseconds to sleep
 */
void cn_time_sleep_millis(const long milli_seconds) {
  struct timespec ts;
  ts.tv_sec = milli_seconds / 1000; // Convert milliseconds to seconds
  ts.tv_nsec =
      (milli_seconds % 1000) * 1000000L; // Convert remainder to nanoseconds

  nanosleep(&ts, NULL);
}
