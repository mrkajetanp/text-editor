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

#include <stdbool.h>

#include <ncurses.h>

#include "render.h"
#include "lib/gap_buffer.h"

/* renders one line */
void render_line(gpointer data, gpointer debug_flag) {
    /* cast the void pointer to line buffer */
    gap_T buff = ((Line)data)->buff;

    for (int i = 0 ; i <= buff->end ; ++i) {

        /* skip the gap */
        if (i < buff->gap_start || i > buff->gap_end) {
            /* omit the null characters */
            if (buff->buffer[i]) {
                /*************************************************************/
                /*     Print special characters if debug mode is enabled     */
                /*************************************************************/
                if (*((bool*)debug_flag)) {
                    if (buff->buffer[i] == '\n') {
                        addch('$' | COLOR_PAIR(1));
                        addch('\n');
                    } else if (buff->buffer[i] == '\0') {
                        printw("%");
                    } else if (buff->buffer[i] == '\t') {
                        printw("|--|");
                    } else {
                        printw("%c", buff->buffer[i]);
                    }
                }
                /*********************************************************/
                /*        Print normally if debug mode is not enabled    */
                /*********************************************************/
                else {
                    if (buff->buffer[i] == '\t')
                        printw("    ");
                    else
                        printw("%c", buff->buffer[i]);
                }
            }
        }

    }
}

/* renders the screen */
void render_screen(Screen s) {
    /*************************************************************************/
    /*                       Render the current screen                       */
    /*************************************************************************/

    /* render every line */
    g_list_foreach(s->lines, render_line, &s->args->debug_mode);

    /*************************************************************************/
    /*                         Render debug mode info                        */
    /*************************************************************************/

    if (s->args->debug_mode) {

        /* number of lines */
        mvprintw(0, getmaxx(stdscr)-25, "Number of lines: %d", s->n_lines);

        /* visual cursor coordinates */
        mvprintw(1, getmaxx(stdscr)-25, "Visual col: %d row: %d", s->col, s->row);

        /* actual cursor position */
        mvprintw(2, getmaxx(stdscr)-25, "Line cursor: %d", CURR_LBUF->cursor);

        mvprintw(3, getmaxx(stdscr)-25, "Visual line end: %d",
                 ((Line)s->cur_line->data)->visual_end);

        /* end of the current line */
        mvprintw(4, getmaxx(stdscr)-25, "Line end: %d",
                 CURR_LBUF->end - GAP_SIZE);

        /* gap start & end */
        mvprintw(5, getmaxx(stdscr)-25, "Line gap: %d - %d", CURR_LBUF->gap_start,
                 CURR_LBUF->gap_end);

        /* character currently under the cursor */
        if (CURR_LBUF->buffer[CURR_LBUF->cursor] == '\n')
            mvprintw(6, getmaxx(stdscr)-25, "Line cursor on: (\\n)");
        else if (CURR_LBUF->buffer[CURR_LBUF->cursor] == '\0')
            mvprintw(6, getmaxx(stdscr)-25, "Line cursor on: (\\0)");
        else if (CURR_LBUF->buffer[CURR_LBUF->cursor] == '\t')
            mvprintw(6, getmaxx(stdscr)-25, "Line cursor on: (\\t)");
        else
            mvprintw(6, getmaxx(stdscr)-25, "Line cursor on: (%c)",
                     CURR_LBUF->buffer[CURR_LBUF->cursor]);

        mvprintw(7, getmaxx(stdscr)-25, "File name: %s", s->args->file_name);
    }

    /*************************************************************************/
    /*                        Adjust window attributes                       */
    /*************************************************************************/

    /* move visual cursor to its proper position */
    move(s->row, s->col);
}
