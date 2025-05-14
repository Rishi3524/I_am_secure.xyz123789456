#include "test_account.h"
#include "../src/account.h"
#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

/* Helper function to create a test account */
static account_t* create_test_account(void) {
    account_t* acc = account_create("testuser", "TestP@ss123", "test@example.com", "1990-01-01");
    if (acc == NULL) {
        ck_abort_msg("Failed to create test account");
    }
    return acc;
}

START_TEST(test_account_create) {
    // Test cases will go here
    struct {
        const char *testname;
        const char *userid;
        const char *password;
        const char *email;
        const char *birthdate;
        bool expectedoutcome;
    } 
    cases[] = {
        {"Valid user", "username", "StrongPassword0!!", "valid@user.com", "2005-04-05", true},
        {"Invalid email", "username", "StrongPassword0!!", "bad@ email.com", "2005-04-05", false},
        {"Weak password", "weakpassuser", "password", "user@example.com", "2005-04-05", false},
        {"Invalid birthdate", "baddate", "StrongPassword0!!", "user@example.com", "2005-02-31", false, "2023-02-31", false},
        {"Long email", "longemail", "StrongPassword0!!", NULL, "2005-04-05", false},
        {"Space birthday", "spacebirthday", "StrongPassword0!!",  "user@example.com", "    -  -  ", false},
        {"Spaces userid", "     ", "StrongPassword0!!", "user@example.com", "2000-04-25", false},
    };

    char emailbuffer[EMAIL_LENGTH + 20];
    for (int i = 0; i <sizeof(cases)/sizeof(cases[0]); i++) {
        const char *email = cases[i].email;
        if ((email == NULL) && strcmp(cases[i].testname, "Long email") == 0) {
            memset(emailbuffer, 'a', EMAIL_LENGTH+5);
            memcpy(emailbuffer+5+EMAIL_LENGTH, "@m.com", 6);
            emailbuffer[EMAIL_LENGTH + 11] = '\0';
            email = emailbuffer;
        }

        account_t *acc = account_create(cases[i].userid, cases[i].password, email, cases[i].birthdate);
        bool success;
        if ((acc != NULL && cases[i].expectedoutcome ==true) || (acc == NULL && cases[i].expectedoutcome == false)) {
            ck_assert_msg("[%s] passed", cases[i].testname);
        } else if ((acc != NULL&&cases[i].expectedoutcome == false)) {
            ck_abort_msg("[%s] expected no account made but one was", cases[i].testname);
        } else {
            ck_abort_msg("[%s] expected an account to be made but recieved NULL", cases[i].testname);
        }
        if (acc) account_free(acc);
    }
} END_TEST

START_TEST(test_account_free) {
    account_t *acc = account_create("usertobefreed", "StrongPassword0!!", "freeme@example.com", "2005-04-05");
    ck_assert_ptr_nonnull(acc);
    account_free(acc);
    account_free(NULL);
} END_TEST

START_TEST(test_account_validate_password) {
    account_t* acc = create_test_account();
    
    /* Test correct password */
    ck_assert(account_validate_password(acc, "TestP@ss123"));
    
    /* Test incorrect password */
    ck_assert(!account_validate_password(acc, "WrongP@ss123"));
    
    /* Test with NULL parameters */
    ck_assert(!account_validate_password(NULL, "TestP@ss123"));
    ck_assert(!account_validate_password(acc, NULL));
    
    /* Test with empty password */
    ck_assert(!account_validate_password(acc, ""));
    
    /* Test with account states */
    time_t current_time = time(NULL);
    
    /* Test banned account */
    account_set_unban_time(acc, current_time + 3600);
    ck_assert(!account_validate_password(acc, "TestP@ss123"));
    account_set_unban_time(acc, 0);
    
    /* Test expired account */
    account_set_expiration_time(acc, current_time - 3600);
    ck_assert(!account_validate_password(acc, "TestP@ss123"));
    account_set_expiration_time(acc, 0);
    
    /* Test rate limiting */
    for (int i = 0; i < 6; i++) {
        account_record_login_failure(acc);
    }
    ck_assert(!account_validate_password(acc, "TestP@ss123"));
    
    account_free(acc);
} END_TEST

START_TEST(test_account_update_password) {
    account_t* acc = create_test_account();
    
    /* Test basic password update */
    ck_assert(account_update_password(acc, "NewP@ssw0rd!"));
    ck_assert(account_validate_password(acc, "NewP@ssw0rd!"));
    ck_assert(!account_validate_password(acc, "TestP@ss123"));
    
    /* Test with NULL parameters */
    ck_assert(!account_update_password(NULL, "NewP@ssw0rd!"));
    ck_assert(!account_update_password(acc, NULL));
    
    /* Test with empty password */
    ck_assert(!account_update_password(acc, ""));
    
    /* Test password complexity requirements */
    ck_assert(!account_update_password(acc, "short"));  /* Too short */
    ck_assert(!account_update_password(acc, "onlylowercase")); /* No diversity */
    ck_assert(!account_update_password(acc, "ALLUPPERCASE")); /* No diversity */
    ck_assert(!account_update_password(acc, "12345678901")); /* No diversity */
    ck_assert(account_update_password(acc, "Password123")); /* Acceptable */
    
    /* Test reset of security measures */
    /* First set up a banned state with failures */
    for (int i = 0; i < 5; i++) {
        account_record_login_failure(acc);
    }
    time_t current_time = time(NULL);
    account_set_unban_time(acc, current_time + 3600);
    
    ck_assert_int_eq(acc->login_fail_count, 5);
    ck_assert(account_is_banned(acc));
    
    /* Update password should reset both */
    ck_assert(account_update_password(acc, "AnotherP@ss123"));
    
    /* Verify both are reset */
    ck_assert_int_eq(acc->login_fail_count, 0);
    ck_assert(!account_is_banned(acc));
    
    account_free(acc);
} END_TEST

