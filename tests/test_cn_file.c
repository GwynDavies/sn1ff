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
#include <criterion/criterion.h>
#include <fcntl.h>

// Helper function to create a file with content for testing
void create_test_file(const char *filename, const char *content) {
  FILE *file = fopen(filename, "wb");
  if (file) {
    fwrite(content, 1, strlen(content), file);
    fclose(file);
  }
}

// Test cn_file_copy
Test(cn_file, test_cn_file_copy_success) {
  const char *source_filepath = "source.txt";
  const char *destination_filepath = "destination.txt";
  const char *content = "This is a test file.";

  create_test_file(source_filepath, content);

  int result = cn_file_copy(source_filepath, destination_filepath);

  // Verify that the file copy function returns 0 (success)
  cr_assert_eq(result, 0, "File copy failed");

  // Verify the content of the copied file
  FILE *dest_file = fopen(destination_filepath, "rb");
  cr_assert_neq(dest_file, NULL, "Destination file not created");

  char buffer[1024];
  size_t bytes_read = fread(buffer, 1, sizeof(buffer), dest_file);
  cr_assert_eq(bytes_read, strlen(content), "File content size mismatch");
  // cr_assert_str_eq(buffer, content, "File content mismatch");

  fclose(dest_file);

  // Clean up
  remove(source_filepath);
  remove(destination_filepath);
}

Test(cn_file, test_cn_file_copy_source_open_fail) {
  const char *source_filepath = "nonexistent_source.txt";
  const char *destination_filepath = "destination.txt";

  int result = cn_file_copy(source_filepath, destination_filepath);

  // Verify that the file copy function returns -1 on failure
  cr_assert_eq(result, -1, "File copy should fail when source can't be opened");
}

Test(cn_file, test_cn_file_copy_destination_open_fail) {
  const char *source_filepath = "source.txt";
  const char *destination_filepath = "nonexistent_directory/destination.txt";
  const char *content = "This is a test file.";

  create_test_file(source_filepath, content);

  int result = cn_file_copy(source_filepath, destination_filepath);

  // Verify that the file copy function returns -1 on failure (destination can't
  // be opened)
  cr_assert_eq(result, -1,
               "File copy should fail when destination can't be opened");

  // Clean up
  remove(source_filepath);
}

// Test cn_file_delete
Test(cn_file, test_cn_file_delete_success) {
  const char *filepath = "test_delete.txt";
  const char *content = "This file will be deleted.";

  create_test_file(filepath, content);

  int result = cn_file_delete(filepath);

  // Verify that the file delete function returns 0 (success)
  cr_assert_eq(result, 0, "File delete failed");

  // Verify the file was actually deleted
  FILE *file = fopen(filepath, "rb");
  cr_assert_eq(file, NULL, "File was not deleted");
}

Test(cn_file, test_cn_file_delete_nonexistent_file) {
  const char *filepath = "nonexistent_file.txt";

  int result = cn_file_delete(filepath);

  // Verify that trying to delete a nonexistent file returns a non-zero value
  cr_assert_neq(result, 0, "Deleting a nonexistent file should fail");
}

Test(cn_file, changes_group_of_file) {
  const char *filename = "./test_temp_file";
  gid_t primary_gid = getegid();
  struct group *primary_grp = getgrgid(primary_gid);
  cr_assert(primary_grp != NULL, "Could not get primary group");

  // Get secondary groups
  int ngroups = getgroups(0, NULL);
  cr_assert(ngroups > 0, "User has no secondary groups");
  gid_t *groups = malloc(sizeof(gid_t) * ngroups);
  cr_assert(groups != NULL, "Memory allocation failed");
  getgroups(ngroups, groups);

  // Find a secondary group different from the primary
  struct group *secondary_grp = NULL;
  for (int i = 0; i < ngroups; i++) {
    if (groups[i] != primary_gid) {
      secondary_grp = getgrgid(groups[i]);
      break;
    }
  }
  free(groups);
  cr_assert(secondary_grp != NULL, "No suitable secondary group found");

  // Create temporary file
  int fd = creat(filename, 0644);
  cr_assert_neq(fd, -1, "Failed to create test file: %s", strerror(errno));
  close(fd);

  // Set initial group to primary
  int r = cn_file_chgrp(filename, primary_grp->gr_name);
  cr_assert_eq(r, 0, "Failed to set file group to primary: %s",
               strerror(errno));

  // Change group to secondary
  r = cn_file_chgrp(filename, secondary_grp->gr_name);
  cr_assert_eq(r, 0, "Failed to change file group to secondary: %s",
               strerror(errno));

  // Verify group changed
  struct stat st;
  cr_assert_eq(stat(filename, &st), 0, "stat failed: %s", strerror(errno));
  cr_assert_eq(st.st_gid, secondary_grp->gr_gid,
               "Group ID not updated to expected secondary group");

  // Cleanup
  unlink(filename);
}

