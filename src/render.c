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
#include <string.h>

#include <ncurses.h>

#include "render.h"
#include "lib/gap_buffer.h"

/* renders one line */
void render_line(gpointer data, gpointer screen) {
    /* cast the pointer to a screen */
    Screen s = (Screen)screen;

    /* cast the pointer to a line buffer */
    gap_T buff = ((Line)data)->buff;

    for (int i = 0 ; i <= buff->end ; ++i) {

        /* skip the gap */
        if (i < buff->gap_start || i > buff->gap_end) {
            /* omit the null characters */
            if (buff->buffer[i]) {
                /*************************************************************/
                /*     Print special characters if debug mode is enabled     */
                /*************************************************************/
                if (s->args->debug_mode) {
                    switch (buff->buffer[i]) {
                    case '\n':
                        if (getcurx(s->contents) != getmaxx(s->contents)-1)
                            waddch(s->contents, '$' | COLOR_PAIR(1));

                        waddch(s->contents, '\n');
                        break;

                    case '\0':
                        wprintw(s->contents, "%");
                        break;

                    case '\t':
                        wattron(s->contents, COLOR_PAIR(2));
                        wprintw(s->contents, " -> ");
                        wattroff(s->contents, COLOR_PAIR(2));
                        break;

                    default:
                        wprintw(s->contents, "%c", buff->buffer[i]);
                        break;
                    }
                }
                /*********************************************************/
                /*        Print normally if debug mode is not enabled    */
                /*********************************************************/
                else {
                    if (buff->buffer[i] == '\t')
                        wprintw(s->contents, "    ");
                    else
                        wprintw(s->contents, "%c", buff->buffer[i]);
                }
            }
        }

    }
}

/* macro for adjusting cursor position according to the gap */
#define CURSOR_CHAR (CURR_LBUF->gap_start > CURR_LBUF->cursor) ?  \
    CURR_LBUF->cursor-1 : CURR_LBUF->cursor

#define VISUAL_END ((CURR_LINE->wraps == 0) ? CURR_LINE->visual_end : \
                 ((CURR_LINE->wrap != CURR_LINE->wraps) ? s->cols :    \
                  CURR_LINE->visual_end-s->cols*CURR_LINE->wraps-CURR_LINE->wraps))

/* renders the screen */
void render_contents(Screen s) {
    /* erase previous contents */
    werase(s->contents);

    /*************************************************************************/
    /*                       Render current buffer contents                  */
    /*************************************************************************/

    /* render every line, stop if window is filled */
    uint cnt = 0;
    for (GList* curr = s->top_line ; curr != NULL ; curr = curr->next) {
        render_line(curr->data, s);
        cnt++;

        if (cnt >= s->rows)
            break;
    }

    /*************************************************************************/
    /*                         Render debug mode info                        */
    /*************************************************************************/

    if (s->args->debug_mode) {
        /* erase previous contents */
        werase(s->debug_info);

        /* render a bar separating debug info from contents ******************/
        wattron(s->debug_info, A_REVERSE);

        for (int i = 0 ; i < LINES ; ++i)
            mvwprintw(s->debug_info, i, 0, " ");

        wattroff(s->debug_info, A_REVERSE);

        /* render actual debug information ***********************************/

        /* number of lines */
        mvwprintw(s->debug_info, 0, 2, "Number of lines: %d", s->n_lines);

        /* visual cursor coordinates */
        mvwprintw(s->debug_info, 1, 2, "Visual col: %d row: %d", s->col, s->row);

        /* actual cursor position */
        mvwprintw(s->debug_info, 2, 2, "Line cursor: %d", CURR_LBUF->cursor);

        mvwprintw(s->debug_info, 3, 2, "Visual line end: %d",
                  CURR_LINE->visual_end);

        mvwprintw(s->debug_info, 4, 2, "Line wraps: %d",
                  CURR_LINE->wraps);

        /* end of the current line */
        mvwprintw(s->debug_info, 5, 2,
                  "Line end: %d", CURR_LBUF->end - GAP_SIZE);

        /* gap start & end */
        mvwprintw(s->debug_info, 6, 2,
                 "Line gap: %d - %d", CURR_LBUF->gap_start, CURR_LBUF->gap_end);


        /* character currently under the cursor */
        switch (CURR_LBUF->buffer[CURSOR_CHAR]) {

        case '\n':
            mvwprintw(s->debug_info, 7, 2, "Line cursor on: (\\n)");
            break;

        case '\0':
            mvwprintw(s->debug_info, 7, 2, "Line cursor on: (\\0)");
            break;

        case '\t':
            mvwprintw(s->debug_info, 7, 2, "Line cursor on: (\\t)");
            break;

        default:
            mvwprintw(s->debug_info, 7, 2, "Line cursor on: (%c)",
                      CURR_LBUF->buffer[CURSOR_CHAR]);
            break;
        }

        mvwprintw(s->debug_info, 8, 2, "File name: %s", s->args->file_name);

        mvwprintw(s->debug_info, 9, 2, "Top line num: %d", s->top_line_num);
        mvwprintw(s->debug_info, 10, 2, "Curr l_num: %d", s->cur_l_num);
        mvwprintw(s->debug_info, 11, 2, "s->rows: %d", s->rows);
        mvwprintw(s->debug_info, 12, 2, "s->cols: %d", s->cols);
        mvwprintw(s->debug_info, 13, 2, "line wrap: %d", CURR_LINE->wrap);
        mvwprintw(s->debug_info, 14, 2, "VISUAL_END: %d", VISUAL_END);

        mvwprintw(s->debug_info, 15, 2, "currx: %d", getcurx(s->contents));
        mvwprintw(s->debug_info, 16, 2, "maxx: %d", getmaxx(s->contents));
    }

    /*************************************************************************/
    /*                        Adjust window attributes                       */
    /*************************************************************************/

    /* move visual cursor to its proper position */
    wmove(s->contents, s->row, s->col);

    /* refresh windows *******************************************************/

    if (s->args->debug_mode)
        wrefresh(s->debug_info);

    wrefresh(s->contents);
}

