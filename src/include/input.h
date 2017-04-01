#ifndef TEXT_EDITOR_INPUT_HANDLER_H
#define TEXT_EDITOR_INPUT_HANDLER_H

#include "screen.h"

void input_loop(Screen);
void insert_mode(Screen);

void handle_insert_char(Screen, char);
void handle_key_left(Screen);
void handle_key_right(Screen);
void handle_enter(Screen);
void handle_backspace(Screen);
void handle_quit(Screen);

#endif
