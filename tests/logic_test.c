#include <stdlib.h>

#include <check.h>
#include <glib-2.0/glib.h>

#include "lib/gap_buffer.h"
#include "screen.h"

/* Arguments structure for testing */
struct Arguments test_arguments;

/* test screen initialization */
START_TEST (test_screen_init) {
    Screen s = screen_init(&test_arguments);

    /* test list of the lines ************************************************/

    /* there is a line in the list */
    ck_assert_ptr_ne(NULL, s->lines);

    /* current (first) line is first in the list */
    ck_assert_ptr_eq(s->lines, s->cur_line);

    /* test the first line ***************************************************/

    /* visual end of the line is 0 */
    ck_assert_int_eq(0, ((Line)s->cur_line->data)->visual_end);

    /* temporary pointer to the first line's buffer */
    gap_T first_buff = ((Line)s->cur_line->data)->buff;

    /* first line's buffer is properly allocated */
    ck_assert_ptr_ne(NULL, first_buff);

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

Suite* s_general(void) {
    Suite* s_general = suite_create("general");
    TCase* tc_init = tcase_create("initialization");

    tcase_add_test(tc_init, test_screen_init);
    suite_add_tcase(s_general, tc_init);

    return s_general;
}

int main() {
    Suite* s_general_s = s_general();
    SRunner* s_general_r = srunner_create(s_general_s);

    test_arguments.debug_mode = false;
    test_arguments.file_name = "-";

    srunner_run_all(s_general_r, CK_NORMAL);
    int number_failed = srunner_ntests_failed(s_general_r);

    srunner_free(s_general_r);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
