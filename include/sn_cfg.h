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

#ifndef SN_CFG_H
#define SN_CFG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <syslog.h>

int sn_cfg_str2loglevel(const char *level_str);

int sn_cfg_load(void);

char *sn_cfg_get_conf_file(void);

int sn_cfg_get_minloglevel(void);
char *sn_cfg_get_client_ttls(void);
char *sn_cfg_get_server_address(void);
bool sn_cfg_watch_enabled(void);
bool sn_cfg_export_enabled(void);

const char *sn_cfg_get_server_upload_dir(void);
const char *sn_cfg_get_server_upload_base_dir(void);
const char *sn_cfg_get_server_watch_dir(void);
const char *sn_cfg_get_server_export_dir(void);

char *sn_cfg_get_server_user(void);
char *sn_cfg_get_server_group(void);

char *sn_cfg_get_server_unix_socket(void);

#endif
