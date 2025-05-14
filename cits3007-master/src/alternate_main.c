#define _GNU_SOURCE
#include "login.h"
#include "account.h"
#include "db.h"
#include "logging.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

const char *strong_password = "SecurePass123!";
const char *valid_email = "deez@tungsahur.com";
const char *valid_birthday = "2001-06-12";

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

 // Test logging functionality
  log_message(LOG_INFO, "Starting account system test");
  log_message(LOG_DEBUG, "Creating test account");

  // Test account creation with validated data
  account_t *acc = account_create("testuser", strong_password, valid_email, valid_birthday);
  if (!acc) {
    log_message(LOG_ERROR, "Failed to create account");
    return 1;
  }
  log_message(LOG_INFO, "Account created successfully");

  // Test account lookup
  account_t lookup_result;
  bool found = account_lookup_by_userid("testuser", &lookup_result);
  log_message(LOG_INFO, "Account lookup result: %s", found ? "found" : "not found");

  // Test password validation
  bool valid = account_validate_password(acc, strong_password);
  log_message(LOG_INFO, "Password validation: %s", valid ? "success" : "failed");

  // Test password update
  bool updated = account_update_password(acc, "NewStrongP@ss456");
  log_message(LOG_INFO, "Password update: %s", updated ? "success" : "failed");

  // Test login tracking
  ip4_addr_t test_ip = 0x7f000001; // 127.0.0.1
  account_record_login_success(acc, test_ip);
  account_record_login_failure(acc);
  log_message(LOG_INFO, "Login tracking completed");

  // Test account state checks
  bool banned = account_is_banned(acc);
  bool expired = account_is_expired(acc);
  log_message(LOG_INFO, "Account banned: %s", banned ? "yes" : "no");
  log_message(LOG_INFO, "Account expired: %s", expired ? "yes" : "no");

  // Test metadata updates
  time_t now = time(NULL);
  account_set_unban_time(acc, now + 3600); // Ban for 1 hour
  account_set_expiration_time(acc, now + 86400); // Expire in 1 day
  account_set_email(acc, "newemail@example.com");
  log_message(LOG_INFO, "Account metadata updated");

  // Test account summary printing
  bool printed = account_print_summary(acc, STDOUT_FILENO);
  log_message(LOG_INFO, "Account summary printed: %s", printed ? "success" : "failed");

  // Test login handling
  login_session_data_t session;
  login_result_t login_result = handle_login("testuser", "NewStrongP@ss456", 
                                           test_ip, now,
                                           STDOUT_FILENO, STDOUT_FILENO,
                                           &session);
  printf("Login result: %d\n", login_result);

  // Cleanup
  account_free(acc);
  return 0;
}