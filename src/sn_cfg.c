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
#include "cn_log.h"
#include "cn_string.h"

#define CONFIG_FILE_SZ 128
char CONFIG_FILE[CONFIG_FILE_SZ] = "/etc/sn1ff/sn1ff.conf";

/*
 * Current default configuration file values
 *
 * min_log_level=info
 * client_ttls=5,4,3,2
 * server_address=192.0.2.0
 */
#define MIN_LOG_LEVEL_STR_SZ 10
char MIN_LOG_LEVEL_STR[MIN_LOG_LEVEL_STR_SZ];
int MIN_LOG_LEVEL;

#define CLIENT_TTLS_STR_SZ 20
char CLIENT_TTLS_STR[CLIENT_TTLS_STR_SZ];

#define SERVER_ADDRESS_STR_SZ 30
char SERVER_ADDRESS_STR[SERVER_ADDRESS_STR_SZ];

#define SERVER_UPLOADS_DIR_SZ 128
char SERVER_UPLOADS_DIR[SERVER_UPLOADS_DIR_SZ] = "/home/chroot/sn1ff/uploads/";

#define SERVER_UPLOADS_BASE_DIR_SZ 32
char SERVER_UPLOADS_BASE_DIR[SERVER_UPLOADS_BASE_DIR_SZ] = "/uploads";

#define SERVER_USER_SZ 32
char SERVER_USER[SERVER_USER_SZ] = "sn1ff";

#define SERVER_GROUP_SZ 32
char SERVER_GROUP[SERVER_GROUP_SZ] = "sn1ff";

#define SERVER_UNIX_DOMAIN_SOCKET_PATH "/tmp/sn1ff_socket"

int sn_cfg_str2loglevel(const char *level_str) {
  if (strcasecmp(level_str, "emerg") == 0)
    return LOG_EMERG;
  if (strcasecmp(level_str, "alert") == 0)
    return LOG_ALERT;
  if (strcasecmp(level_str, "crit") == 0)
    return LOG_CRIT;
  if (strcasecmp(level_str, "err") == 0 || strcasecmp(level_str, "error") == 0)
    return LOG_ERR;
  if (strcasecmp(level_str, "warning") == 0)
    return LOG_WARNING;
  if (strcasecmp(level_str, "notice") == 0)
    return LOG_NOTICE;
  if (strcasecmp(level_str, "info") == 0)
    return LOG_INFO;
  if (strcasecmp(level_str, "debug") == 0)
    return LOG_DEBUG;

  return LOG_INFO; // default fallback
}

/**
 *
 * @param config_path  Is the full file path of the config file
 * @return  0 success
 *         -1 could not open config file
 */
int sn_cfg_load(void) {
  FILE *file = fopen(CONFIG_FILE, "r");
  if (!file) {
    cn_log_msg(
        LOG_WARNING, __func__,
        "Fopen error opening config file -> %s <-, strerror(errno) -> %m <-",
        CONFIG_FILE);
    return -1;
  }

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    // Skip comments and empty lines

    if (line[0] == '#' || line[0] == '\n')
      continue;

    char *key = strtok(line, "=");
    char *value = strtok(NULL, "\n");

    if (key && value && strcmp(key, "min_log_level") == 0) {
      MIN_LOG_LEVEL = sn_cfg_str2loglevel(value);
    } else if (key && value && strcmp(key, "client_ttls") == 0) {
      int result = cn_string_cp(CLIENT_TTLS_STR, CLIENT_TTLS_STR_SZ, value);
      if (result != 0) {
        cn_log_msg(
            LOG_WARNING, __func__,
            "Error getting value for 'client_ttls', config file line -> %s <-",
            line);
        return -1;
      }
    } else if (key && value && strcmp(key, "server_address") == 0) {
      int result =
          cn_string_cp(SERVER_ADDRESS_STR, SERVER_ADDRESS_STR_SZ, value);
      if (result != 0) {
        cn_log_msg(LOG_WARNING, __func__,
                   "Error getting value for 'server_address', config file line "
                   "-> %s <-",
                   line);
        return -1;
      }
    } else {
      fprintf(stderr, "%s: config entry -> %s <- not recognized", __func__,
              line);
    }
  }

  fclose(file);
  return 0;
}

char *sn_cfg_get_conf_file(void) { return CONFIG_FILE; }

int sn_cfg_get_minloglevel(void) { return MIN_LOG_LEVEL; }

char *sn_cfg_get_client_ttls(void) { return CLIENT_TTLS_STR; }

char *sn_cfg_get_server_address(void) { return SERVER_ADDRESS_STR; }

/*
 * Server uploads dir
 */

char *sn_cfg_get_server_uploads_dir(void) { return SERVER_UPLOADS_DIR; }

char *sn_cfg_get_server_uploads_base_dir(void) {
  return SERVER_UPLOADS_BASE_DIR;
}

/*
 * Server user
 */

char *sn_cfg_get_server_user(void) { return SERVER_USER; }

char *sn_cfg_get_server_group(void) { return SERVER_GROUP; }

/*
 * Server unix domain socket
 */

char *sn_cfg_get_server_unix_socket(void) {
  return SERVER_UNIX_DOMAIN_SOCKET_PATH;
}
