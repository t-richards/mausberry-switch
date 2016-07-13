#include <stdlib.h>
#include <check.h>

START_TEST (test_error_on_no_config_file)
{
    int exitstatus = system("../src/mausberry-switch");
    ck_assert_int_eq(WEXITSTATUS(exitstatus), 1);
}
END_TEST

Suite * maus_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Maus");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_error_on_no_config_file);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = maus_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
