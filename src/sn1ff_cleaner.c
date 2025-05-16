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
#include "cn_string.h"
#include "cn_time.h"
#include "sn_cfg.h"
#include "sn_dir.h"
#include "sn_file.h"
#include "sn_fname.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

/*----------------------------------------------------------------.
 |                                                                |
 | Program usage                                                  |
 |                                                                |
 '----------------------------------------------------------------*/

void print_usage(int level, char *program_name) {
  cn_log_msg(
      level, __func__,
      "Usage:\n"
      "  Display this info ...\n"
      "    %s -h\n"
      "\n"
      "%s is run by a systemd service, and is not meant for interactive use\n"
      "\n"
      "  See man pages:\n"
      "    man (8) sn1ff_cleaner\n"
      "    man (7) sn1ff\n"
      "    man (8) sn1ff_service\n"
      "    man (1) sn1ff_client\n"
      "    man (1) sn1ff_monitor\n"
      "  \n\n",
      program_name, program_name);
}

/*----------------------------------------------------------------.
 |                                                                |
 | Client messages                                                |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Process LIST message from client, by supplying the names of
 * available sn1ff files
 *
 * @param [i] client_sock    socket to communicate to the client with
 * @return                   none
 */
void clean_files(const char *sn1ff_files_dir) {
  MultiString ms;

  while (true) {
    cn_multistr_init(&ms);

    // Get list of sn1ff files

    int status = sn_dir_list_files(sn1ff_files_dir, &ms);

    if (status != 0) {
      cn_log_msg(LOG_ERR, __func__,
                 "Error listing snff_files directory -> %s <-",
                 sn1ff_files_dir);
      EXIT_FAILURE;
    }

    if (ms.num_strings > 0) {
      for (size_t i = 0; i < ms.num_strings; ++i) {
        cn_log_msg(LOG_DEBUG, __func__, "Inspecting file %s",
                   cn_multistr_getstr(&ms, i));
        CName name;
        sn_cname_parse_name(cn_multistr_getstr(&ms, i), &name);

        time_t epoch_bin;
        sn_cname_get_epoch_bin(&name, &epoch_bin);

        // Delete sn1ff file if it has "expired"

        if (cn_time_epoch_expired(epoch_bin)) {
          cn_log_msg(LOG_DEBUG, __func__, "Deleting file %s",
                     cn_multistr_getstr(&ms, i));
          sn_file_delete(sn1ff_files_dir, cn_multistr_getstr(&ms, i));
          sleep(1);
        }
      }
    } else {
      cn_log_msg(LOG_DEBUG, __func__, "No sn1ff files to inspect\n");
    }

    cn_multistr_free(&ms);
    cn_log_msg(LOG_DEBUG, __func__, "Sleeping for 60 seconds");
    sleep(60);
  }
}

/*----------------------------------------------------------------.
 |                                                                |
 | Main                                                           |
 |                                                                |
 '----------------------------------------------------------------*/

// TODO: Review if this value is sufficient, and document
#define SN1FF_FILES_DIR_LENGTH 1024
#define SN1FF_FILES_DIR_LENGTH_D (SN1FF_FILES_DIR_LENGTH + 1)

int main(int argc, char *argv[]) {
  char sn1ff_files_dir[256] = {'\0'};

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
  cn_log_msg(LOG_INFO, __func__, "Starting ...");

  /*
   * Get sn1ff files directory, so we can clean the uploaded files
   */

  if (cn_string_cp(sn1ff_files_dir, sizeof(sn1ff_files_dir) - 1,
                   sn_cfg_get_server_uploads_dir()) != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Could not set sn1ff_files_dir from config -> %s <-",
               sn_cfg_get_server_uploads_dir());
  }

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

  clean_files(sn1ff_files_dir);

  // Exit

  cn_log_msg(LOG_INFO, __func__, "Exiting ...");
  cn_log_close();

  return EXIT_SUCCESS;
}
