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

#include "sn_file.h"
#include "cn_fpath.h"
#include "cn_host.h"
#include "cn_log.h"
#include "cn_string.h"
#include "sn_const.h"
#include "sn_dir.h"

/**
 * A sn1ff file can exist in 2 places:
 *
 *   - Under the /tmp directory - when being created
 *   - The snff files directory - when already created, and on the
 *       sn1ff server
 *
 * - file_dir - the directory path, to the file
 * - filename - the name of just the sn1ff file
 * - file_path - the full file path of the file:
 *     <file_dir>/<filename>,
 */

/*----------------------------------------------------------------.
 |                                                                |
 |  Write sn1ff file                                              |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Write header for a snff file
 *
 * @param  file    is the Linux FILE for the sn1ff file
 * @param  header  is the sn1ff file header info to write from to the file
 * @return 0 success
 *         1 file param is null
 *         2 header param is not valid
 */
int sn_file_write_header(FILE *file, const HEADER *hdr) {
  if (file == NULL) {
    cn_log_msg(LOG_ERR, __func__, "Argument 'file' is NULL");
    return 1;
  }

  if (!hdr) {
    fprintf(stderr, "sn_file_write_header: 'hdr' arg not valid\n");
    return 2;
  }

  fprintf(file, "App: sn1ff\n");
  fprintf(file, "Ver: %s\n", VERSION);
  fprintf(file, "Host: %s\n", hdr->host);
  fprintf(file, "IPv4: %s\n", hdr->ipv4);
  fprintf(file, "At: %s\n", hdr->timestamp);
  fprintf(file, "CheckID: %s\n", hdr->checkid);
  fprintf(file, "\n\n");

  return 0;
}

/*----------------------------------------------------------------.
 |                                                                |
 |  Create (begin) sn1ff file                                     |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Begin sn1ff file, under the the users home directory, writing the
 * file header to it
 *
 * @param   file_path  is the created file path for the sn1ff file
 * @return  0 success
 *         -1 unable to create directory for the file
 *         -2 unable to open the file to create it
 */
int sn_file_begin(char *file_path, const char *checkid) {
  char sn1ff_client_dir[256];
  int sn1ff_client_dir_sz = 256;

  int result = sn_dir_client(sn1ff_client_dir, sn1ff_client_dir_sz);

  cn_log_msg(LOG_ERR, __func__, "sn_dir_client path -> %s <-",
             sn1ff_client_dir);

  if (result != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Could not get 'sn1ff' files dir for process user");
    return -1;
  }

  // Create file name

  FName fname;
  sn_fname_init(&fname);
  sn_fname_set_dir(&fname, sn1ff_client_dir);

  // Create default header values

  HEADER hdr = {.host = "No hostname",
                .ipv4 = "No ip for eth0",
                .timestamp = "_______ __, 20__ __:__:__",
                .checkid = "N/A"};

  // Set actual hostname in header

  char hostname[CN_HOST_HOSTNAME_LENGTH_D] = {'\0'};
  int status = cn_host_hostname(hostname);
  if (status == 0)
    strncpy(hdr.host, hostname, CN_HOST_HOSTNAME_LENGTH);

  // Set actual ip addr in header

  char ip_addr[CN_HOST_IPV4_LENGTH_D] = {'\0'};
  result = cn_host_ipv4("eth0", ip_addr);
  if (result == 0)
    strncpy(hdr.ipv4, ip_addr, CN_HOST_IPV4_LENGTH);

  // Set actual timestamp in header

  char utcdt[CN_HOST_UTCDT_LENGTH_D] = {'\0'};
  result = cn_host_utcdt(utcdt);
  if (result == 0)
    strncpy(hdr.timestamp, utcdt, CN_HOST_UTCDT_LENGTH);

  // Set actual checkid in header
  if (checkid != NULL)
    strncpy(hdr.checkid, checkid, SN_FILE_HEADER_CHECKID_LENGTH_D);

  // Call the function to generate the unique filename

  sn_fname_get_tmp_path(&fname, file_path);

  // Open sn1ff file

  FILE *file = fopen(file_path, "w");
  if (file == NULL) {
    cn_log_msg(
        LOG_ERR, __func__,
        "'fopen' gave an error opening file -> %s <-, strerror(errno) -> %m <-",
        file_path);
    return -2;
  }

  // Write header to file

  sn_file_write_header(file, &hdr);

  fclose(file);

  return 0;
}

