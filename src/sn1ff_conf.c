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
#include "sn_cfg.h"
#include <stdio.h>
#include <stdlib.h>

/*----------------------------------------------------------------.
 |                                                                |
 | Usage                                                          |
 |                                                                |
 '----------------------------------------------------------------*/

void print_usage(const char *program_name) {
  printf("Usage: %s [OPTION]\n", program_name);
  printf("Options:\n");
  printf("  -t    Print value of 'client_ttls' from config file\n");
  printf("  -a    Print value of 'server_address' from config file\n");
  printf("  -h    Show this help message\n");
  printf("\n");
  printf("See man pages:\n");
  printf("  man (1) sn1ff_conf");
  printf("  man (1) sn1ff_client\n");
  printf("  man (1) sn1ff_license\n\n");
}

/*----------------------------------------------------------------.
 |                                                                |
 | Main                                                           |
 |                                                                |
 '----------------------------------------------------------------*/

int main(int argc, char *argv[]) {
  if (argc != 2) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  int result = sn_cfg_load();
  if (result != 0) {
    fprintf(stderr, "Could not open/access conf file -> %s <-\n",
            sn_cfg_get_conf_file());
    EXIT_FAILURE;
  }

  if (strcmp(argv[1], "-t") == 0) {
    char *value = sn_cfg_get_client_ttls();
    printf("%s\n", value);
  } else if (strcmp(argv[1], "-a") == 0) {
    char *value = sn_cfg_get_server_address();
    printf("%s\n", value);
  } else if (strcmp(argv[1], "-h") == 0) {
    print_usage(argv[0]);
    return 0;
  } else {
    fprintf(stderr, "Invalid option: %s\n", argv[1]);
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
