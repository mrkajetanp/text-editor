/*
 * text-editor - a simple text editor
 * Copyright (C) 2017 Kajetan Puchalski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

#ifndef TEXT_EDITOR_INPUT_HANDLER_H
#define TEXT_EDITOR_INPUT_HANDLER_H

#include "screen.h"

/* executes the input loop */
void input_loop(Screen);

/* handles characters in insert mode */
void insert_mode(Screen);

/* inserts a char into the current screen */
void handle_insert_char(Screen, char);

/* hande the left arrow key */
void handle_key_left(Screen);

/* hande the right arrow key */
void handle_key_right(Screen);

/* handle the enter key */
void handle_enter(Screen);

/* handle the backspace key */
void handle_backspace(Screen);

/* handle the quit command */
void handle_quit(Screen);

#endif
