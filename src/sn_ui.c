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

#include "sn_ui.h"
#include "cn_log.h"
#include "sn_const.h"

#define DEFAULT_SCROLL_DELAY 250 // Default to 0.25 second (250 ms) delay

// Initial scroll speed

int SCROLL_DELAY = DEFAULT_SCROLL_DELAY;

// NCurses number of lines in screen

int NUMBER_SCREEN_ROWS = 0;

/*----------------------------------------------------------------.
 |                                                                |
 | NCurses                                                        |
 |                                                                |
 '----------------------------------------------------------------*/

/**
 * Setup NCurses screen handling
 */
void sn_ui_init(void) {
  initscr();                  // Start NCurses mode
  start_color();              // Enable color support (optional)
  noecho();                   // Don't echo input
  curs_set(FALSE);            // Hide the cursor
  NUMBER_SCREEN_ROWS = LINES; // LINES is NCurses number of lines in screen
}

/**
 * Close down NCurses screen handling
 */
void sn_ui_close(void) { endwin(); }

void sn_ui_display_no_files(int *user_cmd) {
  clear(); // Clear NCurses screen
  mvprintw(0, 0, "sn1ff");
  mvprintw(1, 0, VERSION);
  mvprintw(1, 10, "[q]quit");
  mvprintw(3, 10, "NO FILES to display");
  mvprintw(5, 10, "$ man sn1ff");
  refresh(); // Refresh NCurses screen to show updates

  timeout(0); // Non-blocking input
  int ch = USER_CMD_NONE;
  ch = getch();
  if (ch == 'q') {
    *user_cmd = USER_CMD_QUIT;
  }
}

void display_header(const char status[], const char host[], const char ipv4[],
                    const char timestamp[], const char *file_name) {
  mvprintw(0, 0, "sn1ff");
  mvprintw(0, 10, "%s", status);
  mvprintw(0, 17, "%s", host);
  mvprintw(0, 26, "%s", ipv4);
  mvprintw(0, 46, "%s", timestamp);
  int udt_symbol_offset = strlen(timestamp) + 1;
  mvprintw(0, 46 + udt_symbol_offset, "%s", "UTC");

  mvprintw(1, 0, VERSION);
  mvprintw(1, 10, "[q]quit [f]faster [s]slower [^s]pause [^q]resume [d]delete");

  mvprintw(2, 10, "%s", file_name);
}

int sn_ui_display_file(const char *file_name, const FILE_DATA *file_data,
                       int *user_cmd) {
  clear(); // Clear NCurses screen

  display_header(file_data->attributes.status, file_data->header.host,
                 file_data->header.ipv4, file_data->header.timestamp,
                 file_name);

  refresh(); // Refresh NCurses screen to show updates

  size_t SCREEN_START_LINE = 3;
  size_t SCREEN_CURRENT_LINE = SCREEN_START_LINE;
  size_t SCREEN_END_LINE = NUMBER_SCREEN_ROWS;
  size_t SCREEN_SIZE = SCREEN_END_LINE - SCREEN_START_LINE;

  // If there are body lines

  if (file_data->body_lines > 0) {
    attron(A_DIM); // Enable dimmed text attribute

    int ch;
    timeout(0); // Non-blocking input

    // Draw body lines within scroll region

    size_t file_data_lines_drawn = 0;
    while (file_data_lines_drawn < file_data->body_lines &&
           SCREEN_CURRENT_LINE < SCREEN_END_LINE) {

      mvprintw(
          SCREEN_CURRENT_LINE, 0, "%s",
          file_data->body[(file_data_lines_drawn) % file_data->body_lines]);

      file_data_lines_drawn++;
      SCREEN_CURRENT_LINE++;

      refresh(); // Refresh NCurses screen to show updates
      cn_time_sleep_millis(SCROLL_DELAY);

      ch = getch();
      if (ch == 'f' && SCROLL_DELAY > 100) {
        cn_log_msg(LOG_DEBUG, __func__,
                   "User requested display speed increase");
        SCROLL_DELAY -= 50; // Increase speed
      }
      if (ch == 's' && SCROLL_DELAY < 1000) {
        cn_log_msg(LOG_DEBUG, __func__,
                   "User requested display speed decrease");
        SCROLL_DELAY += 50; // Decrease speed
      }
      if (ch == 'q') {
        cn_log_msg(LOG_DEBUG, __func__, "User requested quit application");
        *user_cmd = USER_CMD_QUIT;
        return 0;
      }
      if (ch == 'd') {
        cn_log_msg(LOG_DEBUG, __func__,
                   "User requested delete of currently displayed sn1ff file");
        *user_cmd = USER_CMD_DELETE;
        return 0;
      }
    }

    while (file_data_lines_drawn < file_data->body_lines) {

      mvprintw(
          SCREEN_END_LINE, 0, "%s",
          file_data->body[(file_data_lines_drawn) % file_data->body_lines]);
      file_data_lines_drawn++;

      for (size_t i = 1; i <= SCREEN_SIZE; i++) {
        mvprintw(SCREEN_END_LINE - i, 0, "%s",
                 file_data->body[(file_data_lines_drawn - i) %
                                 file_data->body_lines]);
      }

      refresh(); // Refresh NCurses screen to show updates
      cn_time_sleep_millis(SCROLL_DELAY);

      ch = getch();
      if (ch == 'f' && SCROLL_DELAY > 100) {
        cn_log_msg(LOG_DEBUG, __func__,
                   "User requested display speed increase");
        SCROLL_DELAY -= 50; // Increase speed
      }
      if (ch == 's' && SCROLL_DELAY < 1000) {
        cn_log_msg(LOG_DEBUG, __func__,
                   "User requested display speed decrease");
        SCROLL_DELAY += 50; // Decrease speed
      }
      if (ch == 'q') {
        cn_log_msg(LOG_DEBUG, __func__, "User requested quit application");
        *user_cmd = USER_CMD_QUIT;
        return 0;
      }
      if (ch == 'd') {
        cn_log_msg(LOG_DEBUG, __func__,
                   "User requested delete of currently displayed sn1ff file");
        *user_cmd = USER_CMD_DELETE;
        return 0;
      }
    }

    attroff(A_DIM); // Turn off dim attribute
    refresh();      // Refresh NCurses screen to show updates

    // Pause to let user take in the display before we move on to next file

    sleep(1);
  }

  *user_cmd = 0;
  return 0;
}

void sn_ui_draw(const FILE_DATA *file_data) {
  clear();
  refresh();

  if (!file_data)
    return;

  for (size_t i = 0; i < file_data->body_lines; i++) {
    mvprintw((int)i + 1, 1, "%s", file_data->body[i]);
  }

  refresh();
}
