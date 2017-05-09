#include <stdlib.h>

#include <check.h>
#include <glib-2.0/glib.h>

#include "lib/gap_buffer.h"
#include "screen.h"
#include "input.h"

/*****************************************************************************/
/*                                   Macros                                  */
/*****************************************************************************/

/* accessing the current line */
#define CURR_LINE ((Line)s->cur_line->data)

/* accessing the current line buffer */
#define CURR_LBUF (((Line)s->cur_line->data)->buff)

/* accessing the previous line */
#define PREV_LINE ((Line)s->cur_line->prev->data)

/*****************************************************************************/
/*                                   Tests                                   */
/*****************************************************************************/

/* Arguments structure for testing */
struct Arguments test_arguments;

START_TEST (test_line_init) {
    Line l = line_create();

    /* line is allocated properly */
    ck_assert_ptr_nonnull(l);

    /* buffer is allocated properly */
    ck_assert_ptr_nonnull(l->buff);

    ck_assert_int_eq(0, l->buff->cursor);
    ck_assert_int_eq(0, l->visual_end);
    ck_assert_int_eq('\n', l->buff->buffer[0]);

    line_destroy(l);
} END_TEST

/* test screen initialization */
START_TEST (test_screen_init) {
    Screen s = screen_init(&test_arguments);

    /* screen is allocated properly */
    ck_assert_ptr_nonnull(s);

    /* test list of the lines ************************************************/

    /* there is a line in the list */
    ck_assert_ptr_nonnull(s->lines);

    /* current (first) line is first in the list */
    ck_assert_ptr_eq(s->lines, s->cur_line);

    /* test the first line ***************************************************/

    /* first line's buffer is properly allocated */
    ck_assert_ptr_nonnull(CURR_LBUF);

    ck_assert_int_eq(0, CURR_LINE->visual_end);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);

    /* test visual cursor ****************************************************/

    /* visual cursor and number of lines */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(0, s->row);

    /* test others ***********************************************************/

    /* ncurses windows are all NULL */
    ck_assert_ptr_eq(NULL, s->line_numbers);
    ck_assert_ptr_eq(NULL, s->contents);
    ck_assert_ptr_eq(NULL, s->debug_info);

    screen_destroy(s);
} END_TEST

START_TEST (test_new_line_under) {
    Screen s = screen_init(&test_arguments);

    /* store the previous state */
    Line old_curr = CURR_LINE;
    int old_n_lines = s->n_lines;

    screen_new_line_under(s);

    ck_assert_ptr_nonnull(s->cur_line);

    /* cur_line is after the old cur_line */
    ck_assert_ptr_eq(old_curr, PREV_LINE);

    /* s->n_lines was properly incremented */
    ck_assert_int_eq(old_n_lines, s->n_lines-1);

    screen_destroy(s);
} END_TEST

START_TEST (test_new_line_above) {
    Screen s = screen_init(&test_arguments);

    /* previous state ********************************************************/

    Line old_curr = CURR_LINE;
    ck_assert_int_eq(1, s->n_lines);

    /* actual testing ********************************************************/

    screen_new_line_above(s);

    ck_assert_ptr_nonnull(CURR_LINE);

    /* cur_line did not change */
    ck_assert_ptr_eq(old_curr, CURR_LINE);

    /* there is a line above the current one */
    ck_assert_ptr_nonnull(PREV_LINE);

    /* s->n_lines was properly incremented */
    ck_assert_int_eq(2, s->n_lines);

    screen_destroy(s);
} END_TEST

Suite* s_screen() {
    Suite* s_screen = suite_create("screen");

    TCase* tc_init = tcase_create("initialization");
    tcase_add_test(tc_init, test_line_init);
    tcase_add_test(tc_init, test_screen_init);
    suite_add_tcase(s_screen, tc_init);

    TCase* tc_lines = tcase_create("line management");
    tcase_add_test(tc_lines, test_new_line_under);
    tcase_add_test(tc_lines, test_new_line_above);
    suite_add_tcase(s_screen, tc_lines);

    return s_screen;
}

