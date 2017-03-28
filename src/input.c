#include <stdlib.h>
#include <ncurses.h>
#include <stdbool.h>

#include "input.h"
#include "util.h"

void input_loop(gap_T buff) {
    while (true) {
        gap_buffer_print_curses(buff);
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
    case KEY_BACKSPACE:
        gap_buffer_delete(buff);
        break;
    case KEY_LEFT:
        getyx(stdscr, x, y);
        move(y, x-1);
        refresh();
        gap_buffer_move_cursor(buff, -1);
        break;
    case 'q':
        exit(1);
        break;
    default:
        gap_buffer_put(buff, c);
        break;
    }
}
