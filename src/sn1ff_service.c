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

#include "cn_log.h"
#include "cn_multistr.h"
#include "cn_net.h"
#include "cn_string.h"
#include "sn_cfg.h"
#include "sn_dir.h"
#include "sn_file.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <linux/prctl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

/*----------------------------------------------------------------.
 |                                                                |
 | Arguments                                                      |
 |                                                                |
 '----------------------------------------------------------------*/

void print_usage(int level, char *program_name) {
  cn_log_msg(level, __func__,
             "Usage:\n"
             "  Display this info ...\n"
             "    %s -h\n"
             "\n"
             "%s is a systemd service, and is not meant for interactive use\n"
             "\n"
             "  See man pages:\n"
             "    man (8) sn1ff_service\n"
             "    man (7) sn1ff\n"
             "    man (8) sn1ff_cleaner\n"
             "    man (1) sn1ff_client\n"
             "    man (1) sn1ff_monitor\n"
             "  \n\n",
             program_name, program_name);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Client network communication                                   |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Send response to client message(msg)
 */
void send_response(int client_sock, const char *response, size_t size) {
  uint32_t response_length = htonl(size);
  size_t total_sent = 0;
  ssize_t sent_bytes;

  while (total_sent < sizeof(response_length)) {
    sent_bytes = send(client_sock, ((char *)&response_length) + total_sent,
                      sizeof(response_length) - total_sent, 0);
    if (sent_bytes <= 0) {
      cn_log_msg(LOG_DEBUG, __func__, "Client disconnected - exiting");
      exit(0);
    }
    total_sent += sent_bytes;
    cn_log_msg(LOG_DEBUG, __func__, "Total bytes sent= %zu", total_sent);
  }

  total_sent = 0;
  size_t response_len = size;

  while (total_sent < response_len) {
    sent_bytes =
        send(client_sock, response + total_sent, response_len - total_sent, 0);
    if (sent_bytes <= 0) {
      cn_log_msg(LOG_DEBUG, __func__, "Client disconnected - exiting");
      exit(0);
    }
    total_sent += sent_bytes;
    cn_log_msg(LOG_DEBUG, __func__, "Total bytes sent= %zu", total_sent);
  }
}

/*----------------------------------------------------------------.
 |                                                                |
 | Handle client request messages                                 |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Get message from the client
 *
 * @param  client_sock
 * @param  msg_buffer
 * @return  0 success
 *         -1 client disconnected
 *         -1 malloc failed
 */
int get_message(int client_sock, char *msg_buffer) {
  uint32_t message_length;
  ssize_t received_bytes;

  // Receive the message length

  received_bytes =
      recv(client_sock, &message_length, sizeof(message_length), 0);

  if (received_bytes <= 0) {
    cn_log_msg(
        LOG_DEBUG, __func__,
        "Received bytes <= 0 -> %zd <- implies client disconnected - returning",
        received_bytes);
    return -1;
  } else {
    cn_log_msg(
        LOG_DEBUG, __func__,
        "Received bytes <= 0 -> %zd <- implies client disconnected - returning",
        received_bytes);
    cn_log_msg(LOG_DEBUG, __func__, "Received message length -> %u <-",
               message_length);
  }

  // Receive the message

  message_length = ntohl(message_length);

  size_t received = 0;
  while (received < message_length) {
    received_bytes =
        recv(client_sock, msg_buffer + received, message_length - received, 0);
    if (received_bytes <= 0) {
      cn_log_msg(LOG_DEBUG, __func__, "Client disconnected - returning -1");
      return -1;
    }
    received += received_bytes;
  }

  msg_buffer[message_length] = '\0';
  cn_log_msg(LOG_DEBUG, __func__, "Received message -> %s <-", msg_buffer);

  return 0;
}

/**
 * Handle message (msg) LIST from client - by supplying the names of
 * available sn1ff files
 *
 * @param client_sock    socket to communicate to the client with
 * @return  0 success
 *         -1 Could not list check results files uploads dir
 */
int handle_msg_list(int client_sock, const char *sn1ff_files_dir) {
  MultiString ms;
  cn_multistr_init(&ms);

  // Get list of sn1ff files

  int status = sn_dir_list_files(sn1ff_files_dir, &ms);

  if (status != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Could not list check results files, uploads dir -> %s <-",
               __func__, sn1ff_files_dir);
    return -1;
  }

  // Display found sn1ff files

  if (ms.num_strings > 0) {
    ;
    size_t buffer_size_src = cn_multistr_reqd_buffsize(&ms);
    char *buffer_src = malloc(buffer_size_src);
    size_t serialized_size = cn_multistr_serialize(&ms, buffer_src);
    cn_multistr_serialize(&ms, buffer_src);

    cn_log_msg(LOG_DEBUG, __func__, "Size of buffer -> %zu <-", __func__,
               sizeof(buffer_src));
    cn_log_msg(LOG_DEBUG, __func__, "Serialized size -> %zu <-", __func__,
               serialized_size);

    send_response(client_sock, buffer_src, serialized_size);
    free(buffer_src);
  } else {
    cn_log_msg(LOG_DEBUG, __func__, "No .snff files found");

    cn_multistr_append(&ms, "NO_FILES");
    size_t buffer_size_src = cn_multistr_reqd_buffsize(&ms);
    char *buffer_src = malloc(buffer_size_src);
    size_t serialized_size = cn_multistr_serialize(&ms, buffer_src);
    cn_multistr_serialize(&ms, buffer_src);

    send_response(client_sock, buffer_src, serialized_size);
    free(buffer_src);
  }

  return 0;
}

