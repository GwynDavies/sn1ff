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

#include "cn_file.h"
#include "cn_log.h"
#include "cn_remotefe.h"
#include "sn_cfg.h"
#include "sn_file.h"
#include "sn_fpath.h"
#include "sn_status.h"
#include <stdbool.h>

char LOG_MSG[1024] = {'\0'};

void print_usage(int level, char *program_name) {
  cn_log_msg(
      level, __func__,
      "Usage:\n"
      "  Display this info ...\n"
      "    %s -h\n"
      "\n"
      "\n"
      "  Begin sn1ff file ...\n"
      "    %s -b\n"
      "\n"
      "\n"
      "  End sn1ff file, copy it to local sn1ff server directory\n"
      "    %s -e -f <sn1ff file path/name> -s <status [ALRT}WARN|OKAY|NONE]> "
      "-t <TTL in minutes>\n\n",
      ""
      "  End sn1ff file, SCP it to remote sn1ff server host\n"
      "    %s -e -f <sn1ff file path/name> -s <status [ALRT}WARN|OKAY|NONE]> "
      "-t <TTL in minutes> -a <remote sn1ff server host>\n",
      "\n"
      "\n"
      "See man pages:\n"
      "    man (1) sn1ff_client\n"
      "    man (8) sn1ff\n"
      "    man (7) sn1ff_service\n"
      "    man (1) sn1ff_monitor\n"
      "  \n\n",
      program_name, program_name, program_name, program_name);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Cleanup                                                        |
 |                                                                |
 '----------------------------------------------------------------*/

void cleanup(void) {
  cn_log_msg(LOG_DEBUG, __func__, "Exiting ...");
  cn_log_close();
}

/*----------------------------------------------------------------.
 |                                                                |
 | Main                                                           |
 |                                                                |
 '----------------------------------------------------------------*/

int main(int argc, char *argv[]) {
  /*
   * Cleanup handler
   */

  atexit(cleanup);

  /*
   * Load config file
   */

  int result = sn_cfg_load();
  if (result != 0) {
    fprintf(stderr, "Could not open/access conf file -> %s <-\n",
            sn_cfg_get_conf_file());
    return EXIT_FAILURE;
  }

  /*
   * Setup logging
   */

  int min_log_level = sn_cfg_get_minloglevel();
  cn_log_open(argv[0], min_log_level);
  cn_log_msg(LOG_DEBUG, __func__, "Starting ...");

  /*
   * Process options
   */

  bool is_help = false;       // -h Help requested
  bool is_begin_file = false; // -b Begin file
  bool is_end_file = false;   // -e End file, SCP to remote sn1ff server

  char *arg_f = NULL; // File path of sn1ff file
  char *arg_s = NULL; // Status of sn1ff file

  char *arg_t = NULL; // Time-to-live for the file in seconds
  // (e.g. 300 seconds = 5 minutes)

  char *arg_a = NULL; // Address (hostname or ip) of network
  // sn1ff server
  //
  // Leave empty if client is on same host
  // as the sn1ff server (local check)

  // Loop through command-line arguments using getopt

  int opt;
  while ((opt = getopt(argc, argv, "hbef:s:t:a:")) != -1) {
    switch (opt) {
      // Begin file
    case 'b':
      is_begin_file = true;
      break;

      // End file
    case 'e':
      is_end_file = true;
      break;

    case 'f': // File path of SN1FF file
      arg_f = optarg;
      break;
    case 's': // Status
      arg_s = optarg;
      break;
    case 't': // TTL in minutes
      arg_t = optarg;
      break;

    case 'a':         // address to move to, localhost is this
      arg_a = optarg; //     computer
      break;

    case 'h': // help
      is_help = true;
      break;

    default:
      cn_log_msg(LOG_ERR, __func__, "Option not recognized-> %c <-", opt);
      return EXIT_FAILURE;
    }
  }

  // Help

  if (is_help) {
    print_usage(LOG_INFO, argv[0]);
  }

  // Begin file

  else if (is_begin_file) {
    char file_path[FNAME_PATH_LENGTH_D] = {'\0'};
    int result = sn_file_begin(file_path);
    if (result != 0) {
      fprintf(stderr, "Could not begin sn1ff file -> %s <-\n", file_path);
      return EXIT_FAILURE;
    }

    // Print output filename, for shell script (or whatever is running the
    // check) to collect it
    fprintf(stdout, "%s\n", file_path);

    return EXIT_SUCCESS;
  }

  // End file - local copy - address or "host" is not set in arg_a

  else if (is_end_file && arg_f != NULL && arg_s != NULL && arg_t != NULL &&
           arg_a == NULL) {
    // Check "current status"  arg_s

    if (sn_status_isvalid(arg_s) != 0) {
      cn_log_msg(LOG_ERR, __func__,
                 "Invalid status. Must be [ALRT|WARN|OKAY|NONE]");
      return EXIT_FAILURE;
    }

    // Clean non printable chars from file

    if (cn_file_clean(arg_f) != 0) {
      cn_log_msg(LOG_ERR, __func__,
                 "Error cleaning non-printable chars from file ->%s<-", arg_f);
      return EXIT_FAILURE;
    }

    // Build destination file path

    char *endptr = NULL;
    int arg_t_i =
        (int)strtol(arg_t, &endptr, 10); // 10 is for base 10 (decimal)
    if (*endptr != '\0') {
      cn_log_msg(LOG_ERR, __func__,
                 "Conversion error, invalid character -> %s <-", endptr);

      return EXIT_FAILURE;
    }

    char new_dir_path[FNAME_PATH_LENGTH_D] = {'\0'};
    memset(new_dir_path, '\0', (FNAME_PATH_LENGTH_D) * sizeof(char));

    sn_fpath_genfull(arg_f, arg_s, sn_cfg_get_server_uploads_dir(), arg_t_i,
                     new_dir_path);

    // Copy the file

    if (cn_file_copy(arg_f, new_dir_path) != 0) {
      cn_log_msg(LOG_ERR, __func__, "Copy of file failed, from -> %s  to -> %s",
                 arg_f, new_dir_path);
      return EXIT_FAILURE;
    }

    // Change the file group

    if (cn_file_chgrp(new_dir_path, sn_cfg_get_server_group()) != 0) {
      cn_log_msg(
          LOG_ERR, __func__,
          "main: Setting group failed for file -> %s <- to group -> %s <-",
          new_dir_path, sn_cfg_get_server_group);
      return EXIT_FAILURE;
    }

    // Change the file mode to 660

    if (cn_file_mode660(new_dir_path) != 0) {
      cn_log_msg(LOG_ERR, __func__, "Setting mode 660 failed for file -> %s <-",
                 new_dir_path);
      return EXIT_FAILURE;
    }

    // Delete the original file

    if (cn_file_delete(arg_f) != 0) {
      cn_log_msg(LOG_ERR, __func__, "Delete of file failed, from -> %s <-",
                 arg_f);
      return EXIT_FAILURE;
    }
  }

  // End file - Remote SCP - address or "host" has value in arg_a

  else if (is_end_file && arg_f != NULL && arg_s != NULL && arg_t != NULL &&
           arg_a != NULL) {
    // Check "current status"  arg_s

    if (sn_status_isvalid(arg_s) != 0) {
      cn_log_msg(LOG_ERR, __func__,
                 "Invalid status. Must be [ALRT|WARN|OKAY|NONE]");
      return EXIT_FAILURE;
    }

    // Clean non printable chars from file

    if (cn_file_clean(arg_f) != 0) {
      cn_log_msg(LOG_ERR, __func__,
                 "Error cleaning non-printable chars from file ->%s<-", arg_f);
      return EXIT_FAILURE;
    }

    // Build destination file path

    char new_dir_path[FNAME_PATH_LENGTH_D] = {'\0'};

    char *endptr = NULL;
    int arg_t_i =
        (int)strtol(arg_t, &endptr, 10); // 10 is for base 10 (decimal)
    if (*endptr != '\0') {
      cn_log_msg(LOG_ERR, __func__, "Conversion error, invalid character: %s",
                 endptr);
      return EXIT_FAILURE;
    }

    sn_fpath_genfull(arg_f, arg_s, sn_cfg_get_server_uploads_base_dir(),
                     arg_t_i, new_dir_path);

    // Build SCP path

    const int scp_path_len =
        FNAME_PATH_LENGTH + 128; // 128 for user@ip or hostname
    char scp_path[scp_path_len + 1];
    memset(scp_path, '\0', (scp_path_len + 1) * sizeof(char));

    char scp_path_format[] = "%s@%s:%s";

    snprintf(scp_path, scp_path_len, scp_path_format, sn_cfg_get_server_user(),
             arg_a, new_dir_path);

    // Timeout

    int timeout_seconds = 60; // Set timeout to 60 seconds

    // SCP - Call the function to execute the scp command
    int result = cn_remotefe_scp(arg_f, scp_path, timeout_seconds);
    if (result != 0) {
      cn_log_msg(LOG_ERR, __func__, "Remote copy with scp failed");
      return EXIT_FAILURE;
    }
  }

  else {
    cn_log_msg(LOG_ERR, __func__, "Bad command usage ...");
    print_usage(LOG_ERR, argv[0]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
