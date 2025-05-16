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

#include "cn_host.h"
#include <criterion/criterion.h>
#include <criterion/logging.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * cn_host_ipv4
 */

Test(cn_host_ipv4, valid_interface) {
  char ip[CN_HOST_IPV4_LENGTH_D] = {0};

  // Adjust interface name as needed for your system ("lo", "eth0", etc.)
  int result = cn_host_ipv4("lo", ip);

  cr_assert_eq(result, 0, "Expected cn_host_ipv4 to return 0 for 'lo'");
  cr_assert(strlen(ip) > 0, "Expected a non-empty IP address");
  cr_log_info("IP address of 'lo': %s\n", ip);
}

Test(cn_host_ipv4, invalid_interface) {
  char ip[CN_HOST_IPV4_LENGTH_D] = {0};
  int result = cn_host_ipv4("nonexistent", ip);

  cr_assert_eq(result, -2,
               "Expected cn_host_ipv4 to return -2 for nonexistent interface");
}

/*
 * cn_host_hostname
 */

Test(cn_host_hostname, retrieves_hostname) {
  char hostname[CN_HOST_HOSTNAME_LENGTH] = {0};
  int result = cn_host_hostname(hostname);

  cr_assert_eq(result, 0, "Expected cn_host_hostname to return 0");
  cr_assert(strlen(hostname) > 0, "Expected a non-empty hostname");
  cr_log_info("Hostname: %s\n", hostname);
}

/*
 * cn_host_utcdt
 */

Test(cn_host_utcdt, formats_time) {
  char time_str[CN_HOST_UTCDT_LENGTH_D] = {0};
  int result = cn_host_utcdt(time_str);

  cr_assert_eq(result, 0, "Expected cn_host_utcdt to return 0");
  cr_assert(strlen(time_str) > 0, "Expected a non-empty UTC date/time string");
  cr_log_info("UTC date/time: %s\n", time_str);
}
