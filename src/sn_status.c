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

#include "sn_status.h"
#include "cn_log.h"

/**
 * Check status value is valid:
 *  ALRT|WARN|OKAY|NONE
 *
 * @param value  status value to check
 *
 * @return  0 status value is valid
 *         -1 status value is invalid
 *         -2 status value is NULL
 */
int sn_status_isvalid(const char *value) {
  if (value == NULL) {
    cn_log_msg(LOG_ERR, __func__, "Argument 'value' is NULL");
    return -2;
  }

  if (strcmp(value, "ALRT") != 0 && strcmp(value, "WARN") != 0 &&
      strcmp(value, "OKAY") != 0 && strcmp(value, "NONE") != 0) {
    return -1;
  }

  return 0;
}
