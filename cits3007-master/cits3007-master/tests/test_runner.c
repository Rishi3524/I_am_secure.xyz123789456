#include <stdlib.h>
#include <check.h>
#include "test_account.h"
#include "test_login.h"
#include "test_db.h"

int main(void) {
    int number_failed;
    Suite *s = suite_create("Oblivionaire Online Tests");
    
    // Add test suites here
    suite_add_tcase(s, make_account_tests());
    suite_add_tcase(s, make_login_tests());
    suite_add_tcase(s, make_db_tests());
    
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
} 