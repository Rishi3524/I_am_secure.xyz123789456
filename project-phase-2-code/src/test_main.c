/* src/test_main.c */
#include <stdlib.h>
#include <check.h>

/* Test suite is generated from account_tests.ts by checkmk */
Suite* account_tests_suite(void);

int main(void) {
    SRunner *sr = srunner_create(account_tests_suite());
    
    /* Run the tests */
    srunner_run_all(sr, CK_VERBOSE);
    
    /* Get the number of failed tests */
    int number_failed = srunner_ntests_failed(sr);
    
    /* Free the resources used by the test runner */
    srunner_free(sr);
    
    /* Return SUCCESS if all tests passed, FAILURE otherwise */
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