/*----------------------------------------------------------------.
 |                                                                |
 |  Read sn1ff file                                               |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Parse file
 *
 * Param(s):
 *   - In: file path
 *   - In/Out: file_data
 *
 * @return  0 success
 *         -1 Failed to open file
 *         -2 Failed to lock file for reading
 *         -3 Failed to extract filename from path
 *         -4 Failed to extract header value 'Host: '
 *         -5 Failed to extract header value 'IPv4: '
 *         -6 Failed to extract header value 'At: '
 *         -7 Failed to extract header value 'CheckID: '
 */
int sn_file_read(const char *filename, FILE_DATA *file_data) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    cn_log_msg(LOG_ERR, __func__,
               "'fopen' gave an error opening sn1ff file -> %s <-, "
               "strerror(errno) -> %m <-",
               filename);
    return -1;
  }

  // Shared lock for reading

  int fd = fileno(file); // Convert FILE* to file descriptor
  if (flock(fd, LOCK_SH) == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'flock' failed to lock file -> %s <-, strerror(errno) -> %m <-",
               filename);
    fclose(file);
    return -2;
  }

  char line[512];
  memset(file_data, 0, sizeof(FILE_DATA));

  /*
   * Extract file name from file path
   */

  // Extract "file name" from arg_f
  char extracted_filename[CNAME_NAME_LENGTH_D] = {'\0'};
  memset(extracted_filename, '\0', (CNAME_NAME_LENGTH_D) * sizeof(char));

  int result = cn_fpath_get_name(filename, extracted_filename);
  if (result != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Failed to extract filename from path -> %d\n\n", result);
    fclose(file);
    return -3;
  }

  /*
   * Parse extracted file name
   */

  CName name;
  sn_cname_parse_name(extracted_filename, &name);

  /*
   * Get status from parsed file name
   */

  sn_cname_get_status(&name, file_data->attributes.status);

  /*
   * Parse file header values
   */

  while (fgets(line, sizeof(line), file)) {
    cn_string_trim_newline(line);

    // Empty line, done with the header

    if (strlen(line) == 0)
      break;

    if (strncmp(line, "Host: ", 6) == 0) {
      if (cn_string_cp(file_data->header.host, sizeof(file_data->header.host),
                       line + 6) != 0) {
        fclose(file);
        return -4;
      }
    } else if (strncmp(line, "IPv4: ", 6) == 0) {
      if (cn_string_cp(file_data->header.ipv4, sizeof(file_data->header.ipv4),
                       line + 6) != 0) {
        fclose(file);
        return -5;
      }
    } else if (strncmp(line, "At: ", 4) == 0) {
      if (cn_string_cp(file_data->header.timestamp,
                       sizeof(file_data->header.timestamp), line + 4) != 0) {
        fclose(file);
        return -6;
      }
    } else if (strncmp(line, "CheckID: ", 9) == 0) {
      if (cn_string_cp(file_data->header.checkid,
                       sizeof(file_data->header.checkid), line + 9) != 0) {
        fclose(file);
        return -7;
      }
    }
  }

  // Parse file body values

  while (fgets(line, sizeof(line), file) &&
         file_data->body_lines < SN_FILE_MAX_BODY_LINES) {
    cn_string_trim_newline(line);
    size_t len = strlen(line);

    // Copy at most 80 characters and pad with spaces

    memset(file_data->body[file_data->body_lines], ' ',
           SN_FILE_MAX_BODY_LENGTH);
    memcpy(file_data->body[file_data->body_lines], line,
           len < SN_FILE_MAX_BODY_LENGTH ? len : SN_FILE_MAX_BODY_LENGTH);

    // Ensure null termination

    file_data->body[file_data->body_lines][SN_FILE_MAX_BODY_LENGTH] = '\0';

    file_data->body_lines++;
  }

  // Unlock the file when done

  if (flock(fd, LOCK_UN) == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'flock' gave an error trying to unlock file -> %s <-, "
               "strerror(errno) -> %m <-",
               filename);
    fclose(file);
    return 1;
  }

  fclose(file);
  return 0;
}

