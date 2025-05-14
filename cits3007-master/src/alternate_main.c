#ifdef ALTERNATE_MAIN

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "db.h"
#include "login.h"
#include <time.h>
#include <unistd.h>

// Global counters
static int lookup_tests_total = 0;
static int lookup_tests_passed = 0;

static int login_tests_total = 0;
static int login_tests_passed = 0;

// Helper to run and log test outcome
void run_test(const char *name, const char *userid, int expected) {
    account_t acc = {0};
    bool result = account_lookup_by_userid(userid, &acc);
    printf("[%s] ", name);

    lookup_tests_total++;
    if (result == expected) {
        lookup_tests_passed++;
        printf("PASS");
        if (result) {
            printf(" | Found: %s (%s)", acc.userid, acc.email);
        }
        printf("\n");
    } else {
        printf("FAIL | Expected %d, got %d\n", expected, result);
    }
}

// account_lookup_by_userid() Tests

void test_valid_user() {
    run_test("Valid user", "cappuccinoassassino", 1);
}

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
    tricky_userid[20] = '\0';
    run_test("Embedded null", tricky_userid, 1);
}

void test_very_long_userid() {
    char long_id[256];
    memset(long_id, 'a', sizeof(long_id));
    long_id[255] = '\0';
    run_test("Overly long userid", long_id, 0);
}

// handle_login() Tests

void test_login_success() {
    login_session_data_t session = {0};
    ip4_addr_t ip = 0x01020304;
    time_t now = time(NULL);
    login_tests_total++;

    login_result_t result = handle_login("cappuccinoassassino", "password123",
                                         ip, now, STDOUT_FILENO, STDERR_FILENO, &session);
    if (result == LOGIN_SUCCESS) {
        login_tests_passed++;
        printf("PASS: test_login_success\n");
    } else {
        printf("FAIL: test_login_success (code %d)\n", result);
    }
}

void test_user_not_found() {
    login_session_data_t session = {0};
    ip4_addr_t ip = 0x01020304;
    time_t now = time(NULL);
    login_tests_total++;

    login_result_t result = handle_login("not_a_user", "password123",
                                         ip, now, STDOUT_FILENO, STDERR_FILENO, &session);
    if (result == LOGIN_FAIL_USER_NOT_FOUND) {
        login_tests_passed++;
        printf("PASS: test_user_not_found\n");
    } else {
        printf("FAIL: test_user_not_found (code %d)\n", result);
    }
}

void test_handle_login_null_input() {
    login_session_data_t session = {0};
    ip4_addr_t ip = 0x01020304;
    time_t now = time(NULL);

    login_tests_total++;
    login_result_t result = handle_login(NULL, "password123", ip, now, STDOUT_FILENO, STDERR_FILENO, &session);
    if (result == LOGIN_FAIL_INTERNAL_ERROR) {
        login_tests_passed++;
        printf("PASS: test_null_input (username)\n");
    } else {
        printf("FAIL: test_null_input (username)\n");
    }

    login_tests_total++;
    result = handle_login("cappuccinoassassino", NULL, ip, now, STDOUT_FILENO, STDERR_FILENO, &session);
    if (result == LOGIN_FAIL_INTERNAL_ERROR) {
        login_tests_passed++;
        printf("PASS: test_null_input (password)\n");
    } else {
        printf("FAIL: test_null_input (password)\n");
    }

    login_tests_total++;
    result = handle_login("cappuccinoassassino", "password123", ip, now, STDOUT_FILENO, STDERR_FILENO, NULL);
    if (result == LOGIN_FAIL_INTERNAL_ERROR) {
        login_tests_passed++;
        printf("PASS: test_null_input (session)\n");
    } else {
        printf("FAIL: test_null_input (session)\n");
    }
}

void test_wrong_password() {
    login_session_data_t session = {0};
    ip4_addr_t ip = 0x01020304;
    time_t now = time(NULL);
    login_tests_total++;

    login_result_t result = handle_login("cappuccinoassassino", "wrongpass",
                                         ip, now, STDOUT_FILENO, STDERR_FILENO, &session);
    if (result == LOGIN_FAIL_BAD_PASSWORD) {
        login_tests_passed++;
        printf("PASS: test_wrong_password\n");
    } else {
        printf("FAIL: test_wrong_password (code %d)\n", result);
    }
}

// Main Runner

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

    printf("\n=== handle_login() Tests ===\n");
    test_login_success();
    test_user_not_found();
    test_handle_login_null_input();
    test_wrong_password();

    printf("\n=== Test Summary ===\n");
    printf("Lookup Tests: Passed %d/%d\n", lookup_tests_passed, lookup_tests_total);
    printf("Login Tests:  Passed %d/%d\n", login_tests_passed, login_tests_total);

    int total_tests = lookup_tests_total + login_tests_total;
    int total_passed = lookup_tests_passed + login_tests_passed;
    printf("Total:        Passed %d/%d\n", total_passed, total_tests);

    return (total_tests == total_passed) ? 0 : 1;
}

#endif