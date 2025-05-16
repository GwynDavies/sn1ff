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

/*
 * Level         Value
 *
 *  LOG_EMERG     0
 *  LOG_ALERT     1
 *  LOG_CRIT      2
 *  LOG_ERR       3
 *  LOG_WARNING   4
 *  LOG_NOTICE    5
 *  LOG_INFO      6
 */

#define _POSIX_C_SOURCE 200809L

#include "cn_log.h"

int CURRENT_MIN_LEVEL = LOG_INFO; // Default log level

void cn_log_open(const char *app_name, const int min_log_level) {
  CURRENT_MIN_LEVEL = min_log_level;
  openlog(app_name, LOG_PID | LOG_CONS, LOG_USER);
}

void cn_log_close(void) { closelog(); }

/**
 * Log message to syslog
 *
 * Usage:        log_message(LOG_DEBUG, __func__, "User %s logged in",
 * username);
 *
 * @param level  Is the syslog level, e.g. LOG_INFO
 * @param func   Is the function name of the caller, use __func__
 * @param fmt    Is the format string
 * @param
 */
void cn_log_msg(int level, const char *func, const char *fmt, ...) {
  if (level > CURRENT_MIN_LEVEL)
    return;

  char formatted[1024];

  va_list args;
  va_start(args, fmt);
  vsnprintf(formatted, sizeof(formatted), fmt, args);
  va_end(args);

  syslog(level, "[%s] %s", func, formatted);
}
