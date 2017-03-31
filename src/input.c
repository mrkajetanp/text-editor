#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "screen.h"
#include "input.h"
#include "render.h"

void input_loop(Screen s) {
    while (true) {
        /* render_screen(s); */
        render_screen_debug(s);
        handle_insertion(s);
        erase();
    }
}

void handle_insertion(Screen s) {
    int gap_size = s->buff->gap_end - s->buff->gap_start;
    int max_end;
    int c = getch();

    /* TODO: fix moving past screen edges */
    /* TODO: use end field of Screen struct */

    switch (c) {
    case '\n':
        s->row++;
        s->end++;
        s->col = 0;
        gap_buffer_put(s->buff, c);
        break;
    case 127:
    case KEY_BACKSPACE:

        if (s->buff->buffer[s->buff->cursor] == '\n') {
            s->row--;
            s->col = 0;
            for (int i = s->buff->cursor ; i > 0 ; --i) {
                if (s->buff->buffer[i-1] == '\n')
                    break;
                s->col++;
            }
        } else {
            s->col--;
        }

        s->end--;
        gap_buffer_delete(s->buff);
        break;
    case KEY_LEFT:

        if (s->buff->cursor == 0) {
            // ...
        }
        else if (getcurx(stdscr) == getbegx(stdscr)) {
            s->row--;

            s->col = 0;
            for (int i = s->buff->cursor-1 ; i > 0 ; --i) {
                if (s->buff->buffer[i-1] == '\n')
                    break;
                s->col++;
            }
        } else {
            s->col--;
        }

        gap_buffer_move_cursor(s->buff, -1);
        break;
    case KEY_RIGHT:
        if (s->buff->gap_start < s->end)
            max_end = s->end+gap_size+1;
        else
            max_end = s->end;

        if (s->buff->cursor >= max_end) {
            break;
        }

        if (s->buff->buffer[s->buff->cursor] == '\n') {
            s->row++;
            s->col = 0;
        } else {
            s->col++;
        }

        gap_buffer_move_cursor(s->buff, 1);
        break;
    case 'q':
        endwin();
        exit(1);
        break;
    default:
        gap_buffer_put(s->buff, c);
        s->col++;
        s->end++;
        break;
    }
}