/**
 * Receive the different messages and handle them
 *
 * @param   client_sock
 * @param   sn1ff_Files_dir
 * @return  0 Non QUIT message (msg) received and  processed
 *          1 QUIT message (msg) received
 *         -1 Client disconnected
 *         -2 Could not list check results files, uploads dir
 */
int handle_msg(int client_sock, const char *sn1ff_files_dir) {
  char msg_buffer[256] = {'\0'};
  int result = get_message(client_sock, msg_buffer);

  if (result == 0) {
    cn_log_msg(LOG_DEBUG, "msg_buffer -> %s <-", msg_buffer);

    // Message LIST

    if (strcmp(msg_buffer, "LIST") == 0) {
      if (handle_msg_list(client_sock, sn1ff_files_dir) != 0) {
        cn_log_msg(LOG_ERR,
                   "Could not list check results files, uploads dir -> %s <-",
                   sn1ff_files_dir);
        return -1;
      }
    }

    // Message QUIT

    else if (strcmp(msg_buffer, "QUIT") == 0) {
      return 1;
    }

    // Message DELETE

    else if (cn_string_starts_with(msg_buffer, "DELETE")) {
      // TODO: Add error handling here
      char *tokens[3];
      cn_string_split(msg_buffer, tokens);
      sn_file_delete(sn1ff_files_dir, tokens[1]);
    }

    return 0;
  } else
    return result;
}

/*----------------------------------------------------------------.
 |                                                                |
 | Cleaner process                                                |
 |                                                                |
 '----------------------------------------------------------------*/

int do_cleaner_process(void) {
  // Set the child process to be terminated when the parent exits

  if (prctl(PR_SET_PDEATHSIG, SIGKILL) == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'prctl' failed, strerror(errno) -> %m <- Exiting ...");
    return EXIT_FAILURE;
  }

  // Exec sn1ff_cleaner

  execlp("/usr/bin/sn1ff_cleaner", "sn1ff_cleaner", (char *)NULL);
  cn_log_msg(LOG_ERR, __func__,
             "'execlp' failed for cleaner process, strerror(errno) -> %m <- "
             "Exiting ...");
  return EXIT_FAILURE;
}

/*----------------------------------------------------------------.
 |                                                                |
 | Handle client                                                  |
 |                                                                |
 '----------------------------------------------------------------*/

int handle_client(int client_sock, const char *sn1ff_files_dir) {
  // Set the child process to be terminated when the parent exits

  if (prctl(PR_SET_PDEATHSIG, SIGKILL) == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'prctl' failed, strerror(errno) -> %m <- Exiting ...");
    return EXIT_FAILURE;
  }

  while (1) {
    int status = handle_msg(client_sock, sn1ff_files_dir);

    if (status == 1 || status == -1) {
      cn_log_msg(LOG_ERR, __func__,
                 "%s: Non 0 status from handle_msg -> %d <- Exiting on client",
                 status);
      break;
    }
  }

  return EXIT_SUCCESS;
}

