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
        insert_mode(s);
        erase();
    }
}

void insert_mode(Screen s) {
    int c = getch();

    /* TODO: fix moving past screen edges */
    /* TODO: use end field of Screen struct */

    switch (c) {

    case '\n':
        handle_enter(s);
        break;

    case 127:
    case KEY_BACKSPACE:
        handle_backspace(s);
        break;

    case KEY_LEFT:
        handle_key_left(s);
        break;

    case KEY_RIGHT:
        handle_key_right(s);
        break;

    case 'q':
        handle_quit(s);
        break;

    default:
        handle_insert_char(s, c);
        break;

    }
}

void handle_insert_char(Screen s, char c) {
    gap_buffer_put(s->buff, c);
    s->col++;
    s->end++;
    s->cur_line_length++;
}

void handle_key_left(Screen s) {
    if (s->buff->cursor == 0)
        return;

    if (getcurx(stdscr) == getbegx(stdscr)) {
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
}

void handle_key_right(Screen s) {
    int gap_size = s->buff->gap_end - s->buff->gap_start+1;
    int max_end;

    if (s->buff->gap_start < s->end)
        max_end = s->end+gap_size+1;
    else
        max_end = s->end;

    if (s->buff->cursor >= max_end) {
        return;
    }

    /* if (s->buff->gap_start < s->buff->cursor) */
    /*     s->real_cursor = s->buff->cursor-gap_size; */
    /* else */
    /*     s->real_cursor = s->buff->cursor+1; */

    /* s->line_end_dist = 0; */
    /* for (int i = s->buff->cursor ; i < s->end && s->buff->buffer[i] != '\n' ; ++i) { */
    /*     if (i < s->buff->gap_start || i > s->buff->gap_end) */
    /*         s->line_end_dist++; */
    /* } */

    if (s->buff->buffer[s->buff->cursor] == '\n') {
        s->row++;
        s->col = 0;
    } else {
        s->col++;
    }

    gap_buffer_move_cursor(s->buff, 1);
}

void handle_enter(Screen s) {
    s->row++;
    s->end++;
    s->col = 0;
    s->cur_line_length = 0;
    gap_buffer_put(s->buff, '\n');
}

void handle_backspace(Screen s) {
    /* do nothing if cursor is at the beginning of the buffer */
    if (s->buff->cursor == 0)
        return;

    /* if at the beginning of the line, calculate where the cursor should go */
    if (s->buff->buffer[s->buff->cursor-1] == '\n') {
        s->row--;
        /* calculate the length of the previous line */
        s->col = 0;
        for (int i = s->buff->cursor-1 ; i > 0 ; --i) {
            if (s->buff->buffer[i-1] == '\n')
                break;
            s->col++;
        }
    } else {
        /* just move the cursor left */
        s->col--;
    }

    /* in any case, remove the char from the buffer and decrement the end counter */
    s->end--;
    s->cur_line_length--;
    gap_buffer_delete(s->buff);
}

void handle_quit(Screen s) {
    endwin();
    screen_destroy(s);
    exit(1);
}
