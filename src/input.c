/************************************************************************
 * text-editor - a simple text editor                                   *
 *                                                                      *
 * Copyright (C) 2017 Kajetan Puchalski                                 *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                 *
 * See the GNU General Public License for more details.                 *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program. If not, see http://www.gnu.org/licenses/.   *
 *                                                                      *
 ************************************************************************/

#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "screen.h"
#include "input.h"
#include "render.h"

/* executes the input loop */
void input_loop(Screen s) {
    while (true) {
        /* render the screen */
        render_screen(s);

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

    case KEY_UP:
        handle_key_up(s);
        break;

    case KEY_DOWN:
        handle_key_down(s);
        break;

        /* ascii CAN (cancel) control character */
        /* In terminals similar to xterm it's Ctrl-X */
    case 24:
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
    gap_buffer_put(CURR_LBUF, c);

    /* move visual cursor one column to the right */
    s->col++;
}

/* handle the left arrow key */
void handle_key_left(Screen s) {
    /* if at the beginning of the first line, do nothing */
    if (s->row == 0 && s->col == 0)
        return;

    /* if at the beginning of the line */
    if (s->col == 0) {
        /* visual cursor to the upper row */
        s->row--;

        /* go to the upper line */
        s->cur_line = s->cur_line->prev;

        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LBUF->end - GAP_SIZE;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    } else {
        /* move visual & actual cursor one column to the left */
        s->col--;
        gap_buffer_move_cursor(CURR_LBUF, -1);
    }
}

/* handle the right arrow key */
void handle_key_right(Screen s) {
    /* temporary pointer to the current line buffer */

    /* calculate the end of the line */
    int line_end = CURR_LBUF->end - GAP_SIZE;

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
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
    } else {
        /* move visual & actual cursor one column to the right */
        s->col++;
        gap_buffer_move_cursor(CURR_LBUF, 1);
    }
}

/* handle the up arrow key */
void handle_key_up(Screen s) {
    /* TODO: remember last column position in the way emacs does */

    /* if at the top, do nothing */
    if (s->row == 0)
        return;

    /* move one line up */
    s->cur_line = s->cur_line->prev;

    /* if cursor position is bigger than the current line length */
    if (s->col > CURR_LBUF->end - GAP_SIZE) {
        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LBUF->end - GAP_SIZE;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    } else {
        /* move the actual cursor to the same position as visual */
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
        gap_buffer_move_cursor(CURR_LBUF, s->col);
    }

    /* move visual cursor one line up */
    s->row--;
}

/* handle the down arrow key */
void handle_key_down(Screen s) {
    /* if at the last line, do nothing */
    if (s->row+1 == s->n_lines)
        return;

    /* move one line down */
    s->cur_line = s->cur_line->next;

    /* if cursor position is bigger than the current line length */
    if (s->col > CURR_LBUF->end - GAP_SIZE) {
        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LBUF->end - GAP_SIZE;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    } else {
        /* move the actual cursor to the same position as visual */
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
        gap_buffer_move_cursor(CURR_LBUF, s->col);
    }

    /* move visual cursor one line down */
    s->row++;
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

        /* move the visual & actual cursor to the end of the previous line */
        s->row--;
        s->col = CURR_LBUF->end - GAP_SIZE;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    } else {
        /* remove the current character */
        gap_buffer_delete(CURR_LBUF);

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
