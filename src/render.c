#include <ncurses.h>
#include "render.h"
#include "lib/gap_buffer.h"

void gap_buffer_print_curses(gap_T g) {
    int i;
    for (i = 0 ; i <= g->end ; ++i) {
        if (i < g->gap_start || i > g->gap_end) {
            if (g->buffer[i]) {
                printw("%c", g->buffer[i]);
            }
        }
    }

    int row = 0;
    for (i = 0 ; i < g->cursor ; ++i)
        if (g->buffer[i] == '\n')
            ++row;

    int col = 0;
    i = g->cursor;
    while(g->buffer[i-1] != '\n' && i > 0) {
        --i;
        ++col;
    }

    mvprintw(getmaxy(stdscr)-1, 0, "%d: %d", row+1, col);

    move(row, col);
}
