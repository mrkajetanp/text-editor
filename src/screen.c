#include <ncurses.h>
#include <stdlib.h>

#include "screen.h"

/* initializes the screen & its buffer */
Screen screen_init() {
    Screen s = malloc(sizeof *s);

    /* create a buffer for the first line */
    gap_T new_line = gap_buffer_new();

    /* add \n to the line and move the cursor one character to the left */
    gap_buffer_put(new_line, '\n');
    gap_buffer_move_cursor(new_line, -1);

    /* add the first line to the list */
    s->lines = g_list_append(s->lines, new_line);

    /* set the current line pointer */
    s->cur_line = s->lines;

    /* set the number of lines */
    s->n_lines = 1;

    /* set the visual cursor position to the first character of the first line */
    s->col = 0;
    s->row = 0;

    return s;
}

/* creates a new line under the current one */
void screen_new_line_under(Screen s) {
    /* create a buffer for the new line */
    gap_T new_line = gap_buffer_new();

    /* add \n to the line and move the cursor one character to the left */
    gap_buffer_put(new_line, '\n');
    gap_buffer_move_cursor(new_line, -1);

    /* insert the new line buffer into the list in its correct position */
    s->lines = g_list_insert(s->lines, new_line, s->row+1);

    /* set the current line to the new (next) one */
    s->cur_line = s->cur_line->next;

    /* increase the number of lines */
    s->n_lines++;
}

/* removes a line and frees its memory */
void screen_destroy_line(Screen s) {
    /* free the line buffer's memory */
    gap_buffer_destroy(s->cur_line->data);

    /* pointer to the new current line */
    GList* new_current = s->cur_line->prev;

    /* remove the old line from the list */
    s->lines = g_list_remove_link(s->lines, s->cur_line);

    /* set current line pointer to the proper line */
    s->cur_line = new_current;

    /* decrease the number of lines */
    s->n_lines--;
}

/* helper function, frees allocated line buffer */
void free_buffer_node(gpointer data) {
    gap_buffer_destroy(data);
}

/* destroyes all the lines and then the screen itself */
void screen_destroy(Screen s) {
    g_list_free_full(s->lines, free_buffer_node);
    free(s);
}
