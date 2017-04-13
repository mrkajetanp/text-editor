#include <ncurses.h>
#include <argp.h>
#include <glib-2.0/glib.h>

#include "screen.h"
#include "input.h"

int main(int argc, char** argv) {
    argp_parse(0, argc, argv, 0, 0, 0);

    /* ncurses initialization */
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    /* colors */
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);

    /* create new "screen" */
    Screen s = screen_init();

    /* start input loop */
    input_loop(s);

    /* program should never get here */
    return 1;
}
