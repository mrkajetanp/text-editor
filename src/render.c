#include <ncurses.h>
#include "render.h"
#include "lib/gap_buffer.h"

void print_line_debug(gpointer data, gpointer user_data) {
    gap_T buff = (gap_T)data;
    for (int i = 0 ; i <= buff->end ; ++i) {
        /* skip the gap */
        if (i < buff->gap_start || i > buff->gap_end) {
            /* omit the null characters */
            if (buff->buffer[i]) {
                if (buff->buffer[i] == '\n') {
                    addch('$' | COLOR_PAIR(1));
                    addch('\n');
                } else if (buff->buffer[i] == '\0') {
                    printw("%");
                } else {
                    printw("%c", buff->buffer[i]);
                }
            }
        }
    }
    if (user_data) {}
}

void render_screen_debug(Screen s) {
    g_list_foreach(s->lines, print_line_debug, NULL);

    gap_T cur_buf = (gap_T)s->cur_line->data;
    mvprintw(0, getmaxx(stdscr)-15, "n_lines :%d", s->n_lines);
    mvprintw(1, getmaxx(stdscr)-15, "C: %d R: %d", s->col, s->row);
    mvprintw(2, getmaxx(stdscr)-15, "LCur: %d", cur_buf->cursor);
    mvprintw(3, getmaxx(stdscr)-15, "LG: %d - %d", cur_buf->gap_start,
             cur_buf->gap_end);
    mvprintw(4, getmaxx(stdscr)-15, "LE: %d", cur_buf->end);

    if (cur_buf->buffer[cur_buf->cursor] == '\n')
        mvprintw(5, getmaxx(stdscr)-15, "C on: (\\n)");
    else if (cur_buf->buffer[cur_buf->cursor] == '\0')
        mvprintw(5, getmaxx(stdscr)-15, "C on: (\\0)");
    else
        mvprintw(5, getmaxx(stdscr)-15, "C on: (%c)",
                 cur_buf->buffer[cur_buf->cursor]);

    move(s->row, s->col);
}
