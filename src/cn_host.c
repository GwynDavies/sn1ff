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

#include "cn_host.h"
#include "cn_log.h"
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int cn_host_ipv4(const char *interface_name, char *ip_addr) {
  struct ifaddrs *interfaces, *ifa;

  // Get the list of network interfaces

  if (getifaddrs(&interfaces) != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'getifaddrs' gave error getting network interfaces, "
               "strerror(errno) -> %m <-");
    return -1;
  }

  // Iterate through the interfaces

  for (ifa = interfaces; ifa != NULL; ifa = ifa->ifa_next) {
    // Check if the interface matches "eth0" and has an IPv4 address

    if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET &&
        strcmp(ifa->ifa_name, interface_name) == 0) {
      // Convert the IP address to a readable string

      void *tmp_addr_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

      inet_ntop(AF_INET, tmp_addr_ptr, ip_addr, CN_HOST_IPV4_LENGTH_D);

      // Free the memory allocated for the interface list

      freeifaddrs(interfaces);

      return 0;
    }
  }

  // No IPv4 address found for the interface

  cn_log_msg(LOG_ERR, __func__,
             "no ip address found for interface name -> %s <-", interface_name);

  freeifaddrs(interfaces);
  return -2;
}

/**
 * @return 0 success
 */
int cn_host_hostname(char *hostname) {
  int result = gethostname(hostname, CN_HOST_HOSTNAME_LENGTH);
  if (result != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'gethostname' returned error, strerror(errno) -> %m <-");
  }
  return result;
}

int cn_host_utcdt(char *utc_datetime) {
  // Get the current epoch time

  time_t t = time(NULL);

  // Check if time retrieval was successful

  if (t == (time_t)(-1)) {
    // Warning - man page does not state if errno is set or not

    cn_log_msg(LOG_ERR, __func__,
               "'time_t' returned error (-1), strerror(errno) -> %m <-");
    return -1;
  }

  // Convert to UTC time

  struct tm *tm_info = gmtime(&t);
  if (tm_info == NULL) {
    cn_log_msg(
        LOG_ERR, __func__,
        "'gmtime' gave error converting time to UTC, strerror(errno) -> %m <-");
    return -2;
  }

  // Format the time into the buffer

  strftime(utc_datetime, CN_HOST_UTCDT_LENGTH_D, "%a %B %d, %Y %H:%M:%S",
           tm_info);

  return 0;
}
