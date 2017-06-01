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

#ifndef TEXT_EDITOR_INPUT_HANDLER_H
#define TEXT_EDITOR_INPUT_HANDLER_H

#include "screen.h"

/*****************************************************************************/
/*                                  typedefs                                 */
/*****************************************************************************/

typedef unsigned int uint;

/*****************************************************************************/
/*                                   Macros                                  */
/*****************************************************************************/

/* accessing the current line */
#define CURR_LINE ((Line)s->cur_line->data)

/* accessing the previous line */
#define PREV_LINE ((Line)s->cur_line->prev->data)

/* accessing the next line */
#define NEXT_LINE ((Line)s->cur_line->next->data)

/* accessing the current line buffer */
#define CURR_LBUF (((Line)s->cur_line->data)->buff)

/* accessing the previous line buffer */
#define PREV_LBUF (((Line)s->cur_line->prev->data)->buff)

/* accessing the next line buffer */
#define NEXT_LBUF (((Line)s->cur_line->next->data)->buff)

/* calculating the current gap size */
#define GAP_SIZE (CURR_LBUF->gap_end - CURR_LBUF->gap_start+1)

/* calculates visual end of the current line */
#define VISUAL_END ((CURR_LINE->wraps == 0) ? CURR_LINE->visual_end :   \
                    ((CURR_LINE->wrap != CURR_LINE->wraps) ? s->cols :  \
                     CURR_LINE->visual_end-s->cols*CURR_LINE->wraps-CURR_LINE->wraps))

#define NETX_VISUAL_END ((CURR_LINE->wraps == 0) ? CURR_LINE->visual_end : \
                         ((CURR_LINE->wrap != CURR_LINE->wraps) ? s->cols : \
                          CURR_LINE->visual_end-s->cols*CURR_LINE->wraps-CURR_LINE->wraps))

/* accessing the current top line */
#define TOP_LINE ((Line)s->top_line->data)

/* accessing the previous top line */
#define PREV_TOP_LINE ((Line)s->top_line->prev->data)


/*****************************************************************************/
/*                                 Functions                                 */
/*****************************************************************************/

/* executes the input loop */
void input_loop(Screen);

/* handles characters in insert mode */
void insert_mode(Screen);

/* inserts a char into the current screen */
void handle_insert_char(Screen, char);

/* handle the left arrow key */
void handle_move_left(Screen);

/* handle the right arrow key */
void handle_move_right(Screen);

/* handle the up arrow key */
void handle_move_up(Screen);

/* handle the down arrow key */
void handle_move_down(Screen);

/* handle the enter key */
void handle_enter(Screen);

/* handle the tab key */
void handle_tab(Screen);

/* handle the backspace key */
void handle_backspace(Screen);

/* handle the quit command */
void handle_quit(Screen);

/* split the current line on cursor's position */
void split_line(Screen);

/* merge the current line with the upper one */
void merge_line_up(Screen);

#endif
