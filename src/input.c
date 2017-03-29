#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "input.h"
#include "render.h"

void input_loop(gap_T buff) {
    while (true) {
        gap_buffer_print_curses(buff);
        handle_insertion(buff);
        erase();
    }
}

void handle_insertion(gap_T buff) {
    int c = getch();

    switch (c) {
    case '\n':
        // TODO: fix enter handling when splitting the line
        gap_buffer_put(buff, c);
        break;
    case 127:
    case KEY_BACKSPACE:
        gap_buffer_delete(buff);
        break;
    case KEY_LEFT:
        gap_buffer_move_cursor(buff, -1);
        break;
    case KEY_RIGHT:
        gap_buffer_move_cursor(buff, 1);
        break;
    case 'q':
        exit(1);
        break;
    default:
        gap_buffer_put(buff, c);
        break;
    }
}