/*----------------------------------------------------------------.
 |                                                                |
 |  Delete sn1ff file                                             |
 |                                                                |
 '----------------------------------------------------------------*/

#define DELETE_PREFIX ".deleted."

void sn_file_delete(const char *file_dir, const char *file_name) {
  char file_path[256];
  char new_file_path[256];
  snprintf(file_path, sizeof(file_path), "%s/%s", file_dir, file_name);

  // Open file for locking

  int fd = open(file_path, O_RDONLY);
  if (fd == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'open' gave an error opening file for file for deletion -> %s "
               "<-, strerror(errno) -> %m <-",
               file_name);
    return;
  }

  // Exclusive lock to ensure no other process is reading

  if (flock(fd, LOCK_EX) == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'flock' gave an error trying to lock file for deletion -> %s "
               "<-, strerror(errno) -> %m <-",
               file_name);
    close(fd);
    return;
  }

  // Rename before deleting

  snprintf(new_file_path, sizeof(new_file_path), "%s/%s%s", file_dir,
           DELETE_PREFIX, file_name);

  if (rename(file_path, new_file_path) == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'rename' gave an error trying to rename file from -> %s <-, "
               "to -> %s <-, strerror(errno) -> %m <-",
               file_name, new_file_path);
    flock(fd, LOCK_UN);
    close(fd);
    return;
  }

  // Now delete safely

  if (unlink(new_file_path) == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'unlink' gave error deleting renamed file -> %s <-, "
               "strerror(errno) -> %m <-",
               new_file_path);
  } else {
    cn_log_msg(LOG_DEBUG, "Deleted file -> %s <-", file_name);
  }

  flock(fd, LOCK_UN);
  close(fd);
}

/*----------------------------------------------------------------.
 |                                                                |
 |  Copy sn1ff file                                               |
 |                                                                |
 '----------------------------------------------------------------*/

int sn_file_copy(const char *from_dir, const char *to_dir,
                 const char *file_name) {
  char source_path[1024];
  char dest_path[1024];
  char tmp_path[1024];

  int input = -1, output = -1;
  int result = 0;

  // Construct full source, destination, and temporary file paths

  snprintf(source_path, sizeof(source_path), "%s/%s", from_dir, file_name);
  snprintf(dest_path, sizeof(dest_path), "%s/%s", to_dir, file_name);
  snprintf(tmp_path, sizeof(tmp_path), "%s/.%s.tmp", to_dir, file_name);

  // Open source file

  if ((input = open(source_path, O_RDONLY)) == -1) {
    perror("open source");
    return -1;
  }

  // Open/create temp destination file with mode 0600

  if ((output = creat(tmp_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
    perror("creat temporary destination");
    close(input);
    return -1;
  }

  // Get size of the source file

  struct stat file_stat;
  if (fstat(input, &file_stat) == -1) {
    perror("fstat");
    result = -1;
    goto cleanup;
  }

  // Copy data using sendfile()

  off_t offset = 0;
  while (offset < file_stat.st_size) {
    ssize_t sent = sendfile(output, input, &offset, file_stat.st_size - offset);
    if (sent == -1) {
      perror("sendfile");
      result = -1;
      goto cleanup;
    }
  }

  // Close files before renaming

  close(input);
  close(output);
  input = output = -1;

  // Rename temp file to final destination (atomic if on same filesystem)

  if (rename(tmp_path, dest_path) == -1) {
    perror("rename");
    result = -1;
  }

  return result;

cleanup:
  if (input != -1)
    close(input);
  if (output != -1)
    close(output);
  unlink(tmp_path); // Remove incomplete temp file on error
  return result;
}
