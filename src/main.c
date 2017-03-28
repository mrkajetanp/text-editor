#include <ncurses.h>
#include "lib/gap_buffer.h"
#include "util.h"
#include "input.h"

int main () {

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    gap_T buff = gap_buffer_new();

    input_loop(buff);

    /* gap_buffer_put_str(buff, "hello"); */
    /* gap_buffer_print_curses(buff); */
    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    /* getch(); */
    /* clear(); */

    /* gap_buffer_insert(buff, 'v'); */
    /* gap_buffer_print_curses(buff); */
    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    /* getch(); */
    /* clear(); */

    /* gap_buffer_move_cursor(buff, -1); */
    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    /* gap_buffer_insert(buff, 'x'); */
    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */
    /* gap_buffer_print_curses(buff); */

    /* getch(); */
    /* clear(); */

    /* gap_buffer_move_cursor(buff, gap_buffer_distance_to_end(buff)); */
    /* gap_buffer_delete(buff); */

    /* gap_buffer_print_curses(buff); */
    /* getch(); */
    /* clear(); */

    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    /* gap_buffer_move_cursor(buff, -2); */
    /* gap_buffer_delete(buff); */
    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    /* gap_buffer_print_curses(buff); */
    /* getch(); */
    /* clear(); */


    /* gap_buffer_move_cursor(buff, gap_buffer_distance_to_end(buff)); */
    /* gap_buffer_put_str(buff, " there"); */

    /* gap_buffer_print_curses(buff); */
    /* getch(); */
    /* clear(); */

    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    /* gap_buffer_move_cursor(buff, -6); */
    /* gap_buffer_put_str(buff, " you"); */


    /* gap_buffer_print_curses(buff); */
    /* getch(); */
    /* clear(); */

    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    /* gap_buffer_move_cursor(buff, -4); */

    /* gap_buffer_set_mode(buff, REPLACE_MODE); */
    /* gap_buffer_put_str(buff, " we"); */

    /* gap_buffer_print_curses(buff); */
    /* getch(); */
    /* clear(); */


    /* gap_buffer_delete(buff); */

    /* gap_buffer_print_curses(buff); */
    /* getch(); */
    /* clear(); */

    /* gap_buffer_move_cursor(buff, gap_buffer_distance_to_end(buff)); */
    /* gap_buffer_put_str(buff, "\nsomething"); */

    /* gap_buffer_print_curses(buff); */
    /* getch(); */
    /* clear(); */

    /* printw("THE END"); */

    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    /* /\* gap_buffer_delete(buff); *\/ */

    /* /\* gap_buffer_debug(buff); *\/ */
    /* /\* gap_buffer_print(buff); *\/ */

    gap_buffer_destroy(buff);

    getch();
    endwin();

    return 0;
}
