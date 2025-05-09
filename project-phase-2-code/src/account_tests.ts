#suite account_tests

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "account.h"

/* Test helper to create test accounts */
static account_t* create_test_account(void) {
    account_t* acc = account_create("testuser", "TestP@ss123", "test@example.com", "1990-01-01");
    if (acc == NULL) {
        ck_abort_msg("Failed to create test account");
    }
    return acc;
}

/*
 * Tests for account_validate_password
 */
#tcase password_validation_tests

#test valid_password_succeeds
    account_t* acc = create_test_account();
    
    /* Test with correct password */
    ck_assert_msg(account_validate_password(acc, "TestP@ss123"), 
                 "Valid password should be accepted");
    
    account_free(acc);

#test invalid_password_fails
    account_t* acc = create_test_account();
    
    /* Test with incorrect password */
    ck_assert_msg(!account_validate_password(acc, "WrongP@ss123"), 
                 "Invalid password should be rejected");
    
    account_free(acc);

#test null_parameters_fail
    account_t* acc = create_test_account();
    
    /* Test with NULL parameters */
    ck_assert_msg(!account_validate_password(NULL, "TestP@ss123"), 
                 "NULL account should be rejected");
    ck_assert_msg(!account_validate_password(acc, NULL), 
                 "NULL password should be rejected");
    
    account_free(acc);

#test banned_account_access_denied
    account_t* acc = create_test_account();
    
    /* Set a ban time in the future */
    time_t future_time = time(NULL) + 3600; /* One hour in the future */
    account_set_unban_time(acc, future_time);
    
    /* Banned account should not validate */
    ck_assert_msg(!account_validate_password(acc, "TestP@ss123"), 
                 "Password validation should fail for banned account");
    
    account_free(acc);

#test expired_account_access_denied
    account_t* acc = create_test_account();
    
    /* Set expiration time in the past */
    time_t past_time = time(NULL) - 3600; /* One hour in the past */
    account_set_expiration_time(acc, past_time);
    
    /* Expired account should not validate */
    ck_assert_msg(!account_validate_password(acc, "TestP@ss123"), 
                 "Password validation should fail for expired account");
    
    account_free(acc);

#test excessive_failures_prevent_validation
    account_t* acc = create_test_account();
    
    /* Simulate 10 failed login attempts */
    for (int i = 0; i < 10; i++) {
        account_record_login_failure(acc);
    }
    
    /* Account should be rate-limited */
    ck_assert_msg(!account_validate_password(acc, "TestP@ss123"), 
                 "Password validation should fail after excessive failures");
    
    account_free(acc);

/*
 * Tests for account_update_password
 */
#tcase password_update_tests

#test update_valid_password
    account_t* acc = create_test_account();
    
    /* Update to a new password */
    ck_assert_msg(account_update_password(acc, "NewP@ssw0rd!"), 
                 "Valid password update should succeed");
    
    /* Verify new password works */
    ck_assert_msg(account_validate_password(acc, "NewP@ssw0rd!"), 
                 "New password should validate");
    
    /* Verify old password doesn't work */
    ck_assert_msg(!account_validate_password(acc, "TestP@ss123"), 
                 "Old password should no longer validate");
    
    account_free(acc);

#test update_resets_failure_count
    account_t* acc = create_test_account();
    
    /* Record some failed login attempts */
    for (int i = 0; i < 5; i++) {
        account_record_login_failure(acc);
    }
    
    /* Verify failure count is 5 */
    ck_assert_int_eq(acc->login_fail_count, 5);
    
    /* Update password */
    ck_assert(account_update_password(acc, "NewP@ssw0rd!"));
    
    /* Verify failure count is reset to 0 */
    ck_assert_int_eq(acc->login_fail_count, 0);
    
    account_free(acc);

#test update_rejects_weak_password
    account_t* acc = create_test_account();
    
    /* Test with too short password */
    ck_assert_msg(!account_update_password(acc, "Sh0rt!"), 
                 "Too short password should be rejected");
    
    /* Test with password missing diversity */
    ck_assert_msg(!account_update_password(acc, "onlylowercase"), 
                 "Password with only lowercase should be rejected");
    ck_assert_msg(!account_update_password(acc, "12345678"), 
                 "Password with only digits should be rejected");
    ck_assert_msg(!account_update_password(acc, "ALLUPPERCASE"), 
                 "Password with only uppercase should be rejected");
    
    account_free(acc);

#test update_rejects_null_parameters
    account_t* acc = create_test_account();
    
    /* Test with NULL parameters */
    ck_assert_msg(!account_update_password(NULL, "ValidP@ss123"), 
                 "NULL account should be rejected");
    ck_assert_msg(!account_update_password(acc, NULL), 
                 "NULL password should be rejected");
    
    account_free(acc);

