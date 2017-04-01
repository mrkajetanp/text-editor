#include <ncurses.h>
#include "render.h"
#include "lib/gap_buffer.h"

void render_screen(Screen s) {
    int i;

    /* print the text stored in the buffer */
    for (i = 0 ; i <= s->buff->end ; ++i) {
        /* skip the gap */
        if (i < s->buff->gap_start || i > s->buff->gap_end) {
            /* omit the null characters */
            if (s->buff->buffer[i]) {
                printw("%c", s->buff->buffer[i]);
            }
        }
    }

    /* TODO: fix deletion handling, consider the gap */
    /* which row should the cursor be in */
    int row = 0;
    for (i = 0 ; i < s->buff->cursor ; ++i)
        if (s->buff->buffer[i] == '\n')
            ++row;

    /* which column should the cursor be in */
    int col = 0;
    for (i = s->buff->cursor ; i > 0 ; --i) {
        if (s->buff->buffer[i-1] == '\n')
            break;
        ++col;
    }

    /* current line & column info */
    mvprintw(getmaxy(stdscr)-1, 0, "%d: %d", row+1, col);

    /* move the visual cursor to its proper position */
    move(row, col);
}

void render_screen_debug(Screen s) {
    int i;

    /* print the buffer, replace special characters with their literals */
    for (i = 0 ; i <= s->buff->end ; ++i) {
        /* skip the gap */
        if (i < s->buff->gap_start || i > s->buff->gap_end) {
            /* omit the null characters */
            if (s->buff->buffer[i]) {
                printw("%c", s->buff->buffer[i]);
            }
        } else {

            /* if (i == s->buff->gap_start) */
            /*     printw("|"); */
            /* else if (i == s->buff->gap_end) */
            /*     printw("|"); */
            /* else */
            /*     printw("_"); */

        }

        /* get current cursor position */
        int x = getcurx(stdscr);

        /* if at the edge of the screen, go to new line */
        if (x+1 == getmaxx(stdscr))
            printw("\n");
    }

    /* debugging info */
    mvprintw(0, getmaxx(stdscr)-13, "C: %d E: %d", s->buff->cursor, s->buff->end);
    mvprintw(1, getmaxx(stdscr)-13, "G: %d - %d", s->buff->gap_start, s->buff->gap_end);
    mvprintw(2, getmaxx(stdscr)-13, "B: %d - %d", s->buff->start, s->buff->end);
    mvprintw(3, getmaxx(stdscr)-13, "End: %d", s->end);
    mvprintw(4, getmaxx(stdscr)-13, "L_end: %d", s->line_end_dist);

    if (s->buff->buffer[s->buff->cursor] == '\n')
        mvprintw(5, getmaxx(stdscr)-13, "C on: (\\n)");
    else if (s->buff->buffer[s->buff->cursor] == '\0')
        mvprintw(5, getmaxx(stdscr)-13, "C on: (\\0)");
    else
        mvprintw(5, getmaxx(stdscr)-13, "C on: (%c)", s->buff->buffer[s->buff->cursor]);

    mvprintw(6, getmaxx(stdscr)-13, "RCur: %d", s->real_cursor);

    /* current line & column info */
    mvprintw(getmaxy(stdscr)-1, 0, "%d: %d", s->row+1, s->col);

    /* move the visual cursor to its proper position */
    move(s->row, s->col);
}
