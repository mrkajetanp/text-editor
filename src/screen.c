#include <ncurses.h>
#include <stdlib.h>
#include "screen.h"

Screen screen_init() {
    Screen s = malloc(sizeof *s);

    /* s->buff = gap_buffer_new(); */
    gap_T new_line = gap_buffer_new();

    gap_buffer_put(new_line, '\n');
    gap_buffer_move_cursor(new_line, -1);

    s->lines = g_list_append(s->lines, new_line);
    s->cur_line = s->lines;
    s->n_lines = 1;
    s->col = 0;
    s->row = 0;

    return s;
}

void screen_append_new_line(Screen s) {
    gap_T new_line = gap_buffer_new();
    gap_buffer_put(new_line, '\n');
    gap_buffer_move_cursor(new_line, -1);
    s->lines = g_list_append(s->lines, new_line);

    s->cur_line = s->cur_line->next;
    s->n_lines++;
}

void screen_destroy_line(Screen s) {
    gap_buffer_destroy(s->cur_line->data);

    GList* new_current = s->cur_line->prev;
    s->lines = g_list_remove_link(s->lines, s->cur_line);
    s->cur_line = new_current;
    s->n_lines--;
}

void free_buffer_node(gpointer data) {
    gap_buffer_destroy(data);
}

void screen_destroy(Screen s) {
    g_list_free_full(s->lines, free_buffer_node);
    free(s);
}
