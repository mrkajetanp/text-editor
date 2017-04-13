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

#include <glib-2.0/glib.h>

#include "lib/gap_buffer.h"

/* struct representing the screen */
typedef struct _screen* Screen;
struct _screen {
    /*************************************************************************/
    /*                     Fields related to logic                           */
    /*************************************************************************/

    /* pointer to the first line (list pointer) */
    GList* lines;

    /* number of currently existing lines */
    int n_lines;

    /* pointer to the current line */
    GList* cur_line;

    /*************************************************************************/
    /*                      Fields related to rendering                      */
    /*************************************************************************/

    /* visual cursor row */
    int row;

    /* visual cursor column */
    int col;
};

/* initializes the screen & its buffer */
Screen screen_init();

/* creates a new line under the current one */
void screen_new_line_under(Screen);

/* removes a line and frees its memory */
void screen_destroy_line(Screen);

/* destroyes all the lines and then the screen itself */
void screen_destroy(Screen);

#endif
