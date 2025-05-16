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

#ifndef CN_MULTISTR_H
#define CN_MULTISTR_H

#include <stddef.h>

typedef struct {
  size_t total_length; // Total length in bytes of all the strings together
  size_t num_strings;  // Number of strings
  char *strings;       // Buffer to hold all the characters of the multi-string
  size_t capacity;     // The capacity of the strings buffer
} MultiString;

void cn_multistr_init(MultiString *ms);

void cn_multistr_free(MultiString *ms);

void cn_multistr_append(MultiString *ms, const char *str);

const char *cn_multistr_getstr(MultiString *ms, size_t index);

size_t cn_multistr_serialize(MultiString *ms, char *buffer);

int cn_multistr_deserialize(MultiString *ms, const char *buffer);

size_t cn_multistr_reqd_buffsize(MultiString *ms);

void cn_multistr_tostring(MultiString *ms);

#endif
