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

#include <criterion/criterion.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "cn_net.h"

Test(cn_net, server_socket_success) {
  int sockfd = -1;
  int result = cn_net_server_socket(&sockfd);

  cr_assert_eq(result, 0, "Result for creating socket should be 0");
  cr_assert_geq(sockfd, 0, "Socket file descriptor should be >= 0");

  close(sockfd);
}

Test(cn_net, bind_socket_success) {
  const char *path = "/tmp/test_socket_success.sock";

  int sockfd = -1;
  int result = cn_net_server_socket(&sockfd);
  cr_assert_eq(result, 0, "Result for creating socket should be 0");

  unlink(path); // Clean up before test
  cn_net_bind_socket(sockfd, path);

  cr_assert(access(path, F_OK) == 0, "Socket file should exist after bind");

  close(sockfd);
  unlink(path);
}

// TODO : This test just fails - as the function just exits if
//        the socket is already bound. Suggest changing the
//        functionality to not exit, but return error status -
//        then let the using program exit itself
//
// Test(cn_net, bind_socket_failure, .exit_code = 1, .signal = 0) {
//  const char *path = "/tmp/test_socket_fail.sock";
//
//  int sockfd1 = cn_net_server_socket_or_exit();
//  unlink(path);
//  cn_net_bind_socket_or_exit(sockfd1, path);
//
//  int sockfd2 = cn_net_server_socket_or_exit();
//  // This second bind should fail
//  cn_net_bind_socket_or_exit(sockfd2, path);
//
//  // Not reached
//  close(sockfd1);
//  close(sockfd2);
//  unlink(path);
//}
