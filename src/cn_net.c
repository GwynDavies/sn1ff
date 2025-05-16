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

#include "cn_net.h"
#include "cn_log.h"

/*----------------------------------------------------------------.
 |                                                                |
 | Server socket                                                  |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Create a UNIX DOMAIN serrver socket
 *
 * @return  0 success
 *         -1 error creating socket
 */
int cn_net_server_socket(int *server_socket) {
  *server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (*server_socket < 0) {
    cn_log_msg(
        LOG_ERR, __func__,
        "'socket' gave error creating server socket, strerror(errno) -> %m <-");

    return -1;
  }

  return 0;
}

/**
 * Bind UNIX DOMAIN server socketi, to a path
 *
 * @return  0 success
 *         -1 error binding socket
 */
int cn_net_bind_socket(int server_socket, const char *path) {
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

  if (bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
    cn_log_msg(
        LOG_ERR, __func__,
        "'bind' gave error binding to server socket, strerror(errno) -> %m <-");
    return -1;
  }

  return 0;
}
