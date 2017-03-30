#ifndef TEXT_EDITOR_INPUT_HANDLER_H
#define TEXT_EDITOR_INPUT_HANDLER_H

#include "lib/gap_buffer.h"

void input_loop(gap_T);
void handle_insertion(gap_T, int*, int*);

#endif
