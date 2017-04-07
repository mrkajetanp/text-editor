/*
 * text-editor - a simple text editor
 * Copyright (C) 2017 Kajetan Puchalski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "screen.h"
#include "input.h"
#include "render.h"

/* TODO: replace calculating gap size etc. with a macro */

/* executes the input loop */
void input_loop(Screen s) {
    while (true) {
        /* render the screen in release mode */
        /* render_screen(s); */

        /* render the screen in debug mode */
        render_screen_debug(s);

        /* start insert mode */
        insert_mode(s);

        /* erase the visual window */
        erase();
    }
}

/* handles characters in insert mode */
void insert_mode(Screen s) {
    /* get a character */
    int c = getch();

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


/* inserts a char into the current screen */
void handle_insert_char(Screen s, char c) {
    /* put a char in a line buffer */
    gap_buffer_put(s->cur_line->data, c);

    /* move visual cursor one column to the right */
    s->col++;
}

/* hande the left arrow key */
void handle_key_left(Screen s) {
    /* if at the beginning of the first line, do nothing */
    if (s->row == 0 && s->col == 0)
        return;

    /* if at the beginning of the line */
    if (s->col == 0) {
        /* visual cursor to the upper row */
        s->row--;

        /* change current line */
        s->cur_line = s->cur_line->prev;

        /* temporary pointer to the current line buffer */
        gap_T curr_lbuf = (gap_T)s->cur_line->data;

        /* calculate the gap size */
        int gap_size = curr_lbuf->gap_end - curr_lbuf->gap_start+1;

        /* move visual & actual cursor to the end of the line */
        s->col = curr_lbuf->end - gap_size;
        gap_buffer_move_cursor(curr_lbuf, gap_buffer_distance_to_end(curr_lbuf)-1);
    } else {
        /* move visual & actual cursor one column to the left */
        s->col--;
        gap_buffer_move_cursor(s->cur_line->data, -1);
    }
}

/* hande the right arrow key */
void handle_key_right(Screen s) {
    /* temporary pointer to the current line buffer */
    gap_T curr_lbuf = (gap_T)s->cur_line->data;

    /* calculate the gap size */
    int gap_size = curr_lbuf->gap_end - curr_lbuf->gap_start+1;

    /* calculate the end of the line */
    int line_end = ((gap_T)s->cur_line->data)->end - gap_size;

    /* if at the end of the last line, do nothing */
    if (s->row+1 == s->n_lines && s->col == line_end)
        return;

    /* if at the end of the line */
    if (s->col == line_end) {
        /* move one line down */
        s->cur_line = s->cur_line->next;

        /* move visual cursor to the beginning of the next line */
        s->row++;
        s->col = 0;

        /* move actual cursor to the beginning of the line */
        gap_buffer_move_cursor(curr_lbuf, gap_buffer_distance_to_start(curr_lbuf));
    } else {
        /* move visual & actual cursor one column to the right */
        s->col++;
        gap_buffer_move_cursor(s->cur_line->data, 1);
    }
}

/* handle the enter key */
void handle_enter(Screen s) {
    /* create a new line under the current one */
    screen_new_line_under(s);

    /* move the visual cursor to the beginning of the new line */
    s->row++;
    s->col = 0;
}

/* handle the backspace key */
void handle_backspace(Screen s) {
    /* if at the beginning of the first line, do nothing */
    if (s->row == 0 && s->col == 0)
        return;

    /* if at the beginning of the line */
    if (s->col == 0) {
        /* remove the current line, free its memory */
        screen_destroy_line(s);

        /* calculate the gap size */
        int gap_size = ((gap_T)s->cur_line->data)->gap_end -
            ((gap_T)s->cur_line->data)->gap_start+1;

        /* move the visual & actual cursor to the end of the previous line */
        s->row--;
        s->col = ((gap_T)s->cur_line->data)->end - gap_size;
        gap_buffer_move_cursor(s->cur_line->data,
                               gap_buffer_distance_to_end(s->cur_line->data)-1);
    } else {
        /* remove the current character */
        gap_buffer_delete(s->cur_line->data);

        /* move the visual cursor to the left */
        s->col--;
    }
}

/* handle the quit command */
void handle_quit(Screen s) {
    /* end curses mode */
    endwin();

    /* destroy the current screen */
    screen_destroy(s);

    /* exit program */
    exit(0);
}
