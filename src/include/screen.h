#ifndef TEXT_EDITOR_SCREEN_H
#define TEXT_EDITOR_SCREEN_H

#include "lib/gap_buffer.h"

typedef struct _screen* Screen;

struct _screen {
    gap_T buff;
    int row;
    int col;
    int end;
    int real_cursor;
    int line_end_dist;
};

// initialize a new screen & its buffer
Screen screen_init();

// destroys the screen, freeing itself & its buffer
void screen_destroy(Screen);

#endif
