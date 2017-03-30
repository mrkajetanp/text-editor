#include <ncurses.h>
#include "render.h"
#include "lib/gap_buffer.h"

void render_screen(gap_T g) {
    int i;

    /* print the text stored in the buffer */
    for (i = 0 ; i <= g->end ; ++i) {
        /* skip the gap */
        if (i < g->gap_start || i > g->gap_end) {
            /* omit the null characters */
            if (g->buffer[i]) {
                printw("%c", g->buffer[i]);
            }
        }
    }

    /* TODO: fix deletion handling, consider the gap */
    /* which row should the cursor be in */
    int row = 0;
    for (i = 0 ; i < g->cursor ; ++i)
        if (g->buffer[i] == '\n')
            ++row;

    /* which column should the cursor be in */
    int col = 0;
    for (i = g->cursor ; i > 0 ; --i) {
        if (g->buffer[i-1] == '\n')
            break;
        ++col;
    }

    /* current line & column info */
    mvprintw(getmaxy(stdscr)-1, 0, "%d: %d", row+1, col);

    /* move the visual cursor to its proper position */
    move(row, col);
}

void render_screen_debug(gap_T g, int* row, int* col) {
    int i;

    /* print the buffer, replace special characters with their literals */
    for (i = 0 ; i <= g->end ; ++i) {
        /* skip the gap */
        if (i < g->gap_start || i > g->gap_end) {
            /* omit the null characters */
            if (g->buffer[i]) {
                printw("%c", g->buffer[i]);
            }
        } else {

            /* if (i == g->gap_start) */
            /*     printw("|"); */
            /* else if (i == g->gap_end) */
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
    mvprintw(0, getmaxx(stdscr)-11, "C: %d E: %d", g->cursor, g->end);
    mvprintw(1, getmaxx(stdscr)-10, "G: %d - %d", g->gap_start, g->gap_end);

    /* current line & column info */
    mvprintw(getmaxy(stdscr)-1, 0, "%d: %d", *(row)+1, *col);

    /* move the visual cursor to its proper position */
    move(*row, *col);
}