#undef CURSOR_CHAR

void render_line_numbers(Screen s) {
    /* erase previous contents */
    werase(s->line_numbers);

    /* enable color for rendering lines */
    wattron(s->line_numbers, COLOR_PAIR(3));

    /* render actual numbers */
    GList* curr = s->top_line;
    uint j;
    uint line_number = s->top_line_num+1;
    uint last_line_number = s->n_lines-s->top_line_num;
    for (uint i = 0 ; i < last_line_number ; ++i) {
        mvwprintw(s->line_numbers, i, 0, "%4d", line_number);

        if (((Line)curr->data)->wraps != 0) {
            for (j = 1 ; j < ((Line)curr->data)->wraps ; ++j) {
                mvwprintw(s->line_numbers, i+j-1, 0, "");
            }

            i += j;
            last_line_number += j;
        }

        curr = curr->next;
        line_number++;
    }

    /* disable the color */
    wattroff(s->line_numbers, COLOR_PAIR(3));

    /* render tildes on non-existing lines */
    for (uint i = last_line_number ; i <= s->rows ; ++i)
        mvwprintw(s->line_numbers, i, 3, "~");

    wrefresh(s->line_numbers);
}

void render_info_bar_top(Screen s) {
    werase(s->info_bar_top);
    wattron(s->info_bar_top, A_REVERSE);

    /* render bacground ******************************************************/

    for (int i = 0 ; i < COLS ; ++i)
        mvwprintw(s->info_bar_top, 0, i, " ");

    mvwprintw(s->info_bar_top, 0, 2, "text-editor 0.1");

    /* render current file name or "New Buffer" */
    if (strlen(s->args->file_name) > 0) {
        mvwprintw(s->info_bar_top, 0, COLS/2-strlen(s->args->file_name)/2-3,
                  "File: %s", s->args->file_name);
    } else {
        mvwprintw(s->info_bar_top, 0, COLS/2-5, "New Buffer");
    }

    wattroff(s->info_bar_top, A_REVERSE);
    wrefresh(s->info_bar_top);
}


void render_info_bar_bottom(Screen s) {
    werase(s->info_bar_bottom);
    wattron(s->info_bar_bottom, A_REVERSE);

    /* render bacground ******************************************************/

    for (int i = 0 ; i < COLS ; ++i)
        mvwprintw(s->info_bar_bottom, 0, i, " ");

    /* render current line and column number */
    mvwprintw(s->info_bar_bottom, 0, COLS-11, "%4d:%-4d",
              s->cur_l_num, s->col);

    wattroff(s->info_bar_bottom, A_REVERSE);
    wrefresh(s->info_bar_bottom);
}
