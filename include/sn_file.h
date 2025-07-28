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

#ifndef SN_FILE_H
#define SN_FILE_H

#include "cn_dir.h"
#include "cn_file.h"
#include "cn_host.h"
#include "sn_fname.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <uuid/uuid.h>

#define SN_FILE_MAX_BODY_LENGTH 85
#define SN_FILE_MAX_BODY_LINES 256

/**
 * ATTRIBUTES
 */

typedef struct {
  char status[6];
  long ttl;
} ATTRIBUTES;

/**
 * HEADER
 */

#define SN_FILE_HEADER_CHECKID_LENGTH 128
#define SN_FILE_HEADER_CHECKID_LENGTH_D (SN_FILE_HEADER_CHECKID_LENGTH + 1)

typedef struct {
  char host[CN_HOST_HOSTNAME_LENGTH_D];
  char ipv4[CN_HOST_IPV4_LENGTH_D];
  char timestamp[CN_HOST_UTCDT_LENGTH_D];
  char checkid[SN_FILE_HEADER_CHECKID_LENGTH_D];
} HEADER;

/**
 * FILE_DATA
 */

typedef struct {
  ATTRIBUTES attributes;
  HEADER header;
  char body[SN_FILE_MAX_BODY_LINES]
           [SN_FILE_MAX_BODY_LENGTH + 1]; // 80-char lines + null terminator
  size_t body_lines;
} FILE_DATA;

int sn_file_begin(char *file_path, const char *checkid);

int sn_file_write_header(FILE *file, const HEADER *hdr);

int sn_file_read(const char *file_path, FILE_DATA *file_data);

void sn_file_delete(const char *file_dir, const char *file_name);

int sn_file_copy(const char *from_dir, const char *to_dir,
                 const char *file_name);

#endif
