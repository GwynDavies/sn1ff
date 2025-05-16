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

#include "cn_string.h"

void cn_string_split(char *input, char *output[SPLIT_STRING_MAX_VALUES]) {
  int i = 0;
  char *token = strtok(input, " "); // Split by space

  // Loop through and save the tokens into the output array

  while (token != NULL && i < SPLIT_STRING_MAX_VALUES) {
    output[i++] = token;       // Assign token to the output array
    token = strtok(NULL, " "); // Get the next token
  }
}

/**
 * Check if a string (str), "starts with" a prefix
 *
 * Returns 1 - If the string starts with the prefix
 *         0 - Otherwise
 */
int cn_string_starts_with(const char *str, const char *prefix) {
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

/*
 * A safe version of strncpy
 *
 * Copies up to dest_sz - 1 characters from src into dst, null-terminating the
 * result
 *
 * @param dst     Is the destination string to receive the v    size_t src_len =
 * 0;
 * @param dst_sz  Is the size of dst in char-size, not bytes
 * @param src     Is the source string value to copy
 * @return         0 success
 *                -1 src is NULL
 *                -2 dst is NULL
 *                -3 dst_sz is < 1
 */
int cn_string_cp(char *dst, size_t dst_sz, const char *src) {
  size_t src_len = 0;
  if (src == NULL)
    return -1;
  if (dst == NULL)
    return -2;
  if (dst_sz < 1)
    return -3;

  while (src[src_len] != '\0') {
    if (src_len + 1 < dst_sz) {
      dst[src_len] = src[src_len];
    }
    src_len++;
  }

  int null_index = (src_len < dst_sz - 1) ? src_len : dst_sz - 1;
  dst[null_index] = '\0';

  return 0;
}

void cn_string_trim_newline(char *str) {
  size_t len = strlen(str);
  if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
    str[len - 1] = '\0';
  }
}
