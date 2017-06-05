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
#include <assert.h>

#include "screen.h"
#include "input.h"
#include "render.h"

/* executes the input loop */
void input_loop(Screen s) {
    /* initially refresh stdscr */
    refresh();

    while (true) {
        render_info_bar_top(s);
        render_info_bar_bottom(s);
        render_line_numbers(s);
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
        s->rows = LINES-2; /* -2 for top and bottom bar */
        s->cols = (s->args->debug_mode) ? COLS-32 : COLS-6; /* -6 for line nums */
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
    /* if current line ends at the edge of the screen, it wraps */
    if (CURR_LINE->visual_end == (CURR_LINE->wraps+1)*s->cols + CURR_LINE->wraps)
        CURR_LINE->wraps++;

    /* if at the edge of the screen, move down to the next wrap */
    if (s->col == s->cols) {
        CURR_LINE->wrap++;
        s->col = -1; /* after later incrementation it'll be 0 */
        s->row++;
    }

    /* put a char in a line buffer */
    gap_buffer_put(CURR_LBUF, c);

    /* move visual cursor one column to the right */
    s->col++;
    CURR_LINE->visual_cursor++;
    CURR_LINE->visual_end++;
}

#define CURSOR_CHAR (CURR_LBUF->gap_start < CURR_LBUF->cursor) ?  \
    CURR_LBUF->cursor : CURR_LBUF->cursor-1

/* handle the left arrow key */
void handle_move_left(Screen s) {
    /* if at the beginning of the first line, do nothing */
    if (s->cur_l_num == 0 && s->col == 0 && CURR_LINE->wrap == 0)
        return;

    /* at the beginning of a wrap */
    if (s->col == 0 && CURR_LINE->wrap != 0 && CURR_LINE->wraps != 0) {
        s->col = s->cols;
        s->row--;
        CURR_LINE->wrap--;

        CURR_LINE->visual_cursor--;
        gap_buffer_move_cursor(CURR_LBUF, -1);
    }
    /* at the beginning of a top line */
    else if (s->row == 0 && s->col == 0) {
        s->top_line = s->top_line->prev;
        s->cur_line = s->cur_line->prev;


        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LINE->visual_end;
        CURR_LINE->visual_cursor = VISUAL_END;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);

        s->top_line_num--;
        s->cur_l_num--;
    }
    /* at the beginning of a line */
    else if (s->col == 0) {
        /* visual cursor to the upper row */
        s->row--;

        /* go to the upper line */
        s->cur_line = s->cur_line->prev;

        /* move visual & actual cursor to the end of the line */
        s->col = VISUAL_END;
        CURR_LINE->visual_cursor = CURR_LINE->visual_end;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);

        s->cur_l_num--;
    } else {

        /* move further on tab */
        if (CURR_LBUF->buffer[CURSOR_CHAR] == '\t') {
            s->col -= 4;
            CURR_LINE->visual_cursor -= 4;
        } else {
            s->col--;
            CURR_LINE->visual_cursor--;
        }

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
void handle_move_right(Screen s) {
    /* if at the end of the last line, do nothing */
    if (s->cur_l_num+1 == s->n_lines && CURR_LINE->wraps == CURR_LINE->wrap &&
        s->col == VISUAL_END)
        return;

    /* at the end of the bottom line */
    if (s->row+1 == s->rows && s->col == VISUAL_END) {
        s->top_line = s->top_line->next;
        s->top_line_num++;

        /* move one line down */
        s->cur_line = s->cur_line->next;

        s->col = 0;
        CURR_LINE->visual_cursor = 0;
        s->cur_l_num++;

        /* move actual cursor to the beginning of the line */
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
    }
    /* wrap end of a wrap */
    else if (s->col == s->cols && CURR_LINE->wrap != CURR_LINE->wraps) {
        s->col = 0;
        s->row++;
        CURR_LINE->wrap++;
        CURR_LINE->visual_cursor++;
        gap_buffer_move_cursor(CURR_LBUF, 1);
    }
    /*  end of the line */
    else if (s->col == VISUAL_END) {
        /* move one line down */
        s->cur_line = s->cur_line->next;

        /* move visual cursor to the beginning of the next line */
        s->row++;
        s->col = 0;
        s->cur_l_num++;
        CURR_LINE->visual_cursor = 0;

        /* move actual cursor to the beginning of the line */
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
    } else {
        /* move further on tab */
        if (CURR_LBUF->buffer[CURSOR_CHAR] == '\t') {
            s->col+=4;
            CURR_LINE->visual_cursor += 4;
        } else {
            s->col++;
            CURR_LINE->visual_cursor++;
        }

        /* move visual & actual cursor one column to the right */
        gap_buffer_move_cursor(CURR_LBUF, 1);
    }
}

#undef TEMP_CURSOR
#undef CURSOR_CHAR

/* handle the up arrow key */
void handle_move_up(Screen s) {
    /* if at the top, do nothing */
    if (s->cur_l_num == 0 && CURR_LINE->wrap == 0)
        return;

    /* top line, move rendered lines up */
    if (s->row == 0) {
        s->top_line = s->top_line->prev;
        s->top_line_num--;
        s->row++; /* it balances out to 0 */
    }

    /* TODO: possible performance improvements? */

    if (CURR_LINE->wraps != 0 && CURR_LINE->wrap != 0) {
        uint new_row = s->row-1;
        uint new_col = s->col;

        /* keep moving left until the cursor reaches the previous row */
        while (s->row > new_row)
            handle_move_left(s);

        /* keep moving left until the cursor hits the previous column
           or the beginning */
        while (s->col > new_col && s->col != 0)
            handle_move_left(s);
    }
    /* moving onto a wrapped line */
    else if (PREV_LINE->wraps != 0) {
        s->cur_line = s->cur_line->prev;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));

        uint new_row = s->row-1;
        uint new_col = s->col;

        /* go to the beginning of the line */
        s->row--;
        s->row -= CURR_LINE->wraps;
        s->col = 0;
        CURR_LINE->wrap = 0;
        CURR_LINE->visual_cursor = 0;

        while (s->row < new_row) {
            handle_move_right(s);
        }

        while (s->col < new_col && s->col != VISUAL_END)
            handle_move_right(s);

        s->cur_l_num--;
    }
    /* cursor position is bigger than the previous line length */
    else if (s->col > PREV_LINE->visual_end) {
        s->cur_line = s->cur_line->prev;

        s->stored_col = s->col; /* store the column */

        /* move visual & actual cursor to the end of the line */
        s->col = CURR_LINE->visual_end;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);

        CURR_LINE->visual_cursor = CURR_LINE->visual_end;
        s->cur_l_num--;
        s->row--;
    } else {
        s->cur_line = s->cur_line->prev;

        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
        uint old_col = (s->stored_col > CURR_LINE->visual_end) ? s->col :
            s->stored_col;

        s->col = 0;

        /* keep moving right until current column reaches the old one */
        while (s->col < old_col)
            handle_move_right(s);

        /* if cursor ends up further than s->col was, i.e. on a tab, move before it */
        if (old_col < s->col)
            handle_move_left(s);

        CURR_LINE->visual_cursor = NEXT_LINE->visual_cursor;
        s->cur_l_num--;
        s->row--;
    }
}

