#include "test_db.h"
#include "../src/db.h"
#include <check.h>

START_TEST(test_account_lookup_found) {
    // Test cases will go here
} END_TEST

START_TEST(test_account_lookup_not_found) {
    // Test cases will go here
} END_TEST

START_TEST(test_account_lookup_invalid) {
    // Test cases will go here
} END_TEST

TCase* make_db_tests(void) {
    TCase *tc = tcase_create("Database Tests");
    
    tcase_add_test(tc, test_account_lookup_found);
    tcase_add_test(tc, test_account_lookup_not_found);
    tcase_add_test(tc, test_account_lookup_invalid);
    
    return tc;
} 