/* useful functions that make sens only in the context of this project */
#include <ncurses.h>
#include "util.h"
#include "lib/gap_buffer.h"

void gap_buffer_print_curses(gap_T g) {
    for (int i = 0 ; i <= g->end ; ++i) {
        if (i < g->gap_start || i > g->gap_end) {
            if (g->buffer[i]) {
                printw("%c", g->buffer[i]);
            }
        }
    }
    int x, y;
    getmaxyx(stdscr, y, x);

    // TODO: adjust for \n at the end of the line
    int row = g->cursor / (x+1);
    int col = g->cursor-(row*(x+1));

    move(row, col);
}
