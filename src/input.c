#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "input.h"
#include "render.h"

void input_loop(gap_T buff) {
    int row = 0;
    int col = 0;
    while (true) {
        render_screen(buff);
        /* render_screen_debug(buff, &row, &col); */
        handle_insertion(buff, &row, &col);
        erase();
    }
}

void handle_insertion(gap_T buff, int* row, int* col) {
    int c = getch();

    /* TODO: fix moving past screen edges */

    switch (c) {
    case '\n':
        (*row)++;
        (*col) = 0;
        gap_buffer_put(buff, c);
        break;
    case 127:
    case KEY_BACKSPACE:
        if (buff->buffer[buff->cursor] == '\n') {
            (*row)--;
            *col = 0;
            for (int i = buff->cursor ; i > 0 ; --i) {
                if (buff->buffer[i-1] == '\n')
                    break;
                (*col)++;
            }
        } else {
            (*col)--;
        }
        gap_buffer_delete(buff);
        break;
    case KEY_LEFT:
        (*col)--;
        gap_buffer_move_cursor(buff, -1);
        break;
    case KEY_RIGHT:
        (*col)++;
        gap_buffer_move_cursor(buff, 1);
        break;
    case 'q':
        endwin();
        exit(1);
        break;
    default:
        gap_buffer_put(buff, c);
        (*col)++;
        break;
    }
}
