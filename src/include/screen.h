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

#ifndef TEXT_EDITOR_SCREEN_H
#define TEXT_EDITOR_SCREEN_H

#include <stdbool.h>
#include <glib-2.0/glib.h>
#include <argp.h>
#include <ncurses.h>

#include "lib/gap_buffer.h"

struct Arguments {
    bool debug_mode;
    char* file_name;
};

/*****************************************************************************/
/*                                Line Struct                                */
/*****************************************************************************/

/* struct representing one line */
typedef struct _line* Line;
struct _line {
    /* line gap buffer */
    gap_T buff;

    /* visual end of the line */
    int visual_end;

    /* wrap we're currently on */
    int wrap;

    /* number of times the line is wrapped */
    int wraps;
};

/* creates a new line */
Line line_create();

/* destroys a line, freeing its memory */
void line_destroy(Line);

/*****************************************************************************/
/*                               Screen Struct                               */
/*****************************************************************************/

/* struct representing the screen */
typedef struct _screen* Screen;
struct _screen {
    /* Fields related to logic ***********************************************/

    /* pointer to the first line (list pointer) */
    GList* lines;

    /* number of currently existing lines */
    int n_lines;

    /* pointer to the current line */
    GList* cur_line;

    /* number of the current line */
    int cur_l_num;

    /* Fields related to rendering *******************************************/

    /* line to start rendering at */
    GList* top_line;

    /* line number to start rendering at */
    int top_line_num;

    /* visual cursor row */
    int row;

    /* number of visual rows */
    int rows;

    /* visual cursor column */
    int col;

    /* number of visual cols */
    int cols;

    /* window with buffer contents */
    WINDOW* contents;

    /* window with buffer's line numbers */
    WINDOW* line_numbers;

    /* window with debug information */
    WINDOW* debug_info;

    /* Fields related to the program *****************************************/

    struct Arguments* args;
};

/* initializes the screen & its buffer */
Screen screen_init(struct Arguments* args);

/* initializes the screen's ncurses windows */
void screen_init_ncurses(Screen s);

/* creates a new line under the current one */
void screen_new_line_under(Screen);

/* creates a new line above the current one */
void screen_new_line_above(Screen);

/* removes a line and frees its memory */
void screen_destroy_line(Screen);

/* destroyes all the lines and then the screen itself */
void screen_destroy(Screen);

#endif
