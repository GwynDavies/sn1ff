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

#define _POSIX_C_SOURCE 200809L // For kill in <signal.h>

#include "cn_remotefe.h"
#include "cn_log.h"

// Global variable to store the PID of the child process

pid_t child_pid = -1;

/*
 * Signal handler for timeout (SIGALRM)
 */
void timeout_handler() {
  if (child_pid > 0) {
    cn_log_msg(LOG_ERR, __func__,
               "Timeout reached. Terminating the SCP process, for CHILD PID -> "
               "%" PRIdMAX,
               (intmax_t)child_pid);

    kill(child_pid, SIGKILL); // Kill the child process (SCP)
  }
}

int cn_remotefe_scp(const char *local_file, const char *remote_dest,
                    int timeout_seconds) {
  pid_t pid = fork(); // Create a new process

  if (pid == -1) {
    cn_log_msg(LOG_ERR, __func__,
               "'fork' gave error, strerror(errno) -> %m <-");
    return 1;
  }

  if (pid == 0) {
    /*
     * Child process: execute scp command
     */

    char *args[] = {"scp", (char *)local_file, (char *)remote_dest, NULL};

    if (execvp(args[0], args) == -1) {
      cn_log_msg(LOG_ERR, __func__,
                 "'execvp' gave error, strerror(errno) -> %m <-");
      EXIT_FAILURE;
    }
  } else {
    /*
     * Parent process: set the timeout and wait for the child process
     */

    // Set the timeout handler

    signal(SIGALRM, timeout_handler);

    // Start the timer (set an alarm to send SIGALRM after `timeout_seconds`
    // seconds)

    alarm(timeout_seconds);

    // Save the child PID to terminate it in case of timeout

    child_pid = pid;

    // Wait for the child process (the scp process) to finish

    int status;
    waitpid(pid, &status, 0);

    // Cancel the alarm if the child process finished successfully

    alarm(0);

    // Check if scp was successful (child process exited normally)

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      // SCP was successful, now delete the local file

      if (remove(local_file) == 0) {
        cn_log_msg(LOG_DEBUG, __func__, "Local file deleted okay -> %s <-",
                   local_file);
      } else {
        cn_log_msg(LOG_ERR, __func__,
                   "'remove' gave error removing file -> %s <-, "
                   "strerror(errno) -> %m <-",
                   local_file);
      }
    } else {
      // SCP failed or was terminated due to timeout
      if (WIFSIGNALED(status)) {
        cn_log_msg(LOG_ERR, __func__,
                   "SCP was terminated by a signal. Timeout may have occurred");
      } else {
        cn_log_msg(LOG_ERR, __func__,
                   "SCP failed. Local file -> %s <- was not deleted",
                   local_file);
      }
    }
  }

  return 0;
}
