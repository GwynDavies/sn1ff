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

#ifndef CN_HOST_H
#define CN_HOST_H

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define CN_HOST_HOSTNAME_LENGTH 16
#define CN_HOST_HOSTNAME_LENGTH_D (CN_HOST_HOSTNAME_LENGTH + 1)
#define CN_HOST_IPV4_LENGTH 16
#define CN_HOST_IPV4_LENGTH_D (CN_HOST_IPV4_LENGTH + 1)
#define CN_HOST_UTCDT_LENGTH 30
#define CN_HOST_UTCDT_LENGTH_D (CN_HOST_UTCDT_LENGTH + 1)

int cn_host_ipv4(const char *interface_name, char *ip_addr);

int cn_host_hostname(char *hostname);

int cn_host_utcdt(char *utc_datetime);

#endif
