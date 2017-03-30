#include <stdio.h>
#include <check.h>
#include <stdlib.h>

#include "lib/gap_buffer.h"

START_TEST (test_one_line_insertion) {

} END_TEST

Suite* s_buffer(void) {
    Suite* s_buffer = suite_create("buffer");
    TCase* tc_one_line = tcase_create("one_line");

    tcase_add_test(tc_one_line, test_one_line_insertion);
    suite_add_tcase(s_buffer, tc_one_line);

    return s_buffer;
}

int main() {
    Suite* s_buff = s_buffer();
    SRunner* s_buff_r = srunner_create(s_buff);

    srunner_run_all(s_buff_r, CK_NORMAL);
    int number_failed = srunner_ntests_failed(s_buff_r);

    srunner_free(s_buff_r);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