START_TEST (letter_insertion) {
    Screen s = screen_init(&test_arguments);

    /* initial state *********************************************************/

    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    /* actual insertion ******************************************************/

    handle_insert_char(s, 'a');

    /* inserted char is at the beginning of the buffer */
    ck_assert_int_eq('a', CURR_LBUF->buffer[0]);

    /* '\n' is the next char after the gap */

    /* control variables are incremented */
    ck_assert_int_eq('\n', CURR_LBUF->buffer[9]);
    ck_assert_int_eq(1, CURR_LBUF->cursor);
    ck_assert_int_eq(1, s->col);
    ck_assert_int_eq(1, CURR_LINE->visual_end);

    screen_destroy(s);
} END_TEST

START_TEST (move_left) {
    Screen s = screen_init(&test_arguments);

    /* at the beginning of the first line ************************************/

    /* initial state */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    handle_move_left(s);

    /* state has not changed */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    /* middle / end of the line **********************************************/

    handle_insert_char(s, 'a');
    handle_insert_char(s, 'b');
    handle_insert_char(s, 'c');

    /* state before move */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(3, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(3, CURR_LINE->visual_end);
    ck_assert_int_eq(3, CURR_LBUF->cursor);
    ck_assert_int_eq('c', CURR_LBUF->buffer[2]);

    handle_move_left(s);

    /* state after move */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(2, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(3, CURR_LINE->visual_end);
    ck_assert_int_eq(2, CURR_LBUF->cursor);
    ck_assert_int_eq('b', CURR_LBUF->buffer[1]);

    /* middle / end of the line on a tab *************************************/

    /* insert a tab */
    handle_tab(s);

    /* state before move */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(6, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(7, CURR_LINE->visual_end);
    ck_assert_int_eq(3, CURR_LBUF->cursor);
    ck_assert_int_eq('\t', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);

    handle_move_left(s);

    /* state after move */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(2, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(7, CURR_LINE->visual_end);
    ck_assert_int_eq(2, CURR_LBUF->cursor);
    ck_assert_int_eq('b', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);

    /* beginning of the other line *******************************************/

    /* adjust to testing position */
    handle_move_right(s);
    handle_move_right(s);
    handle_enter(s);

    /* state before move */
    ck_assert_int_eq(2, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(1, s->row);
    ck_assert_int_eq(0, CURR_LINE->visual_end);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[CURR_LBUF->cursor]);

    handle_move_left(s);

    /* state after move, at the end of the upper line */
    ck_assert_int_eq(2, s->n_lines);
    ck_assert_int_eq(7, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(7, CURR_LINE->visual_end);
    ck_assert_int_eq(8, CURR_LBUF->cursor);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[CURR_LBUF->cursor+1]);

    screen_destroy(s);
} END_TEST

START_TEST (move_right) {
    Screen s = screen_init(&test_arguments);

    /* at the end of the last line *******************************************/

    /* initial state */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    handle_move_right(s);

    /* state has not changed */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    /* middle of the line ****************************************************/

    handle_insert_char(s, 'a');
    handle_insert_char(s, 'b');
    handle_insert_char(s, 'c');
    handle_move_left(s);

    /* state before move */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(2, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(3, CURR_LINE->visual_end);
    ck_assert_int_eq(2, CURR_LBUF->cursor);
    ck_assert_int_eq('b', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);

    handle_move_right(s);

    /* state after move */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(3, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(3, CURR_LINE->visual_end);
    ck_assert_int_eq(3, CURR_LBUF->cursor);
    ck_assert_int_eq('c', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);

    /* middle of the line on tab *********************************************/

    /* insert a tab */
    handle_tab(s);
    handle_move_left(s);

    /* state before move */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(3, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(7, CURR_LINE->visual_end);
    ck_assert_int_eq(3, CURR_LBUF->cursor);
    ck_assert_int_eq('c', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);

    handle_move_right(s);

    /* state after move */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(7, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(7, CURR_LINE->visual_end);
    ck_assert_int_eq(4, CURR_LBUF->cursor);
    ck_assert_int_eq('\t', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);

    /* end of the other line *************************************************/

    /* adjust to testing position */
    handle_move_right(s);
    handle_move_right(s);
    handle_enter(s);
    handle_move_left(s);

    /* state before move */
    ck_assert_int_eq(2, s->n_lines);
    ck_assert_int_eq(7, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq(7, CURR_LINE->visual_end);
    ck_assert_int_eq(4, CURR_LBUF->cursor);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[9]);

    handle_move_right(s);

    /* state after move, at the beginning of the next line */
    ck_assert_int_eq(2, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(1, s->row);
    ck_assert_int_eq(0, CURR_LINE->visual_end);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);

    screen_destroy(s);
} END_TEST

START_TEST (move_up) {
    Screen s = screen_init(&test_arguments);

    /* at the first line *****************************************************/

    /* initial state */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    handle_move_up(s);

    /* nothing has changed */
    ck_assert_int_eq(1, s->n_lines);
    ck_assert_int_eq(0, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq('\n', CURR_LBUF->buffer[0]);
    ck_assert_int_eq(0, CURR_LBUF->cursor);
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    /* cursor position bigger than length of the previous line ***************/

    /* prepare for testing */
    handle_insert_char(s, 'a');
    handle_insert_char(s, 'b');
    handle_enter(s);
    handle_insert_char(s, 'c');
    handle_insert_char(s, 'd');
    handle_insert_char(s, 'e');
    handle_insert_char(s, 'f');

    /* state before move */
    ck_assert_int_eq(2, s->n_lines);
    ck_assert_int_eq(4, s->col);
    ck_assert_int_eq(1, s->row);
    ck_assert_int_eq('f', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);
    ck_assert_int_eq(4, CURR_LBUF->cursor);
    ck_assert_int_eq(4, CURR_LINE->visual_end);

    handle_move_up(s);

    /* state after move, at the end of the previous line */
    ck_assert_int_eq(2, s->n_lines);
    ck_assert_int_eq(2, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq('b', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);
    ck_assert_int_eq(2, CURR_LBUF->cursor);
    ck_assert_int_eq(2, CURR_LINE->visual_end);

    /* cursor position smaller than length of the previous line **************/

    /* prepare for testing, line is longer than the next one */
    handle_insert_char(s, '1');
    handle_insert_char(s, '2');
    handle_insert_char(s, '3');
    handle_insert_char(s, '4');
    handle_move_down(s);

    /* state before move */
    ck_assert_int_eq(2, s->n_lines);
    ck_assert_int_eq(4, s->col);
    ck_assert_int_eq(1, s->row);
    ck_assert_int_eq('f', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);
    ck_assert_int_eq(4, CURR_LBUF->cursor);
    ck_assert_int_eq(4, CURR_LINE->visual_end);

    handle_move_up(s);

    /* state after move, at the same position on the upper line */
    ck_assert_int_eq(2, s->n_lines);
    ck_assert_int_eq(4, s->col);
    ck_assert_int_eq(0, s->row);
    ck_assert_int_eq('2', CURR_LBUF->buffer[CURR_LBUF->cursor-1]);
    ck_assert_int_eq(4, CURR_LBUF->cursor);
    ck_assert_int_eq(6, CURR_LINE->visual_end);

    screen_destroy(s);
} END_TEST

Suite* s_input() {
    Suite* s_input = suite_create("input");

    TCase* tc_movement = tcase_create("movement");
    tcase_add_test(tc_movement, letter_insertion);
    tcase_add_test(tc_movement, move_left);
    tcase_add_test(tc_movement, move_right);
    tcase_add_test(tc_movement, move_up);
    suite_add_tcase(s_input, tc_movement);

    return s_input;
}

int main() {
    Suite* s_screen_s = s_screen();
    Suite* s_input_s = s_input();

    SRunner* s_logic_runner = srunner_create(s_screen_s);
    srunner_add_suite(s_logic_runner, s_input_s);

    test_arguments.debug_mode = false;
    test_arguments.file_name = "-";

    srunner_run_all(s_logic_runner, CK_NORMAL);
    int number_failed = srunner_ntests_failed(s_logic_runner);

    srunner_free(s_logic_runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
