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

/*****************************************************************************/
/*                                   Macros                                  */
/*****************************************************************************/

/* accessing the current line */
#define CURR_LINE ((Line)s->cur_line->data)

/* accessing the current line buffer */
#define CURR_LBUF (((Line)s->cur_line->data)->buff)

/*****************************************************************************/
/*                                  typedefs                                 */
/*****************************************************************************/

typedef unsigned int uint;

/*****************************************************************************/
/*                               Other Structs                               */
/*****************************************************************************/

struct Arguments {
    bool debug_mode; /* if debug mode is enabled */
    char* file_name; /* current file name */
};

/*****************************************************************************/
/*                                Line Struct                                */
/*****************************************************************************/

/* struct representing one line */
typedef struct _line* Line;
struct _line {
    gap_T buff; /* line's gap buffer */
    uint visual_cursor; /* line's visual cursor */
    uint visual_end; /* visual end of the line */
    uint wrap; /* current wrap number */
    uint wraps; /* number of times the line is wrapped */
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

    GList* lines; /* pointer to the first line (list pointer) */
    uint n_lines; /* number of currently existing lines */
    GList* cur_line; /* pointer to the current line */
    uint cur_l_num; /* current line number */
    uint stored_col; /* last stored column to (possibly) move to */

    /* Fields related to rendering *******************************************/

    GList* top_line; /* first rendered line */
    uint top_line_num; /* first rendered line number */
    uint row; /* visual cursor's row */
    uint col; /* visual cursor column */
    uint rows; /* number of visual rows */
    uint cols; /* number of visual columns */

    WINDOW* contents; /* window with buffer contents */
    WINDOW* line_numbers; /* window with buffer's line numbers */
    WINDOW* info_bar_top; /* top bar with useful information */
    WINDOW* info_bar_bottom; /* bottom bar with useful information */
    WINDOW* debug_info; /* window with debug information */

    FILE* file; /* currently opened file */

    /* Fields related to the program *****************************************/

    struct Arguments* args; /* struct with program arguments */
};

/* initializes the screen & its buffer */
Screen screen_init(struct Arguments*);

/* initializes the screen's ncurses windows */
void screen_init_ncurses(Screen s);

/* creates a new line under the current one */
void screen_new_line_under(Screen);

/* creates a new line above the current one */
void screen_new_line_above(Screen);

/* goes to the first line in the current screen */
void screen_go_to_first_line(Screen s);

/* removes a line and frees its memory */
void screen_destroy_line(Screen);

/* destroyes all the lines and then the screen itself */
void screen_destroy(Screen);

#endif
