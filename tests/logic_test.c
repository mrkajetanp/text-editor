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

    /* only char in the buffer is '\n' */
    ck_assert_int_eq('\n', l->buff->buffer[0]);

    /* line's cursor starts at 0 */
    ck_assert_int_eq(0, l->buff->cursor);

    /* visual end of the line is 0 */
    ck_assert_int_eq(0, l->visual_end);

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

    /* visual end of the line is 0 */
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    /* first line's buffer is properly allocated */
    ck_assert_ptr_nonnull(CURR_LBUF);

    /* first line's actual cursor starts at 0 */
    ck_assert_int_eq(0, CURR_LBUF->cursor);

    /* initially only char in the line is '\n' */
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
    ck_assert_int_eq(0, CURR_LINE->visual_end);

    /* actual insertion ******************************************************/

    handle_insert_char(s, 'a');

    /* inserted char is at the beginning of the buffer */
    ck_assert_int_eq('a', CURR_LBUF->buffer[0]);

    /* '\n' is the next char after the gap */
    ck_assert_int_eq('\n', CURR_LBUF->buffer[9]);

    /* buffer's cursor is incremented */
    ck_assert_int_eq(1, CURR_LBUF->cursor);

    /* line's visual end is incremented */
    ck_assert_int_eq(1, CURR_LINE->visual_end);

    screen_destroy(s);
} END_TEST

Suite* s_input() {
    Suite* s_input = suite_create("input");

    TCase* tc_movement = tcase_create("movement");
    tcase_add_test(tc_movement, letter_insertion);
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
