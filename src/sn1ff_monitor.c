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
#include "cn_time.h"
#include "sn_cfg.h"
#include "sn_file.h"
#include "sn_ui.h"
#include <arpa/inet.h>
#include <errno.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MSG_RESPONSE_BUFFER_SIZE 1024
#define USER_DISPLAY_PAUSE_SECS 1

char LOG_MSG[1024] = {'\0'};

void print_usage(int level, char *program_name) {
  cn_log_msg(level, __func__,
             "Usage:\n"
             "  Display this info ...\n"
             "    %s -h\n"
             "\n"
             "\n"
             "  See man pages:\n"
             "    man (1) sn1ff_monitor\n"
             "    man (8) sn1ff\n"
             "    man (7) sn1ff_service\n"
             "    man (7) sn1ff_cleaner\n"
             "    man (1) sn1ff_client\n"
             "  \n\n",
             program_name);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Multi strings                                                  |
 |                                                                |
 '----------------------------------------------------------------*/

void deserialize_multistr(char *buffer, MultiString *ms) {
  cn_multistr_free(ms);

  if (cn_multistr_deserialize(ms, buffer) == -1) {
    cn_log_msg(LOG_ERR, __func__, "Failed to de-serialize MultiString\n");
  }
}

/*----------------------------------------------------------------.
 |                                                                |
 | Socket messages                                                |
 |                                                                |
 '----------------------------------------------------------------*/

void send_message(int sock, const char *message) {
  uint32_t length = htonl(strlen(message));
  size_t total_sent = 0;
  ssize_t sent_bytes;

  while (total_sent < sizeof(length)) {
    sent_bytes = send(sock, ((char *)&length) + total_sent,
                      sizeof(length) - total_sent, 0);
    if (sent_bytes <= 0) {
      cn_log_msg(LOG_ERR, __func__,
                 "'send' gave <= 0 bytes - server may have disconnected - "
                 "exiting, strerror(errno) -> %m <-");
      return;
    }
    total_sent += sent_bytes;
  }

  size_t message_len = strlen(message);
  total_sent = 0;

  while (total_sent < message_len) {
    sent_bytes = send(sock, message + total_sent, message_len - total_sent, 0);
    if (sent_bytes <= 0) {
      cn_log_msg(LOG_ERR, __func__,
                 "'send' gave <= 0 bytes - server may have disconnected - "
                 "exiting, strerror(errno) -> %m <-");
      return;
    }
    total_sent += sent_bytes;
  }
}

/**
 * Receive response on the socket, for a message sent to the server
 *
 * Param(s):
 *   sock
 *   ms
 */
void receive_message_response(int sock, MultiString *ms) {
  uint32_t length;
  ssize_t received_bytes;

  // Receive length of the response

  received_bytes = recv(sock, &length, sizeof(length), 0);
  if (received_bytes <= 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'recv' gave <= 0 bytes - server may have disconnected - "
               "exiting, strerror(errno) -> %m <-");
    return;
  }

  length = ntohl(length);

  char *received_buffer = malloc(length + 1);

  if (!received_buffer) {
    cn_log_msg(LOG_ERR, __func__,
               "'malloc' gave NULL - just ignoring, strerror(errno) -> %m <-");
    return;
  }

  // Clear buffer

  memset(received_buffer, '\0', (length + 1) * sizeof(char));

  // Receive the response

  size_t received = 0;
  while (received < length) {
    received_bytes =
        recv(sock, received_buffer + received, length - received, 0);

    if (received_bytes <= 0) {
      cn_log_msg(LOG_ERR, __func__,
                 "'recv' gave <= 0 bytes - server may have disconnected - "
                 "exiting, strerror(errno) -> %m <-");

      free(received_buffer);
      return;
    }

    received += received_bytes;
  }

  // Print the response received

  if (received_buffer) {
    deserialize_multistr(received_buffer, ms);
  }

  free(received_buffer);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Cleanup                                                        |
 |                                                                |
 '----------------------------------------------------------------*/

static int SOCKET;

void cleanup(void) {
  cn_log_msg(LOG_DEBUG, __func__, "Exiting ...");
  cn_log_close();

  sn_ui_close();
  endwin();

  close(SOCKET);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Main                                                           |
 |                                                                |
 '----------------------------------------------------------------*/

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
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
   * Get sn1ff files directory, so we can display the uploaded files
   */

  strncpy(sn1ff_files_dir, sn_cfg_get_server_uploads_dir(),
          sizeof(sn1ff_files_dir) - 1);

  /*
   * Process arguments
   */

  // Check we have required number of arguments

  if (argc >= 2) {
    cn_log_msg(LOG_ERR, __func__, "Too many arguments provided");

    print_usage(LOG_ERR, argv[0]);
    return EXIT_FAILURE;
  }

  // Loop through any command-line arguments using getopt

  bool is_help = false; // Help requested

  int opt;
  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
    case 'h': // help
      is_help = true;
      break;

    default:
      cn_log_msg(LOG_ERR, __func__, "Option not recognized -> %c <-", opt);
      print_usage(LOG_ERR, argv[0]);
      return EXIT_FAILURE;
    }

    // Process options

    if (is_help) {
      print_usage(LOG_INFO, argv[0]);
      return EXIT_SUCCESS;
    }

    else {
      cn_log_msg(LOG_ERR, __func__, "Option not recognized -> %c <-", opt);
      print_usage(LOG_ERR, argv[0]);
      return EXIT_FAILURE;
    }
  }

  /*
   * Setup networking to server
   */

  SOCKET = socket(AF_UNIX, SOCK_STREAM, 0);
  if (SOCKET < 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'socket' failed on create, strerror(errno) -> %m <-");
    return EXIT_FAILURE;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, sn_cfg_get_server_unix_socket(),
          sizeof(addr.sun_path) - 1);

  if (connect(SOCKET, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'socket' failed on connect, strerror(errno) -> %m <-");
    return EXIT_FAILURE;
  }

  /*
   * Setup UI (NCurses)
   */

  sn_ui_init();

  /*
   * Processing loop:
   *   - Send commands to the server over socket
   *       - LIST available check results files
   *
   *   - Receive responses over the socket from the server
   *   - Exit when user indicates they want to quit
   */

  while (true) {
    int user_cmd = USER_CMD_NONE;

    // Send command LIST_FILES to the server

    cn_log_msg(LOG_DEBUG, __func__, "Send message LIST to the service");
    send_message(SOCKET, "LIST");

    // Receive response

    MultiString ms;
    cn_multistr_free(&ms);
    cn_multistr_init(&ms);
    receive_message_response(SOCKET, &ms);

    // Check there are sn1ff files to display

    if (ms.num_strings == 1 &&
        (strcmp(cn_multistr_getstr(&ms, 0), "NO_FILES") == 0)) {
      sn_ui_display_no_files(&user_cmd);

      cn_multistr_free(&ms);

      if (user_cmd == USER_CMD_QUIT) {
        break;
      }

      sleep(USER_DISPLAY_PAUSE_SECS);
      continue;
    }

    // Display received file names

    for (size_t i = 0; i < ms.num_strings; ++i) {
      // Handle sn1ff file having expired

      char full_filename[256] = "";
      strcat(full_filename, sn1ff_files_dir);
      strcat(full_filename, cn_multistr_getstr(&ms, i));

      // Display sn1ff file contents

      FILE_DATA file_data;
      if (sn_file_read(full_filename, &file_data) != 0) {
        // Skip non existing file - possibly deleted by
        // cleaner process if TTL expired
        if (errno == ENOENT) {
          cn_log_msg(LOG_ERR, __func__, "Skipping file not existing %s",
                     full_filename);
          cn_multistr_free(&ms);
          continue;
        }
        // Skip file as it just could not be opened
        // for some reason
        else {
          cn_log_msg(LOG_ERR, __func__,
                     "'fopen' gave an error opening file -> %s <-, "
                     "strerror(errno) -> %m <-",
                     full_filename);
          cn_multistr_free(&ms);
          continue;
        };
      }

      sn_ui_display_file(cn_multistr_getstr(&ms, i), &file_data, &user_cmd);

      if (user_cmd == USER_CMD_QUIT) {
        cn_log_msg(LOG_DEBUG, __func__, "User requested 'QUIT'");

        cn_multistr_free(&ms);
        break;
      }

      if (user_cmd == USER_CMD_DELETE) {
        cn_log_msg(LOG_DEBUG, __func__, "Sending command 'DELETE' to server");
        char delete_msg[128];
        strcpy(delete_msg, "DELETE ");
        strcat(delete_msg, cn_multistr_getstr(&ms, i));
        send_message(SOCKET, delete_msg);
      }

      // Just give visual pause to make display easier on
      // the user

      sleep(1);
    }

    cn_multistr_free(&ms);

    if (user_cmd == USER_CMD_QUIT) {
      cn_log_msg(LOG_DEBUG, __func__, "User requested 'QUIT'");
      break;
    }
  } // while

  // Cleanup

  return EXIT_SUCCESS;
}
