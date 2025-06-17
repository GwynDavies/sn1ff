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

#include "cn_multistr.h"
#include "cn_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 256

/**
 * Initialize "multi string" for use
 *
 * param(s):
 *     ms - Multi string to be initialized, also returns
 *
 * return:
 *     None
 */
void cn_multistr_init(MultiString *ms) {
  ms->total_length = 0;
  ms->num_strings = 0;
  ms->strings = malloc(INITIAL_CAPACITY * sizeof(char));
  ms->capacity = INITIAL_CAPACITY;
  ms->strings[0] = '\0';
}

/**
 * Free and "Zero out" resources
 */
void cn_multistr_free(MultiString *ms) {
  if (ms->strings) {
    free(ms->strings);
  }
  ms->strings = NULL;
  ms->capacity = ms->total_length = ms->num_strings = 0;
}

void cn_multistr_append(MultiString *ms, const char *str) {
  size_t str_len = strlen(str);

  if (ms->total_length + str_len + 1 > ms->capacity) {
    ms->capacity = (ms->total_length + str_len + 1) * 2;
    ms->strings = realloc(ms->strings, ms->capacity);
  }

  if (ms->num_strings > 0) {
    ms->strings[ms->total_length] = '\0';
  }

  strcpy(ms->strings + ms->total_length, str);
  ms->total_length += str_len + 1;
  ms->num_strings++;
}

const char *cn_multistr_getstr(MultiString *ms, size_t index) {
  if (index >= ms->num_strings) {
    return NULL;
  }

  size_t offset = 0;
  for (size_t i = 0; i < index; ++i) {
    offset += strlen(ms->strings + offset) + 1;
  }

  return ms->strings + offset;
}

size_t cn_multistr_serialize(MultiString *ms, char *buffer) {
  size_t offset = 0;

  // Serialize ms->total_length

  memcpy(buffer + offset, &ms->total_length, sizeof(ms->total_length));
  offset += sizeof(ms->total_length);

  // Serialize ms->num_strings

  memcpy(buffer + offset, &ms->num_strings, sizeof(ms->num_strings));
  offset += sizeof(ms->num_strings);

  // Serialize ms->strings

  memcpy(buffer + offset, ms->strings, ms->total_length);
  return offset + ms->total_length;
}

/**
 * De-serialize buffer into MultiString
 */
int cn_multistr_deserialize(MultiString *ms, const char *buffer) {
  size_t offset = 0;

  // De-serialize ms->total_length

  memcpy(&ms->total_length, buffer + offset, sizeof(ms->total_length));
  offset += sizeof(ms->total_length);

  // De-serialize ms->num_strings

  memcpy(&ms->num_strings, buffer + offset, sizeof(ms->num_strings));
  offset += sizeof(ms->num_strings);

  // De-serialize ms->strings

  ms->strings = malloc(ms->total_length);
  memcpy(ms->strings, buffer + offset, ms->total_length);

  ms->capacity = ms->total_length;

  return 0;
}

size_t cn_multistr_reqd_buffsize(MultiString *ms) {
  size_t required_buffer_size =
      sizeof(ms->total_length) + sizeof(ms->num_strings) + ms->total_length;

  return required_buffer_size;
}

void cn_multistr_tostring(MultiString *ms) {
  cn_log_msg(LOG_DEBUG, __func__, "Size                      Value\n");
  cn_log_msg(LOG_DEBUG, __func__, "ms.total_length : %zu     %zu\n",
             sizeof(ms->total_length), ms->total_length);
  cn_log_msg(LOG_DEBUG, __func__, "ms.num_strings  : %zu     %zu\n",
             sizeof(ms->num_strings), ms->num_strings);
  cn_log_msg(LOG_DEBUG, __func__, "ms.strings      : n/a     ");
  for (size_t i = 0; i < ms->total_length; i++)
    cn_log_msg(LOG_DEBUG, __func__, "%c", ms->strings[i]);
  cn_log_msg(LOG_DEBUG, __func__, "\n");
  cn_log_msg(LOG_DEBUG, __func__, "ms.capacity     : %zu     %zu\n",
             sizeof(ms->capacity), ms->capacity);
}
