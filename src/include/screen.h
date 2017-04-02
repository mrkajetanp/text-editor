#ifndef TEXT_EDITOR_SCREEN_H
#define TEXT_EDITOR_SCREEN_H

#include <glib-2.0/glib.h>
#include "lib/gap_buffer.h"

typedef struct _screen* Screen;

struct _screen {
    // gap_T buff;
    GList* lines;
    int n_lines;
    GList* cur_line;
    int row;
    int col;
};

// initialize a new screen & its buffer
Screen screen_init();

void screen_append_new_line(Screen);

void screen_destroy_line(Screen);

// destroys the screen, freeing itself & its buffer
void screen_destroy(Screen);

#endif