/*----------------------------------------------------------------.
|                                                                |
| Cleanup                                                        |
|                                                                |
'----------------------------------------------------------------*/

static int server_sock = 0;

void cleanup() {
  if (server_sock)
    close(server_sock);

  if (sn_cfg_get_server_unix_socket())
    unlink(sn_cfg_get_server_unix_socket());

  cn_log_msg(LOG_DEBUG, __func__, "Exiting ...");
  cn_log_close();
}

/*----------------------------------------------------------------.
 |                                                                |
 | Main                                                           |
 |                                                                |
 '----------------------------------------------------------------*/

int main(int argc, char *argv[]) {
  char sn1ff_files_dir[256] = {'\0'};

  /*
   * Setup cleanup handler
   */

  atexit(cleanup);

  /*
   * Load config file
   */

  int result = sn_cfg_load();
  if (result != 0) {
    fprintf(stderr, "Could not open/access conf file -> %s <-\n",
            sn_cfg_get_conf_file());
    EXIT_FAILURE;
  }

  /*
   * Setup logging
   */

  int min_log_level = sn_cfg_get_minloglevel();
  cn_log_open(argv[0], min_log_level);
  cn_log_msg(LOG_DEBUG, __func__, "Starting ...");

  /*
   * Get uploads dir to receive check results files
   */

  strncpy(sn1ff_files_dir, sn_cfg_get_server_uploads_dir(),
          sizeof(sn1ff_files_dir) - 1);

  /*
   * Process arguments
   */

  // Check we have required number of arguments

  if (argc > 2) {
    cn_log_msg(LOG_ERR, __func__, "Too many arguments provided");

    print_usage(LOG_ERR, argv[0]);
    return EXIT_FAILURE;
  }

  // Loop through command-line arguments using getopt

  bool is_help = false; // Help requested

  int opt;
  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
    case 'h': // help
      is_help = true;
      break;

    default:
      cn_log_msg(LOG_ERR, __func__, "Option not recognized -> %c <- exiting",
                 opt);
      return EXIT_FAILURE;
    }

    // Process options

    if (is_help) {
      print_usage(LOG_INFO, argv[0]);
      return EXIT_SUCCESS;
    }

    else {
      print_usage(LOG_ERR, argv[0]);
      return EXIT_FAILURE;
    }
  }

  /*
   * Fork-and-exec cleaner process
   */

  pid_t pid = fork();
  if (pid == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'fork' failed for cleaner process, strerror(errno) -> %m <- "
               "Exiting ...");
    return EXIT_FAILURE;
  }

  if (pid == 0) {
    do_cleaner_process();
  }

  // Remove any existing socket file

  unlink(sn_cfg_get_server_unix_socket());

  // Create server socket, bind and listen

  if (cn_net_server_socket(&server_sock) != 0) {
    cn_log_msg(LOG_ERR, __func__, "Error creating UNIX DOMAIN server socket");
    return EXIT_FAILURE;
  }

  if (cn_net_bind_socket(server_sock, sn_cfg_get_server_unix_socket()) != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Error binding UNIX DOMAIN server socket to path -> %s <-",
               sn_cfg_get_server_unix_socket());
    return EXIT_FAILURE;
  }

  // Start listening on server socket for connections

  listen(server_sock, 5);
  cn_log_msg(LOG_DEBUG, __func__, "Listening on socket path -> %s <-",
             sn_cfg_get_server_unix_socket());

  /*
   * Client loop
   */

  while (true) {
    // Accept client connection

    int client_sock = accept(server_sock, NULL, NULL);
    if (client_sock < 0) {
      cn_log_msg(
          LOG_ERR, __func__,
          "'accept' on socket failed, strerror(errno) -> %m <- Exiting ...");
      return EXIT_FAILURE;
    }

    // Fork

    pid = fork();
    if (pid == 0) {
      // Child process starts here
      handle_client(client_sock, sn1ff_files_dir);
      cn_log_msg(LOG_DEBUG, __func__,
                 "Child process finished handling client - exiting");
      exit(0);
    } else if (pid > 0) {
      // Parent process:continues here - close the client socket
      close(client_sock);
    } else {
      cn_log_msg(LOG_ERR, __func__, "'fork' failed, strerror(errno) -> %m <-");
    }
  }

  return EXIT_SUCCESS;
}
