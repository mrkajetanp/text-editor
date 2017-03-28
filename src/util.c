/* useful functions that make sens only in the context of this project */
#include <ncurses.h>
#include <util.h>

void gap_buffer_print_curses(gap_T g) {
    for (int i = 0 ; i <= g->end ; ++i) {
        if (i < g->gap_start || i > g->gap_end) {
            if (g->buffer[i])
                printw("%c", g->buffer[i]);
        }
    }
}
