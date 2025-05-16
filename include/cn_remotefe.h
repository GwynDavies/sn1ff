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

#ifndef CN_REMOTEFE_H
#define CN_REMOTEFE_H

#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Perform SCP transfer to the remote server, using a "fork and exec" process.
 * The SCP transfer is done by the exec'ed process. The local file is deleted if
 * the SCP is successful.
 *
 *
 * @param local_file       The full path, of the local file to be transferred to
 * the remote host by SCP.
 *
 *                         Example - /path/to/local/file.snff
 *
 *
 * @param remote_dest      The remote host details in format:
 *
 *                         Example -
 * username@remote_host:/path/to/remote/destination
 *
 *
 * @param timeout_seconds  If the SCP attempt hangs, then the process will
 * timeout in this amount of seconds
 *
 *                         Example for 60 seconds - 60
 *
 *
 * @return                 Status code:
 *                           0 - success
 *                           1 - fork failed
 */
int cn_remotefe_scp(const char *local_file, const char *remote_dest,
                    int timeout_seconds);

#endif
