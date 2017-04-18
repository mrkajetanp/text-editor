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

#ifndef TEXT_EDITOR_UTIL_H
#define TEXT_EDITOR_UTIL_H

#include <stdbool.h>

#include "screen.h"

/*****************************************************************************/
/*                                   Macros                                  */
/*****************************************************************************/

/* accessing the current line buffer */
#define CURR_LBUF ((gap_T)s->cur_line->data)

/* calculating the current gap size */
#define GAP_SIZE (CURR_LBUF->gap_end - CURR_LBUF->gap_start+1)

/*****************************************************************************/
/*                                   Functions                               */
/*****************************************************************************/

/* renders the screen */
void render_screen(Screen);

#endif