// Helper to create a test file
static const char *create_temp_file(void) {
  static char path[] = "/tmp/testfileXXXXXX";
  int fd = mkstemp(path);
  if (fd == -1) {
    cr_assert_fail("Failed to create temp file: %s", strerror(errno));
  }
  close(fd);
  return path;
}

// Helper to get permissions of a file
static mode_t get_permissions(const char *filename) {
  struct stat st;
  if (stat(filename, &st) != 0) {
    cr_assert_fail("Failed to stat file: %s", strerror(errno));
  }
  return st.st_mode & 0777;
}

// Test for 0660
Test(cn_file, test_set_permissions_660) {
  const char *path = create_temp_file();
  cr_assert_eq(cn_file_mode660(path), 0, "Failed to set 0660 permissions");

  mode_t mode = get_permissions(path);
  cr_expect_eq(mode, 0660, "Expected mode 0660, got %04o", mode);

  unlink(path);
}

// Test for 0600
Test(cn_file, test_set_permissions_600) {
  const char *path = create_temp_file();
  cr_assert_eq(cn_file_mode600(path), 0, "Failed to set 0600 permissions");

  mode_t mode = get_permissions(path);
  cr_expect_eq(mode, 0600, "Expected mode 0600, got %04o", mode);

  unlink(path);
}

/*
 * cn_file_clean
 */

// Utility: Write test data to a file
void write_test_file(const char *filename, const char *data) {
  FILE *f = fopen(filename, "wb");
  cr_assert_not_null(f, "Failed to open test file for writing");
  fwrite(data, 1, strlen(data), f);
  fclose(f);
}

// Utility: Read content from a file
char *read_file(const char *filename) {
  FILE *f = fopen(filename, "rb");
  cr_assert_not_null(f, "Failed to open test file for reading");
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  char *buffer = malloc(size + 1);
  fread(buffer, 1, size, f);
  buffer[size] = '\0';
  fclose(f);
  return buffer;
}

#define TEST_FILENAME "test_input.txt"

Test(cn_file, cn_file_clean_remove_non_printable_ascii) {
  const char *input = "Hello\x01\x02World\x7F!\n\tGood\x0Bb bye";
  const char *expected = "HelloWorld!\n\tGood bye";

  write_test_file(TEST_FILENAME, input);

  // Check status code

  int result = cn_file_clean(TEST_FILENAME);
  cr_assert_eq(result, 0, "clean_file_in_place() returned non-zero");

  // Check contents

  char *output = read_file(TEST_FILENAME);
  cr_assert_str_eq(output, expected,
                   "Output mismatch: expected cleaned printable string");

  free(output);

  remove(TEST_FILENAME);
}

Test(cn_file, cn_file_clean_retains_utf8_unicode_printable) {
  const char *input = "Hello 🌍!\x01\x02\n";
  const char *expected = "Hello 🌍!\n";

  write_test_file(TEST_FILENAME, input);

  int result = cn_file_clean(TEST_FILENAME);
  cr_assert_eq(result, 0, "clean_file_in_place() returned non-zero");

  char *output = read_file(TEST_FILENAME);
  cr_assert_str_eq(output, expected,
                   "UTF-8 printable characters should be retained");

  free(output);

  remove(TEST_FILENAME);
}
