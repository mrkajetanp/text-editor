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

#include "screen.h"
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
    l->visual_end = 0;

    return l;
}

void line_destroy(Line l) {
    gap_buffer_destroy(l->buff);
    free(l);
}

/* initializes the screen & its buffer */
Screen screen_init(struct Arguments* args) {
    Screen s = malloc(sizeof *s);

    /* create a new (first) line */
    Line new_line = line_create();

    /* initialize the list to a NULL (empty list) */
    s->lines = NULL;

    /* add the first line to the list */
    s->lines = g_list_append(s->lines, new_line);

    /* set the current line pointer */
    s->cur_line = s->lines;

    /* set the current line number */
    s->cur_l_num = 0;

    /* set the number of lines */
    s->n_lines = 1;

    /* set the visual cursor position to the first character of the first line */
    s->col = 0;
    s->row = 0;

    /* set first line to render */
    s->top_line = s->lines;

    s->top_line_num = 0;

    s->line_numbers = NULL;
    s->contents = NULL;
    s->debug_info = NULL;

    /* set argument structure */
    s->args = args;

    return s;
}

void screen_init_ncurses(Screen s) {
    /* create a window for line numbers */
    s->line_numbers = newwin(LINES, 4, 0, 0);

    /* create a window for contents */
    s->contents = newwin(LINES,
                         (s->args->debug_mode) ? COLS-26-4 : COLS-4,
                         0, 4);

    /* if in debug mode, create additional window for debug information */
    if (s->args->debug_mode) {
        s->debug_info = newwin(LINES, 26, 0, COLS-25);
    } else {
        s->debug_info = NULL;
    }
}

/* creates a new line under the current one */
void screen_new_line_under(Screen s) {
    /* initialize a new line */
    Line new_line = line_create();

    /* insert the new line buffer into the list in its correct position */
    s->lines = g_list_insert(s->lines, new_line, s->row+1);

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
    s->lines = g_list_insert(s->lines, new_line, s->row);

    /* increase the number of lines */
    s->n_lines++;
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