/* handle the down arrow key */
void handle_move_down(Screen s) {
    /* the last line, do nothing */
    if (s->cur_l_num+1 == s->n_lines && CURR_LINE->wrap == CURR_LINE->wraps)
        return;

    /* the bottom line, move rendered lines down */
    if (s->row+1 == s->rows) {
        s->top_line = s->top_line->next;
        s->top_line_num++;
        s->row--; /* it balances out to 0 */
    }

    /* wrap other than the last one */
    if (CURR_LINE->wraps != 0 && CURR_LINE->wrap != CURR_LINE->wraps) {
        uint new_row = s->row+1;
        uint new_col = s->col;

        /* keep moving right until the cursor reaches the next row */
        while (s->row < new_row)
            handle_move_right(s);

        /* keep moving right until the cursor hits previous column or the end */
        while (s->col < new_col && s->col != VISUAL_END)
            handle_move_right(s);
    }
    /* if cursor position is bigger than the next line's length */
    else if (s->col > NEXT_LINE->visual_end) {
        s->cur_line = s->cur_line->next;

        s->stored_col = s->col; /* store the column */

        s->col = CURR_LINE->visual_end;
        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_end(CURR_LBUF)-1);

        CURR_LINE->visual_cursor = CURR_LINE->visual_end;
        s->cur_l_num++;
        s->row++;
    } else {
        s->cur_line = s->cur_line->next;

        gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));

        uint old_col = 0;
        if (s->col < PREV_LINE->visual_end) {
            s->stored_col = s->col;
            old_col = s->col;
        } else if (s->col == PREV_LINE->visual_end) {
            old_col = s->stored_col;
        }

        s->col = 0;
        CURR_LINE->visual_cursor = 0;

        /* keep moving right until current column reaches the old one */
        while (s->col < old_col)
            handle_move_right(s);

        /* if cursor ends up further than s->col was e.g. on a tab,
           move before it */
        if (old_col < s->col)
            handle_move_left(s);

        s->cur_l_num++;
        s->row++;
    }
}

/* handle the enter key */
void handle_enter(Screen s) {
    if (s->col == 0) {
        /* beginning of the line, just insert a line above */
        screen_new_line_above(s);

        /* top row, move rendered lines up */
        if (s->row == 0)
            s->top_line = s->lines;

    } else if (s->col == CURR_LINE->visual_end) {
        /* end of the line, insert a new line below */
        screen_new_line_under(s);
    } else {
        /* middle of the line, split it */
        split_line(s);
    }

    /* move the visual cursor to the beginning of the new line */
    s->row++;
    s->cur_l_num++;
    s->col = 0;

    /* if bottom line, move rendered lines down */
    if (s->row == s->rows) {
        s->top_line = s->top_line->next;
        s->top_line_num++;
        s->row -= 1 + PREV_TOP_LINE->wraps;
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
    /* beginning of the first line, do nothing */
    if (s->cur_l_num == 0 && s->col == 0 && CURR_LINE->wraps == 0)
        return;

    /* beginning of the wrap */
    if (s->col == 0 && CURR_LINE->wraps > 0) {

        CURR_LINE->wraps--;
        CURR_LINE->wrap--;

        s->col = s->cols+1;
        s->row--;
    }
    /* beginning of the top line */
    if (s->row == 0 && s->col == 0) {
        s->top_line = s->top_line->prev;
        s->top_line_num--;
        merge_line_up(s);
        s->cur_l_num--;
        s->row = 0;

    }
    /* beginning of the other line line */
    else if (s->col == 0) {
        /* merge the line with the upper one */
        merge_line_up(s);
        s->cur_l_num--;
    } else {
        /* move the visual cursor to the left */
        if (CURR_LBUF->buffer[CURSOR_CHAR] == '\t') {
            s->col-=4;
            CURR_LINE->visual_end-=4;
            CURR_LINE->visual_cursor-=4;
        } else {
            s->col--;
            CURR_LINE->visual_end--;
            CURR_LINE->visual_cursor--;
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
    uint old_col = PREV_LINE->visual_end;

    /* store number of moved chars */
    uint moved_chars = 0;

    /* number of tabs moved */
    uint tabs = 0;

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
}
