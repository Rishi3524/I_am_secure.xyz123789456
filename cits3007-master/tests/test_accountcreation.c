#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "account.h"

// Test result counters
static int total_tests = 0;
static int total_passed = 0;

// Helper to run and log test outcome
void run_accountcreate_test(const char *testname, const char *userid,
    const char *ptpassword, const char *email, const char *birthdate, bool expected) {
    printf("[%s] ", testname);

    total_tests++;
    account_t *newaccount = account_create(userid, password, email, birthdate);
    if (((newaccount == NULL) && (expected == false)) || ((newaccount != NULL)&&(expected==true)){
        total_passed++;
        printf("PASS\n");
    } else {
        printf("FAIL | Expected %d, got %d\n", expected, newaccount);
    }
    if (newaccount) {
        account_free(newaccount);
    }
}

// Individual test functions
void test_valid_user() {
    run_test("Valid user", "cappuccinoassassino", 1);
}
#endif

void test_nonexistent_user() {
    run_test("Nonexistent user", "ballerinacappuccina", 0);
}

void test_empty_string() {
    run_test("Empty string", "", 0);
}

void test_null_input() {
    run_test("NULL input", NULL, 0);
}

void test_case_sensitive() {
    run_test("Case sensitivity", "CappuccinoAssassino", 0);
}

void test_trailing_spaces() {
    run_test("Trailing spaces", "cappuccinoassassino   ", 0);
}

void test_embedded_null() {
    char tricky_userid[64] = "cappuccinoassassino";
    tricky_userid[20] = '\0';  // Properly null-terminated after full name
    run_test("Embedded null", tricky_userid, 1); 
}

void test_very_long_userid() {
    char long_id[256];
    memset(long_id, 'a', sizeof(long_id));
    long_id[255] = '\0';
    run_test("Overly long userid", long_id, 0);
}

// === Main Test Runner ===
int main() {
    printf("=== account_lookup_by_userid() Tests ===\n\n");

    test_valid_user();
    test_nonexistent_user();
    test_empty_string();
    test_null_input();
    test_case_sensitive();
    test_trailing_spaces();
    test_embedded_null();
    test_very_long_userid();

    printf("\n=== Test Summary ===\n");
    printf("Passed %d/%d tests.\n", total_passed, total_tests);

    return (total_tests == total_passed) ? 0 : 1;
}