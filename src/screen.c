#include <ncurses.h>
#include <stdlib.h>
#include "screen.h"

Screen screen_init() {
    Screen s = malloc(sizeof *s);

    s->buff = gap_buffer_new();
    s->row = 0;
    s->col = 0;
    s->end = 0;
    s->cur_line_length = 0;

    return s;
}

void screen_destroy(Screen s) {
    gap_buffer_destroy(s->buff);
    free(s);
}
