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
    /* initially refresh stdscr */
    refresh();

    /* initially render line numbers */
    render_line_numbers(s);

    while (true) {
        /* render the buffer contents */
        render_contents(s);

        /* start insert mode */
        insert_mode(s);
    }
}

/* handles characters in insert mode */
void insert_mode(Screen s) {
    /* get a character */
    int c = getch();

    switch (c) {

    case '\n':
        handle_enter(s);

        /* number of lines changed, so re-render them */
        render_line_numbers(s);
        break;

    case '\t':
        handle_tab(s);
        break;

    case 127:
    case KEY_BACKSPACE:
        handle_backspace(s);
        break;

    case KEY_LEFT:
        handle_move_left(s);
        break;

    case KEY_RIGHT:
        handle_move_right(s);
        break;

    case KEY_UP:
        handle_move_up(s);
        break;

    case KEY_DOWN:
        handle_move_down(s);
        break;

    case KEY_RESIZE:
        /* adjust the number of rows and cols */
        s->rows = LINES;
        s->cols = (s->args->debug_mode) ? COLS-32 : COLS;

        render_line_numbers(s);
        break;

        /* ascii CAN (cancel) control character */
        /* In terminals similar to xterm it's Ctrl-X */
    case 24:
        handle_quit(s);
        break;

    default:
        if (c >= 32 && c <= 127)
            handle_insert_char(s, c);
        break;
    }
}

/* inserts a char into the current screen */
void handle_insert_char(Screen s, char c) {
    if (s->col == s->cols) {
        CURR_LINE->wraps++;
        s->col = -1;
        s->row++;
    }

    /* put a char in a line buffer */
    gap_buffer_put(CURR_LBUF, c);

    /* move visual cursor one column to the right */
    s->col++;
    CURR_LINE->visual_end++;
}

#define CURSOR_CHAR (CURR_LBUF->gap_start < CURR_LBUF->cursor) ?  \
    CURR_LBUF->cursor : CURR_LBUF->cursor-1

/* handle the left arrow key */
void handle_move_left(Screen s) {
    /* if at the beginning of the first line, do nothing */
    if (s->cur_l_num == 0 && s->col == 0)
        return;

    /* if at the beginning of the line */
    if (s->row == 0 && s->col == 0) {
        s->top_line = s->top_line->prev;
        s->top_line_num--;

        render_line_numbers(s);

        /* go to the upper line */
        s->cur_line = s->cur_line->prev;

        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LINE->visual_end;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);

        s->cur_l_num--;
    }
    else if (s->col == 0) {
        /* visual cursor to the upper row */
        s->row--;

        /* go to the upper line */
        s->cur_line = s->cur_line->prev;

        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LINE->visual_end;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);

        s->cur_l_num--;
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

#define CURR_VIS_END ((CURR_LINE->wraps != 0) ?                         \
                      CURR_LINE->visual_end - s->cols*CURR_LINE->wraps - CURR_LINE->wraps : \
                      CURR_LINE->visual_end - s->cols*CURR_LINE->wraps)