START_TEST(test_account_record_login) {
    account_t acc = {0};
    ip4_addr_t ip = 0x7F000001;  /* 127.0.0.1 */

    /* Initial state */
    ck_assert_uint_eq(acc.login_count,      0);
    ck_assert_uint_eq(acc.login_fail_count, 0);

    /* One failure */
    account_record_login_failure(&acc);
    ck_assert_uint_eq(acc.login_fail_count, 1);
    ck_assert_uint_eq(acc.login_count,      0);

    /* Then success */
    account_record_login_success(&acc, ip);
    ck_assert_uint_eq(acc.login_count,      1);
    ck_assert_uint_eq(acc.login_fail_count, 0);
    ck_assert_uint_eq(acc.last_ip,          ip);
    ck_assert_uint_eq(acc.last_login_time,  (time_t)1600000000);

    /* NULL-pointer safety */
    account_record_login_failure(NULL);
    account_record_login_success(NULL, ip);
    // Test cases will go here
} END_TEST

START_TEST(test_account_ban_expiry) {
    account_t* acc = create_test_account();
    time_t current_time = time(NULL);
    
    /* Test account_is_banned */
    
    /* Test fresh account not banned */
    ck_assert(!account_is_banned(acc));
    
    /* Test ban with future time */
    account_set_unban_time(acc, current_time + 3600);
    ck_assert(account_is_banned(acc));
    
    /* Test ban with past time */
    account_set_unban_time(acc, current_time - 3600);
    ck_assert(!account_is_banned(acc));
    
    /* Test ban with zero time */
    account_set_unban_time(acc, 0);
    ck_assert(!account_is_banned(acc));
    
    /* Test ban with NULL account */
    ck_assert(!account_is_banned(NULL));
    
    /* Test account_is_expired */
    
    /* Test fresh account not expired */
    ck_assert(!account_is_expired(acc));
    
    /* Test expiry with future time */
    account_set_expiration_time(acc, current_time + 3600);
    ck_assert(!account_is_expired(acc));
    
    /* Test expiry with past time */
    account_set_expiration_time(acc, current_time - 3600);
    ck_assert(account_is_expired(acc));
    
    /* Test expiry with current time (boundary case) */
    account_set_expiration_time(acc, current_time);
    ck_assert(account_is_expired(acc));
    
    /* Test expiry with zero time */
    account_set_expiration_time(acc, 0);
    ck_assert(!account_is_expired(acc));
    
    /* Test expiry with NULL account */
    ck_assert(!account_is_expired(NULL));
    
    /* Test both states together */
    account_set_unban_time(acc, current_time + 3600);  /* Banned */
    account_set_expiration_time(acc, current_time - 3600);  /* Expired */
    ck_assert(account_is_banned(acc));
    ck_assert(account_is_expired(acc));
    ck_assert(!account_validate_password(acc, "TestP@ss123"));
    
    account_free(acc);
} END_TEST

START_TEST(test_account_set_times) {
    account_t acc = {0};
    time_t now = time(NULL);

    account_set_unban_time(&acc, now + 60);
    ck_assert_int_eq(acc.unban_time, now + 60);

    account_set_expiration_time(&acc, now + 120);
    ck_assert_int_eq(acc.expiration_time, now + 120);

    // NULL pointer safety
    account_set_unban_time(NULL, now);
    account_set_expiration_time(NULL, now);
} END_TEST

START_TEST(test_account_print) {
    const char *test_userid = "printsummary";
    const char *test_file = "printsummarytest.txt";
    account_t *acc = account_create(test_userid, "StrongPassword0!!", "print@summary.com", "2005-04-05");
    ck_assert_ptr_nonnull(acc);

    int fd = open(test_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    ck_assert_msg(fd >= 0, "Failed to open output file");
    bool result = account_print_summary(acc, fd);
    ck_assert_msg(result, "account_print_summary returned false");
    FILE *fp = fopen(test_file, "r");
    ck_assert_ptr_nonnull(fp);

    char line[512];
    bool includesuserid = false;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, test_userid)) {
            includesuserid = true;
            break;
        }
    }
    fclose(fp);
    remove(test_file);
    account_free(acc);
    ck_assert_msg(includesuserid, "Summary output did not contain user ID");
} END_TEST

TCase* make_account_tests(void) {
    TCase *tc = tcase_create("Account Tests");
    
    tcase_add_test(tc, test_account_create);
    tcase_add_test(tc, test_account_free);
    tcase_add_test(tc, test_account_validate_password);
    tcase_add_test(tc, test_account_update_password);
    tcase_add_test(tc, test_account_record_login);
    tcase_add_test(tc, test_account_ban_expiry);
    tcase_add_test(tc, test_account_set_times);
    tcase_add_test(tc, test_account_print);
    
    return tc;
}
    tcase_add_test(tc, test_account_print);
    
    return tc;
}
