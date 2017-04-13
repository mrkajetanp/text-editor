#ifndef TEXT_EDITOR_INPUT_HANDLER_H
#define TEXT_EDITOR_INPUT_HANDLER_H

#include "screen.h"

/* executes the input loop */
void input_loop(Screen);

/* handles characters in insert mode */
void insert_mode(Screen);

/* inserts a char into the current screen */
void handle_insert_char(Screen, char);

/* handle the left arrow key */
void handle_key_left(Screen);

/* handle the right arrow key */
void handle_key_right(Screen);

/* handle the up arrow key */
void handle_key_up(Screen);

/* handle the enter key */
void handle_enter(Screen);

/* handle the backspace key */
void handle_backspace(Screen);

/* handle the quit command */
void handle_quit(Screen);

#endif
