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
#include <string.h>

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

    case '\t':
        handle_tab(s);
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
    CURR_LINE->visual_end++;
}

#define CURSOR_CHAR (CURR_LBUF->gap_start < CURR_LBUF->cursor) ?  \
    CURR_LBUF->cursor : CURR_LBUF->cursor-1

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
        s->col = CURR_LINE->visual_end;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    } else {

        /* move further on tab */
        if (CURR_LBUF->buffer[CURSOR_CHAR] == '\t')
            s->col-=4;
        else
            s->col--;

        /* move visual & actual cursor one column to the left */
        gap_buffer_move_cursor(CURR_LBUF, -1);
    }
}

#undef CURSOR_CHAR

/* char at the cursor, adjusted depending on the position of the gap */
#define CURSOR_CHAR (CURR_LBUF->gap_start <= CURR_LBUF->cursor) ?  \
    TEMP_CURSOR : CURR_LBUF->cursor

/* temporary macro for char at the cursor position */
#define TEMP_CURSOR (CURR_LBUF->gap_start < CURR_LBUF->cursor) ? \
    CURR_LBUF->cursor+1 : CURR_LBUF->cursor+GAP_SIZE

/* handle the right arrow key */
void handle_key_right(Screen s) {
    /* if at the end of the last line, do nothing */
    if (s->row+1 == s->n_lines && s->col == CURR_LINE->visual_end)
        return;

    /* if at the end of the line */
    if (s->col == CURR_LINE->visual_end) {
        /* move one line down */
        s->cur_line = s->cur_line->next;

        /* move visual cursor to the beginning of the next line */
        s->row++;
        s->col = 0;

        /* move actual cursor to the beginning of the line */
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
    } else {
        /* move further on tab */
        if (CURR_LBUF->buffer[CURSOR_CHAR] == '\t')
            s->col+=4;
        else
            s->col++;

        /* move visual & actual cursor one column to the right */
        gap_buffer_move_cursor(CURR_LBUF, 1);
    }
}

#undef TEMP_CURSOR
#undef CURSOR_CHAR

/* handle the up arrow key */
void handle_key_up(Screen s) {
    /* if at the top, do nothing */
    if (s->row == 0)
        return;

    /* move one line up */
    s->cur_line = s->cur_line->prev;

    /* if cursor position is bigger than the current line length */
    if (s->col > CURR_LINE->visual_end) {
        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LINE->visual_end;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    } else {
        /* move the actual cursor to the beginning of the line */
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));

        /* store the last column */
        int old_col = s->col;

        /* set visual column to the beginning of the line */
        s->col = 0;

        /* keep moving right until current column reaches the old one */
        while (s->col < old_col)
            handle_key_right(s);
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
    if (s->col > CURR_LINE->visual_end) {
        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LINE->visual_end;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    } else {
        /* move the actual cursor to the beginning of the line */
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));

        /* store the last column */
        int old_col = s->col;

        /* set visual column to the beginning of the line */
        s->col = 0;

        /* keep moving right until current column reaches the old one */
        while (s->col < old_col)
            handle_key_right(s);
    }

    /* move visual cursor one line down */
    s->row++;
}

/* handle the enter key */
void handle_enter(Screen s) {
    /* if cursor is not at the end of the line */
    if (s->col != CURR_LBUF->end - GAP_SIZE) {
        split_line(s);
    } else {
        screen_new_line_under(s);
    }

    /* move the visual cursor to the beginning of the new line */
    s->row++;
    s->col = 0;
}

void handle_tab(Screen s) {
    /* put a tab into the current line buffer */
    gap_buffer_put(CURR_LBUF, '\t');

    /* move visual cursor four columns to the right */
    s->col+=4;
    CURR_LINE->visual_end+=4;
}

#define CURSOR_CHAR (CURR_LBUF->gap_start < CURR_LBUF->cursor) ?  \
    CURR_LBUF->cursor : CURR_LBUF->cursor-1

