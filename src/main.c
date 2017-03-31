#include <ncurses.h>

#include "screen.h"
#include "input.h"

int main () {

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
