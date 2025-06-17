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
#include "cn_proc.h"

int cn_file_copy(const char *source_filepath,
                 const char *destination_filepath) {
  cn_log_msg(
      LOG_DEBUG, __func__,
      "Copy:\n  source_filepath -> %s <-\n  destintion_filepath -> %s <-\n",
      source_filepath, destination_filepath);

  FILE *src = fopen(source_filepath, "rb");
  if (!src) {
    cn_log_msg(LOG_ERR, __func__,
               "'fopen' gave error trying to open source file -> %s <-, "
               "strerror(errno) -> %m <-",
               source_filepath);
    return -1;
  }

  FILE *dest = fopen(destination_filepath, "wb");
  if (!dest) {
    cn_log_msg(LOG_ERR, __func__,
               "'fopen' gave error trying to open destination file -> %s <-, "
               "strerror(errno) -> %m <-",
               destination_filepath);
    fclose(src);
    return -1;
  }

  char buffer[1024];
  size_t bytes;
  while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
    fwrite(buffer, 1, bytes, dest);
  }

  fclose(src);
  fclose(dest);

  return 0;
}

int cn_file_delete(const char *filepath) { return remove(filepath); }

/**
 * Change group ownership of a file to a group the current user is in.
 *
 * @param  path       Path to the file
 * @param  groupname  Target group name (must be in user's groups)
 * @return  0 success,
 *         -1 on error
 */
int cn_file_chgrp(const char *path, const char *groupname) {
  // Get group for group name

  struct group *grp = getgrnam(groupname);
  if (!grp) {
    cn_log_msg(LOG_ERR, __func__,
               "'getgrnam' gave error trying to get group name -> %s <-, "
               "strerror(errno) -> %m <-",
               groupname);
    return -1;
  }

  gid_t target_gid = grp->gr_gid;

  // Check if process is a member of the target group

  // TODO: Add check that process owns the file
  //
  // Per POSIX and Linux rules, changing the group ownership of a file is
  // allowed if:
  // - The calling process owns the file (i.e., stat(path).st_uid == getuid()),
  // and
  // - The calling process is a member of the target group ID (target_gid), or
  // has
  //     the CAP_CHOWN capability (root usually does).

  int result = cn_proc_ingrp(target_gid);
  if (result != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Process failed to show membership of group -> %s <-",
               groupname);
    return -1;
  }

  // Change group ownership (keep UID the same with -1)

  if (chown(path, -1, target_gid) != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'chown' gave error trying to change file -> %s <-, "
               "strerror(errno) -> %m <-");
    return -1;
  }

  return 0;
}

/**
 * Change file permissions to 0660 (rw-rw----)
 *
 * @param path Path to the file
 * @return 0 on success, -1 on failure
 */
int cn_file_mode660(const char *path) {
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP; // 0660

  if (chmod(path, mode) != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'chmod' gave error trying to change file -> %s <-, "
               "strerror(errno) -> %m <-",
               path);
    return -1;
  }

  return 0;
}

/**
 * Change file permissions to 0600 (rw-------)
 *
 * @param path Path to the file
 * @return 0 on success, -1 on failure
 */
int cn_file_mode600(const char *path) {
  mode_t mode = S_IRUSR | S_IWUSR; // 0600

  if (chmod(path, mode) != 0) {
    cn_log_msg(LOG_ERR, __func__,
               "'chmod' gave error trying to change file -> %s <-, "
               "strerror(errno) -> %m <-",
               path);
    return -1;
  }

  return 0;
}

/**
 * Check if character is printable
 *
 * @return 0 is printable
 *         1 is not printable
 */
static int is_printable_utf8(const unsigned char *buf, size_t *char_len) {
  mbstate_t state;
  memset(&state, 0, sizeof(state));
  wchar_t wc;
  size_t len = mbrtowc(&wc, (const char *)buf, MB_CUR_MAX, &state);

  if (len == (size_t)-1 || len == (size_t)-2 || len == 0) {
    *char_len = 1; // Skip 1 byte on invalid sequences
    return 0;
  }

  *char_len = len;

  if (iswprint(wc) || iswspace(wc)) {
    return 1;
  }
  return 0;
}

#define BUFFER_SIZE 4096

/**
 * Cleans non-printable characters from a file:
 *     * Opens the file in read-write mode,
 *     * Cleans non-printable UTF-8 characters,
 *     * Rewrites the cleaned data to the same file,
 *     * Truncates any leftover data from the original file.
 *
 * Assumptions:
 *     * The file is UTF-8 encoded,
 *     * Printable characters include standard printable Unicode
 *           characters,
 *     * Non-printable characters include control characters like
 *           0x00–0x1F, except \n, \r, and \t.
 *
 * @param  filename  is the file to clean
 * @return 0 success
 *         1 error opening file
 */
int cn_file_clean(const char *filename) {
  setlocale(LC_CTYPE, "");

  FILE *file = fopen(filename, "rb+"); // Open for reading and writing
  if (!file) {
    cn_log_msg(LOG_ERR, __func__,
               "'fopen' failed to open file ->%s<- strerror(errno) -> %m <-",
               file);
    return 1;
  }

  unsigned char in_buf[BUFFER_SIZE];
  unsigned char out_buf[BUFFER_SIZE];
  size_t read_len, write_pos = 0;

  // Read whole file into memory and clean it

  while ((read_len = fread(in_buf, 1, BUFFER_SIZE, file)) > 0) {
    size_t i = 0;
    while (i < read_len) {
      size_t char_len = 0;

      if (is_printable_utf8(&in_buf[i], &char_len)) {
        if (write_pos + char_len < BUFFER_SIZE) {
          memcpy(&out_buf[write_pos], &in_buf[i], char_len);
          write_pos += char_len;
        }
      }
      i += char_len;
    }
  }

  // Rewind and write cleaned data

  rewind(file);
  fwrite(out_buf, 1, write_pos, file);
  fflush(file);

  // Truncate the rest of the file

  ftruncate(fileno(file), write_pos);

  fclose(file);
  return 0;
}
