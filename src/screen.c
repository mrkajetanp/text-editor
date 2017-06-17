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
#include <assert.h>

#include "screen.h"
#include "render.h"
#include "lib/gap_buffer.h"

Line line_create() {
    /* create a buffer for the new line */
    gap_T new_line = gap_buffer_new();

    /* add \n to the line and move the cursor one character to the left */
    gap_buffer_put(new_line, '\n');
    gap_buffer_move_cursor(new_line, -1);

    /* allocate memory for the new line */
    Line l = malloc(sizeof *l);

    l->buff = new_line;
    l->visual_cursor = 0;
    l->visual_end = 0;

    l->wrap = 0;
    l->wraps = 0;

    return l;
}

void line_destroy(Line l) {
    gap_buffer_destroy(l->buff);
    free(l);
}

/* initializes the screen & its buffer */
Screen screen_init(struct Arguments* args) {
    Screen s = malloc(sizeof *s);
    Line new_line = line_create();

    s->lines = NULL; /* start with an empty list */
    s->lines = g_list_append(s->lines, new_line); /* add the first line */
    s->cur_line = s->lines; /* set the current line pointer */

    s->cur_l_num = 0; /* first line number (index) is 0 */
    s->n_lines = 1; /* initial number of lines is 1 */

    s->col = 0; /* visual cursor - first column */
    s->row = 0; /* visual cursor - first row */

    /* default number of rows and cols, useful for debugging */
    s->rows = 10;
    s->cols = 30;

    s->top_line = s->lines; /* start rendering at the first line */

    s->top_line_num = 0; /* first top line's number is 0 */
    s->stored_col = 0; /* initial stored column to 0 */

    s->line_numbers = NULL;
    s->contents = NULL;
    s->info_bar_top = NULL;
    s->info_bar_bottom = NULL;
    s->debug_info = NULL;
    s->file = NULL;

    s->render_info_bar_bottom = true;

    s->modified = false;

    /* set argument structure */
    s->args = args;

    return s;
}

void screen_init_ncurses(Screen s) {
    /* create a window for top info bar */
    s->info_bar_top = newwin(1, COLS, 0, 0);

    /* create a window for bottom info bar */
    screen_create_info_bar_bottom(s);

    /* create a window for line numbers */
    s->line_numbers = newwin(LINES-2, 5, 1, 0);

    /* create a window for contents */
    s->contents = newwin(LINES-2,
                         (s->args->debug_mode) ? COLS-26-5 : COLS-5,
                         1, 5);

    /* set number of rows and cols depending on the window */
    s->rows = LINES-2; /* -2 for top and bottom bar */
    s->cols = (s->args->debug_mode) ? COLS-32 : COLS-6; /* -6 for line nums */

    /* if in debug mode, create additional window for debug information */
    if (s->args->debug_mode)
        s->debug_info = newwin(LINES-2, 26, 1, COLS-25);
    else
        s->debug_info = NULL;
}

/* create and enable the bottom info bar */
void screen_create_info_bar_bottom(Screen s) {
    s->render_info_bar_bottom = true;
    s->info_bar_bottom = newwin(1, COLS, LINES-1, 0);
}

/* delete and disable the bottom info bar */
void screen_delete_info_bar_bottom(Screen s) {
    s->render_info_bar_bottom = false;

    for (int i = 0 ; i < COLS ; ++i)
        mvwprintw(s->info_bar_bottom, 0, i, " ");

    wrefresh(s->info_bar_bottom);
    delwin(s->info_bar_bottom);
}

/* creates a new line under the current one */
void screen_new_line_under(Screen s) {
    /* initialize a new line */
    Line new_line = line_create();

    /* insert the new line buffer into the list in its correct position */
    s->lines = g_list_insert(s->lines, new_line, s->cur_l_num+1);

    /* set the current line to the new (next) one */
    s->cur_line = s->cur_line->next;

    /* increase the number of lines */
    s->n_lines++;
}

/* creates a new line above the current one */
void screen_new_line_above(Screen s) {
    /* initialize a new line */
    Line new_line = line_create();

    /* insert the new line buffer into the list in its correct position */
    s->lines = g_list_insert(s->lines, new_line, s->cur_l_num);

    /* increase the number of lines */
    s->n_lines++;
}

/* goes to the first line in the current screen */
void screen_go_to_first_line(Screen s) {
    s->col = 0;
    s->row = 0;

    s->cur_line = s->lines;
    s->top_line = s->lines;

    s->cur_l_num = 0;
    s->top_line_num = 0;

    CURR_LINE->visual_cursor = 0;
    gap_buffer_move_cursor(CURR_LBUF, gap_buffer_distance_to_start(CURR_LBUF));
}

/* creates a save confirmation window */
void screen_save_confirmation_window(Screen s) {
    screen_delete_info_bar_bottom(s);

    WINDOW* confirmation = newwin(3, COLS, LINES-3, 0);

    wattron(confirmation, A_REVERSE);
    for (int i = 0 ; i < COLS ; ++i)
        mvwprintw(confirmation, 0, i, " ");

    mvwprintw(confirmation, 0, 0, "Save modified buffer?");
    mvwprintw(confirmation, 1, 0, " Y");
    mvwprintw(confirmation, 2, 0, " N");
    mvwprintw(confirmation, 2, 16, "^C");

    wattroff(confirmation, A_REVERSE);

    mvwprintw(confirmation, 1, 3, "Yes");
    mvwprintw(confirmation, 2, 3, "No");
    mvwprintw(confirmation, 2, 19, "Cancel");

    wrefresh(confirmation);
    curs_set(0);
    getch();

    delwin(confirmation);
    screen_create_info_bar_bottom(s);
}

/* removes a line and frees its memory */
void screen_destroy_line(Screen s) {
    /* free the line buffer's memory */
    line_destroy(s->cur_line->data);

    /* pointer to the new current line */
    GList* new_current = s->cur_line->prev;

    /* remove the old line from the list */
    s->lines = g_list_remove_link(s->lines, s->cur_line);

    /* set current line pointer to the proper line */
    s->cur_line = new_current;

    /* decrease the number of lines */
    s->n_lines--;
}

/* helper function, frees allocated line buffer */
void free_buffer_node(gpointer data) {
    line_destroy(data);
}

/* destroyes all the lines and then the screen itself */
void screen_destroy(Screen s) {
    /* destroy each line */
    g_list_free_full(s->lines, free_buffer_node);

    /* destroy windows */
    delwin(s->line_numbers);
    delwin(s->contents);

    /* if in debug mode, destroy debug information window */
    if (s->args->debug_mode)
        delwin(s->debug_info);

    /* free allocated screen */
    free(s);
}
