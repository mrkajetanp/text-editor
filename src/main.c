#include <ncurses.h>

#include "lib/gap_buffer.h"
#include "input.h"

int main () {

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    gap_T buff = gap_buffer_new();

    input_loop(buff);

    gap_buffer_destroy(buff);
    endwin();
    return 0;
}