/* handle the backspace key */
void handle_backspace(Screen s) {
    /* if at the beginning of the first line, do nothing */
    if (s->row == 0 && s->col == 0)
        return;

    /* if at the beginning of the line */
    if (s->col == 0) {
        /* merge the line with the upper one */
        merge_line_up(s);
    } else {
        /* move the visual cursor to the left */
        if (CURR_LBUF->buffer[CURSOR_CHAR] == '\t') {
            s->col-=4;
            CURR_LINE->visual_end-=4;
        } else {
            s->col--;
            CURR_LINE->visual_end--;
        }

        /* remove the current character */
        gap_buffer_delete(CURR_LBUF);
    }
}

#undef CURSOR_CHAR

/* handle the quit command */
void handle_quit(Screen s) {
    /* end curses mode */
    endwin();

    /* destroy the current screen */
    screen_destroy(s);

    /* exit program */
    exit(0);
}

/* split the current line on cursor's position */
void split_line(Screen s) {
    /* create a new line under the current one */
    screen_new_line_under(s);

    /* return to the line we're splitting */
    s->cur_line = s->cur_line->prev;

    /* number of chars to move to the new line (right of split point) */
    int chars_to_move = 0;

    /* choose starting cursor position on the split point */
    int i = (CURR_LBUF->gap_end < CURR_LBUF->cursor) ?
        CURR_LBUF->cursor+1 : CURR_LBUF->cursor;

    /* loop through the line to the end */
    while (i < CURR_LBUF->end) {
        /* skip the gap */
        if (i < CURR_LBUF->gap_start || i > CURR_LBUF->gap_end) {
            /* skip \n and \0s */
            if (CURR_LBUF->buffer[i] == '\n' || CURR_LBUF->buffer[i] == '\0')
                continue;

            /* put the char we're on in the new line */
            gap_buffer_put(NEXT_LBUF, CURR_LBUF->buffer[i]);
            chars_to_move++;
        }
        ++i;
    }

    /* move cursor to the end of the current line (excluding \n) */
    gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    /* delete as many characters as we moved to the new line */
    for (i = 0 ; i < chars_to_move ; ++i)
        gap_buffer_delete(CURR_LBUF);

    /* adjust old line's visual end */
    CURR_LINE->visual_end -= chars_to_move;

    /* move to the newly created line */
    s->cur_line = s->cur_line->next;

    /* adjust new line's visual end */
    CURR_LINE->visual_end += chars_to_move;

    /* move buffer cursor to the beginning of the line */
    gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
}

/* merge the current line with the upper one */
void merge_line_up(Screen s) {
    /* store the merge point position on the upper line */
    int old_col = PREV_LINE->visual_end;

    /* store number of moved chars */
    int moved_chars = 0;

    gap_buffer_move_cursor(PREV_LBUF, gap_buffer_distance_to_end(PREV_LBUF)-1);
    for (int i = CURR_LBUF->start ; i < CURR_LBUF->end ; ++i) {
        /* skip the gap */
        if (i < CURR_LBUF->gap_start || i > CURR_LBUF->gap_end) {
            /* skip \n and \0s */
            if (CURR_LBUF->buffer[i] == '\n' || CURR_LBUF->buffer[i] == '\0')
                continue;

            /* put the char we're on in the new line */
            gap_buffer_put(PREV_LBUF, CURR_LBUF->buffer[i]);
            moved_chars++;
        }
    }

    /* remove the current line, free its memory */
    screen_destroy_line(s);

    /* adjust merged line's visual end */
    CURR_LINE->visual_end += moved_chars;

    /* move the visual & actual cursor to the merge point on the previous line */
    gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
    s->row--;

    /* set visual column to the beginning of the line */
    s->col = 0;

    /* keep moving right until current column reaches the old one */
    while (s->col < old_col)
        handle_key_right(s);
}
