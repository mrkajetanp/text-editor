#include <stdlib.h>

#include <check.h>
#include <glib-2.0/glib.h>

#include "lib/gap_buffer.h"
#include "screen.h"

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
    ck_assert_int_eq(0, ((Line)s->cur_line->data)->visual_end);

    /* temporary pointer to the first line's buffer */
    gap_T first_buff = ((Line)s->cur_line->data)->buff;

    /* first line's buffer is properly allocated */
    ck_assert_ptr_nonnull(first_buff);

    /* first line's actual cursor starts at 0 */
    ck_assert_int_eq(0, first_buff->cursor);

    /* initially only char in the line is '\n' */
    ck_assert_int_eq('\n', first_buff->buffer[0]);

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
    Line old_curr = s->cur_line->data;
    int old_n_lines = s->n_lines;

    screen_new_line_under(s);

    ck_assert_ptr_nonnull(s->cur_line);

    /* cur_line is after the old cur_line */
    ck_assert_ptr_eq(old_curr, s->cur_line->prev->data);

    /* s->n_lines was properly incremented */
    ck_assert_int_eq(old_n_lines, s->n_lines-1);

    screen_destroy(s);
} END_TEST

START_TEST (test_new_line_above) {
    Screen s = screen_init(&test_arguments);

    /* store the previous state */
    Line old_curr = s->cur_line->data;
    int old_n_lines = s->n_lines;

    screen_new_line_above(s);

    ck_assert_ptr_nonnull(s->cur_line);

    /* cur_line did not change */
    ck_assert_ptr_eq(old_curr, s->cur_line->data);

    /* there is a line above the current one */
    ck_assert_ptr_nonnull(s->cur_line->prev);

    /* s->n_lines was properly incremented */
    ck_assert_int_eq(old_n_lines, s->n_lines-1);

    screen_destroy(s);
} END_TEST

Suite* s_screen(void) {
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

int main() {
    Suite* s_screen_s = s_screen();
    SRunner* s_screen_r = srunner_create(s_screen_s);

    test_arguments.debug_mode = false;
    test_arguments.file_name = "-";

    srunner_run_all(s_screen_r, CK_NORMAL);
    int number_failed = srunner_ntests_failed(s_screen_r);

    srunner_free(s_screen_r);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
