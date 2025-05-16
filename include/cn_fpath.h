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

#ifndef CN_FPATH_H
#define CN_FPATH_H

#include <libgen.h> // For basename
#include <stdlib.h>
#include <string.h> // For strcpy

// Define error codes
#define FPATH_FILENAME_LENGTH 128
#define FPATH_FILENAME_LENGTH_D (FPATH_FILENAME_LENGTH + 1)
#define CN_FPATH_GET_FNAME_SUCCESS 0
#define CN_FPATH_GET_FNAME_INVALID_PATH 1
#define CN_FPATH_GET_FNAME_MEM_ALLOC_FAIL 2

int cn_fpath_get_name(const char *fpath, char *name);

#endif