/* handle the right arrow key */
void handle_move_right(Screen s) {
    /* if at the end of the last line, do nothing */
    if (s->cur_l_num+1 == s->n_lines && s->col == CURR_VIS_END)
        return;

    /* TODO: refactor */
    /* lengthy if condition could be a macro */

    /* at the end of the bottom line */
    if (s->row+1 == s->rows && s->col == CURR_LINE->visual_end - s->cols*CURR_LINE->wraps) {
        s->top_line = s->top_line->next;
        s->top_line_num++;

        render_line_numbers(s);

        /* move one line down */
        s->cur_line = s->cur_line->next;

        s->col = 0;
        s->cur_l_num++;

        /* move actual cursor to the beginning of the line */
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
    }
    /* if at the end of the line */
    else if (s->col == CURR_LINE->visual_end - s->cols*CURR_LINE->wraps) {
        /* move one line down */
        s->cur_line = s->cur_line->next;

        /* move visual cursor to the beginning of the next line */
        s->row++;
        s->col = 0;
        s->cur_l_num++;

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
void handle_move_up(Screen s) {
    /* if at the top, do nothing */
    if (s->cur_l_num == 0)
        return;

    /* TODO: refactor this */
    if (s->row == 0) {
        s->top_line = s->top_line->prev;
        s->top_line_num--;
        s->row++;

        render_line_numbers(s);
    }

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
            handle_move_right(s);

        /* if cursor ends up further than s->col was i.e. (on a tab) move before it */
        if (old_col < s->col)
            handle_move_left(s);
    }

    s->cur_l_num--;
    /* move visual cursor one line up */
    s->row--;
}

/* handle the down arrow key */
void handle_move_down(Screen s) {
    /* if at the last line, do nothing */
    if (s->cur_l_num+1 == s->n_lines)
        return;

    /* TODO: refactor this */
    if (s->row+1 == s->rows) {
        s->top_line = s->top_line->next;
        s->top_line_num++;
        s->row--;

        render_line_numbers(s);
    }

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
            handle_move_right(s);

        /* if cursor ends up further than s->col was i.e. (on a tab) move before it */
        if (old_col < s->col)
            handle_move_left(s);
    }

    s->cur_l_num++;
    /* move visual cursor one line down */
    s->row++;
}

/* handle the enter key */
void handle_enter(Screen s) {
    if (s->col == 0) {
        /* if at the beginning of the line, just insert a line above */
        screen_new_line_above(s);

        if (s->row == 0)
            s->top_line = s->lines;

    } else if (s->col == CURR_LINE->visual_end) {
        /* if at the end of the line, insert a line below */
        screen_new_line_under(s);
    } else {
        /* if in the middle of the line, split it */
        split_line(s);
    }

    /* move the visual cursor to the beginning of the new line */
    s->row++;
    s->cur_l_num++;
    s->col = 0;

    if (s->row == s->rows) {
        s->top_line = s->top_line->next;
        s->top_line_num++;
        s->row--;
    }
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
    if (s->cur_l_num == 0 && s->col == 0 && CURR_LINE->wraps == 0)
        return;

    if (s->col == 0 && CURR_LINE->wraps > 0) {
        CURR_LINE->wraps--;
        s->col = s->cols+1;
        s->row--;
    }

    if (s->row == 0 && s->col == 0) {
        s->top_line = s->top_line->prev;
        s->top_line_num--;
        merge_line_up(s);
        s->cur_l_num--;
        s->row = 0;

        render_line_numbers(s);

        /* if at the beginning of the line */
    } else if (s->col == 0) {
        /* merge the line with the upper one */
        merge_line_up(s);
        s->cur_l_num--;
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

    /* number of tabs moved */
    int tabs = 0;

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

            if (CURR_LBUF->buffer[i] == '\t')
                tabs++;
        }
        ++i;
    }

    /* move cursor to the end of the current line (excluding \n) */
    gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);
    /* delete as many characters as we moved to the new line */
    for (i = 0 ; i < chars_to_move ; ++i)
        gap_buffer_delete(CURR_LBUF);

    /* adjust old line's visual end */
    CURR_LINE->visual_end -= chars_to_move + tabs*3;

    /* move to the newly created line */
    s->cur_line = s->cur_line->next;

    /* adjust new line's visual end */
    CURR_LINE->visual_end += chars_to_move + tabs*3;

    /* move buffer cursor to the beginning of the line */
    gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
}

/* merge the current line with the upper one */
void merge_line_up(Screen s) {
    /* store the merge point position on the upper line */
    int old_col = PREV_LINE->visual_end;

    /* store number of moved chars */
    int moved_chars = 0;

    /* number of tabs moved */
    int tabs = 0;

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

            if (CURR_LBUF->buffer[i] == '\t')
                tabs++;
        }
    }

    /* remove the current line, free its memory */
    screen_destroy_line(s);

    /* adjust merged line's visual end */
    CURR_LINE->visual_end += moved_chars + tabs*3;

    /* move the visual & actual cursor to the merge point on the previous line */
    gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
    s->row--;

    /* set visual column to the beginning of the line */
    s->col = 0;

    /* keep moving right until current column reaches the old one */
    while (s->col < old_col)
        handle_move_right(s);

    /* re-render line numbers */
    render_line_numbers(s);
}
