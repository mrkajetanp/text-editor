#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "input.h"
#include "util.h"

void input_loop(gap_T buff) {
    while (true) {
        /* int x, y; */
        gap_buffer_print_curses(buff);

        /* getmaxyx(stdscr, y, x); */
        /* int row = buff->cursor / y+1; */
        /* int col = buff->cursor-(row*(y+1)); */
        /* move(row, col); */
        /* refresh(); */

        handle_insertion(buff);
        erase();
    }
}

void handle_insertion(gap_T buff) {
    int x, y;
    int c = getch();

    switch (c) {
    case '\n':
        gap_buffer_put(buff, c);
        break;
    case 127:
    case KEY_BACKSPACE:
        gap_buffer_delete(buff);
        break;
    case KEY_LEFT:
        gap_buffer_move_cursor(buff, -1);
        getyx(stdscr, y, x);
        move(y, x-1);
        refresh();
        break;
    case 'q':
        exit(1);
        break;
    default:
        gap_buffer_put(buff, c);
        break;
    }
}
