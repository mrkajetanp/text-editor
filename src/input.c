#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "screen.h"
#include "input.h"
#include "render.h"

void input_loop(Screen s) {
    while (true) {
        /* render_screen(s); */
        render_screen_debug(s);
        insert_mode(s);
        erase();
    }
}

void insert_mode(Screen s) {
    int c = getch();

    switch (c) {

    case '\n':
        handle_enter(s);
        break;

    case 127:
    case KEY_BACKSPACE:
        handle_backspace(s);
        break;

    case KEY_LEFT:
        handle_key_left(s);
        break;

    case KEY_RIGHT:
        handle_key_right(s);
        break;

    case 'q':
        handle_quit(s);
        break;

    default:
        handle_insert_char(s, c);
        break;

    }
}


void handle_insert_char(Screen s, char c) {
    gap_buffer_put(s->cur_line->data, c);
    s->col++;
}

void handle_key_left(Screen s) {
    if (s->row == 0 && s->col == 0)
        return;

    if (s->col == 0) {
        s->row--;
        s->cur_line = s->cur_line->prev;
        int gap_size = ((gap_T)s->cur_line->data)->gap_end -
            ((gap_T)s->cur_line->data)->gap_start+1;
        s->col = ((gap_T)s->cur_line->data)->end - gap_size;

        gap_T curr_buff = (gap_T)s->cur_line->data;
        gap_buffer_move_cursor(curr_buff, gap_buffer_distance_to_end(curr_buff)-1);
    } else {
        s->col--;
        gap_buffer_move_cursor(s->cur_line->data, -1);
    }
}


void handle_key_right(Screen s) {
    int gap_size = ((gap_T)s->cur_line->data)->gap_end -
        ((gap_T)s->cur_line->data)->gap_start+1;
    int line_end = ((gap_T)s->cur_line->data)->end - gap_size;

    if (s->row+1 == s->n_lines && s->col == line_end)
        return;

    if (s->col == line_end) {
        s->row++;
        s->col = 0;
        s->cur_line = s->cur_line->next;
        gap_T curr_buff = (gap_T)s->cur_line->data;
        gap_buffer_move_cursor(curr_buff, gap_buffer_distance_to_start(curr_buff));
    } else {
        s->col++;
        gap_buffer_move_cursor(s->cur_line->data, 1);
    }
}

void handle_enter(Screen s) {
    screen_new_line_under(s);
    s->row++;
    s->col = 0;
}

void handle_backspace(Screen s) {
    // TODO: fix cursor when moving up by deleting a line
    if (s->row == 0 && s->col == 0)
        return;

    if (s->col == 0) {
        screen_destroy_line(s);
        s->row--;
        int gap_size = ((gap_T)s->cur_line->data)->gap_end -
            ((gap_T)s->cur_line->data)->gap_start+1;
        s->col = ((gap_T)s->cur_line->data)->end - gap_size;
    } else {
        gap_buffer_delete(s->cur_line->data);
        s->col--;
    }
}

void handle_quit(Screen s) {
    endwin();
    screen_destroy(s);
    exit(1);
}
