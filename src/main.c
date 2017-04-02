#include <ncurses.h>

#include "screen.h"
#include "input.h"

int main () {
    // TODO: change structure to the doubly linked list of gap buffers
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    Screen s = screen_init();

    input_loop(s);

    screen_destroy(s);
    endwin();
    return 0;
}