#test multiple_updates_work
    account_t* acc = create_test_account();
    
    /* Multiple password changes should work */
    ck_assert(account_update_password(acc, "Passw0rd!1"));
    ck_assert(account_validate_password(acc, "Passw0rd!1"));
    
    ck_assert(account_update_password(acc, "Passw0rd!2"));
    ck_assert(account_validate_password(acc, "Passw0rd!2"));
    
    ck_assert(account_update_password(acc, "Passw0rd!3"));
    ck_assert(account_validate_password(acc, "Passw0rd!3"));
    
    /* Only the most recent password should work */
    ck_assert(!account_validate_password(acc, "Passw0rd!1"));
    ck_assert(!account_validate_password(acc, "Passw0rd!2"));
    
    account_free(acc);

/*
 * Tests for account_is_banned
 */
#tcase account_banned_tests

#test fresh_account_not_banned
    account_t* acc = create_test_account();
    
    /* Account should not be banned initially */
    ck_assert_msg(!account_is_banned(acc), 
                 "Fresh account should not be banned");
    
    account_free(acc);

#test account_banned_with_future_unban_time
    account_t* acc = create_test_account();
    
    /* Set a ban time in the future */
    time_t current_time = time(NULL);
    time_t future_time = current_time + 3600; /* One hour in the future */
    account_set_unban_time(acc, future_time);
    
    /* Account should now be banned */
    ck_assert_msg(account_is_banned(acc), 
                 "Account with future unban_time should be banned");
    
    account_free(acc);

#test account_not_banned_with_past_unban_time
    account_t* acc = create_test_account();
    
    /* Set a ban time in the past */
    time_t current_time = time(NULL);
    time_t past_time = current_time - 3600; /* One hour in the past */
    account_set_unban_time(acc, past_time);
    
    /* Account should no longer be banned */
    ck_assert_msg(!account_is_banned(acc), 
                 "Account with past unban_time should not be banned");
    
    account_free(acc);

#test account_with_zero_unban_time_not_banned
    account_t* acc = create_test_account();
    
    /* Set unban_time to 0 (no ban) */
    account_set_unban_time(acc, 0);
    
    /* Account should not be banned */
    ck_assert_msg(!account_is_banned(acc), 
                 "Account with unban_time=0 should not be banned");
    
    account_free(acc);

#test null_account_is_banned_returns_false
    /* Test with NULL parameter */
    ck_assert_msg(!account_is_banned(NULL), 
                 "NULL account should return false for is_banned");

/*
 * Tests for account_is_expired
 */
#tcase account_expired_tests

#test fresh_account_not_expired
    account_t* acc = create_test_account();
    
    /* Account should not be expired initially */
    ck_assert_msg(!account_is_expired(acc), 
                 "Fresh account should not be expired");
    
    account_free(acc);

#test account_not_expired_with_future_time
    account_t* acc = create_test_account();
    
    /* Set an expiration time in the future */
    time_t current_time = time(NULL);
    time_t future_time = current_time + 3600; /* One hour in the future */
    account_set_expiration_time(acc, future_time);
    
    /* Account should not be expired yet */
    ck_assert_msg(!account_is_expired(acc), 
                 "Account with future expiration_time should not be expired");
    
    account_free(acc);

#test account_expired_with_past_time
    account_t* acc = create_test_account();
    
    /* Set an expiration time in the past */
    time_t current_time = time(NULL);
    time_t past_time = current_time - 3600; /* One hour in the past */
    account_set_expiration_time(acc, past_time);
    
    /* Account should now be expired */
    ck_assert_msg(account_is_expired(acc), 
                 "Account with past expiration_time should be expired");
    
    account_free(acc);

#test account_with_zero_expiration_time_not_expired
    account_t* acc = create_test_account();
    
    /* Set expiration_time to 0 (never expires) */
    account_set_expiration_time(acc, 0);
    
    /* Account should not be expired */
    ck_assert_msg(!account_is_expired(acc), 
                 "Account with expiration_time=0 should not be expired");
    
    account_free(acc);

#test null_account_is_expired_returns_false
    /* Test with NULL parameter */
    ck_assert_msg(!account_is_expired(NULL), 
                 "NULL account should return false for is_expired");

#tcase account_interaction_tests

#test banned_account_still_expired
    account_t* acc = create_test_account();
    
    /* Set both ban and expiration */
    time_t current_time = time(NULL);
    time_t future_ban = current_time + 7200; /* Two hours in the future */
    time_t past_expiration = current_time - 3600; /* One hour in the past */
    
    account_set_unban_time(acc, future_ban);
    account_set_expiration_time(acc, past_expiration);
    
    /* Account should be both banned and expired */
    ck_assert(account_is_banned(acc));
    ck_assert(account_is_expired(acc));
    
    account_free(acc);
