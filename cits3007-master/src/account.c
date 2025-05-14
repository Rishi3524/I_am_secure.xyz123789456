#include "account.h"
#include "logging.h"
#include <argon2.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
/**
 * Create a new account with the specified parameters.
 *
 * This function initializes a new dynamically allocated account structure
 * with the given user ID, hash information derived from the specified plaintext password, email address,
 * and birthdate. Other fields are set to their default values.
 *
 * On success, returns a pointer to the newly created account structure.
 * On error, returns NULL and logs an error message.
 */

account_t *account_create(const char *userid, const char *plaintext_password,
                          const char *email, const char *birthdate
                      )
{
  if (userid[0] == '\0') {
    log_message(LOG_ERROR, "account_create: Must input a user ID");
    return NULL;
  }
  bool spaceorworsename = true;
  for (size_t i = 0; i < strlen(userid); i++) {
    if (userid[i] == ' ' || !isprint((unsigned char)userid[i])) {
      spaceorworsename = false;
      break;
    }
  }
  if (spaceorworsename) {
    log_message(LOG_ERROR, "account_create: User ID has invalid characters");
    return NULL;
  }
  // valiate email
  if (!validate_email(email)) {
      log_message(LOG_ERROR,"account_create: Invalid email error");
      return NULL;
  }

  //validate birthday
  if (!account_validate_birthday(birthdate)) {
    log_message(LOG_ERROR,"account_create: Invalid birthday error");
    return NULL;
  }

  if (strlen(userid) >= USER_ID_LENGTH) {
    log_message(LOG_ERROR, "account_create: User ID too long");
    return NULL;
  }
  
  if (!is_password_strong(plaintext_password)) {
    log_message(LOG_ERROR, "account_create: Password is not strong enough");
    return NULL;
  }
  // try to allocate memory for the account
  account_t *account = malloc(sizeof(account_t));
  if (!account) {
    log_message(LOG_ERROR,"account_create: Failed to allocate memory");
    return NULL;
  }
  memset(account, 0, sizeof(account_t));

  strncpy(account->userid, userid, USER_ID_LENGTH - 1);
  account->userid[USER_ID_LENGTH - 1] = '\0';
  strncpy(account->birthdate, birthdate, BIRTHDATE_LENGTH - 1);
  account->birthdate[BIRTHDATE_LENGTH - 1] = '\0';
  strncpy(account->email, email, EMAIL_LENGTH - 1);
  account->email[EMAIL_LENGTH - 1] = '\0';

  // PASSWORD HASHING
  uint32_t t_cost = 3;        //iterations 
  uint32_t m_cost = 1 << 16;  // 64 MiB memory cost
  uint32_t parallelism = 1;   // threads
  unsigned char salt[16];
  if (!generate_secure_random(salt, sizeof(salt))) {
    log_message(LOG_ERROR, "Failed to generate secure random salt for password hash");
    free(account);
    return NULL;
  }

  char hash[HASH_LENGTH];
  memset(hash, 0, HASH_LENGTH);

  size_t password_len = strlen(plaintext_password);

  int result = argon2id_hash_encoded(
    t_cost, m_cost, parallelism,
    plaintext_password, password_len,
    salt, sizeof(salt),
    32, 
    hash, HASH_LENGTH - 1 
  );

  if (result != ARGON2_OK) {
    log_message(LOG_ERROR, "Failed to hash password during update: %s", argon2_error_message(result));
    free(account);
    return NULL;
  }

  strncpy(account->password_hash, hash, HASH_LENGTH - 1);
  account->password_hash[HASH_LENGTH - 1] = '\0';
  memset(hash, 0, HASH_LENGTH); //no pass remnants in memory

  account->unban_time = 0;                // Ban the account up until this time (0 = no ban)
  account->expiration_time = 0;           // Account is only valid until this time (0 = unlimited)
  account->login_count = 0;         // Number of successful auth attempts, default = 0
  account->login_fail_count = 0;    // Number of unsuccessful auth attempts, default = 0
  account->last_login_time = 0;           // Time of last successful login, default = time 0.
  account->last_ip = 0;               // Last IP connected from, default = 0

  return account;
}

void account_free(account_t *acc) {
  if (acc) {
    explicit_bzero(acc->password_hash, HASH_LENGTH);
    memset(acc, 0, sizeof(*acc));
    free(acc); 
  }
  //will need to free everything else
  // this also feels to simple idk
}

bool validate_email(const char *email) {
  if (strlen(email) >= EMAIL_LENGTH) {
    return false;
  }
  while (*email) {
    if (*email == ' ' || !isprint((unsigned char)*email)) {
      return false;
    }
    email++;
  }
  return true;
}

bool account_validate_birthday(const char *birthday) {
  //YYYY-MM-DD
  if (strlen(birthday) != BIRTHDATE_LENGTH) {
    return false;
  }

  for (int i = 0; i < 10; i++) {
    if (i == 4 || i == 7) {
      if (birthday[i] != '-') { 
        return false;
      }
    } else {
      if (!isdigit((unsigned char)*birthday[i])) {
        return false;
      }
    }
  }

  char year[5], month[3], day[3];
  strncpy(year, birthday, 4);
  year[4] = ‘\0’;
  strncpy(month, birthday+5, 2);
  month[2] = ‘\0’; 
  strncpy(day, birthday+7, 2);
  day[2] = ‘\0’; 
  int iyear = atoi(year);
  int imonth = atoi(month);
  int iday = atoi(day);
  iyear = atoi(year);
  /*if (iyear > 2025 || iyear < 1875) { 
    return false;
  } */
  if (imonth > 12 || imonth < 1 || iday < 1) {
    return false;
  }
  int maximumdays = 31;
  if (imonth == 4 || imonth == 6 || imonth == 9 || imonth == 11) {
    maximumdays = 30;
  }

  if (imonth == 2) {
    if (iyear %4 == 0 && (iyear%100!= 0|| iyear % 400 == 0)) {
      maximumdays = 29;
    } else {
      maximumdays = 28;
    }
  }

  if (iday > maximumdays) {
    return false;
  }

  return true;
}
/**
 * Validate password complexity
 */
static bool is_password_strong(const char *password) {
    if (password == NULL) {
        return false;
    }
    
    size_t length = strlen(password);
    /* Enforce minimum length */
    if (length < 8) {
        return false;
    }
    
    /* Check for character diversity */
    bool has_upper = false, has_lower = false;
    bool has_digit = false, has_special = false;
    
    for (size_t i = 0; i < length; i++) {
        if (isupper((unsigned char)password[i])) has_upper = true;
        else if (islower((unsigned char)password[i])) has_lower = true;
        else if (isdigit((unsigned char)password[i])) has_digit = true;
        else has_special = true;
    }
    
    /* Require at least 3 different character types */
    int diversity = has_upper + has_lower + has_digit + has_special;
    return diversity >= 3;
}

/**
 * Generate cryptographically secure random bytes
 */
static bool generate_secure_random(unsigned char *buffer, size_t length) {
    /* Use /dev/urandom for secure random data */
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        log_message(LOG_ERROR, "Failed to open /dev/urandom");
        return false;
    }
    
    size_t bytes_read = 0;
    while (bytes_read < length) {
        ssize_t result = read(fd, buffer + bytes_read, length - bytes_read);
        if (result <= 0) {
            close(fd);
            log_message(LOG_ERROR, "Failed to read from /dev/urandom");
            return false;
        }
        bytes_read += result;
    }
    
    close(fd);
    return true;
}

/*
 * Check if an account is currently banned
 */
bool account_is_banned(const account_t *acc) {
    /* Input validation */
    if (acc == NULL) {
        log_message(LOG_WARN, "NULL account passed to account_is_banned");
        return false;
    }
    
    /* If unban_time is 0, the account is not banned */
    if (acc->unban_time == 0) {
        return false;
    }
    
    /* Get current time */
    time_t current_time = time(NULL);
    if (current_time == (time_t)-1) {
        /* Failed to get system time - log error and fail securely */
        log_message(LOG_ERROR, "Failed to get system time in account_is_banned");
        return true; /* Secure default: treat as banned if we can't verify */
    }
    
    /* Account is banned if current time is less than unban_time */
    return current_time < acc->unban_time;
}

/**
 * Check if an account has expired
 */
bool account_is_expired(const account_t *acc) {
    /* Input validation */
    if (acc == NULL) {
        log_message(LOG_WARN, "NULL account passed to account_is_expired");
        return false;
    }
    
    /* If expiration_time is 0, the account never expires */
    if (acc->expiration_time == 0) {
        return false;
    }
    
    /* Get current time */
    time_t current_time = time(NULL);
    if (current_time == (time_t)-1) {
        /* Failed to get system time - log error and fail securely */
        log_message(LOG_ERROR, "Failed to get system time in account_is_expired");
        return true; /* Secure default: treat as expired if we can't verify */
    }
    
    /* Account is expired if current time is greater than or equal to expiration_time */
    return current_time >= acc->expiration_time;
}

/**
 * Check if an account is rate limited due to too many failed login attempts
 */
static bool is_account_rate_limited(const account_t *acc) {
    if (acc == NULL) {
        return false;
    }
    
    /* Rate limiting threshold - increases exponentially with failures */
    if (acc->login_fail_count < 3) {
        return false;  /* Allow up to 3 failures without limits */
    } else if (acc->login_fail_count < 5) {
        return false;  /* Allow up to 5 failures */
    } else if (acc->login_fail_count < 10) {
        /* Between 5-10 failures - implement a soft rate limit */
        return true;
    } else {
        /* More than 10 failures - hard rate limit */
        log_message(LOG_WARN, "Account exceeded maximum login failures");
        return true;
    }
}

/**
 * Validate a password against a stored hash
 */
bool account_validate_password(const account_t *acc, const char *plaintext_password) {
    /* Input validation */
    if (acc == NULL || plaintext_password == NULL) {
        log_message(LOG_WARN, "NULL parameter passed to account_validate_password");
        return false;
    }

    /* Check if the hash is empty or invalid */
    if (strlen(acc->password_hash) == 0) {
        log_message(LOG_ERROR, "Account has no password hash");
        return false;
    }

    /* Check for account ban status */
    if (account_is_banned(acc)) {
        log_message(LOG_WARN, "Password validation attempted on banned account");
        return false;
    }
    
    /* Check for account expiration */
    if (account_is_expired(acc)) {
        log_message(LOG_WARN, "Password validation attempted on expired account");
        return false;
    }
    
    /* Check for rate limiting */
    if (is_account_rate_limited(acc)) {
        log_message(LOG_WARN, "Password validation rate limited due to too many failures");
        return false;
    }

    /* Use Argon2 to verify the password against the stored hash */
    int result = argon2id_verify(acc->password_hash, plaintext_password, strlen(plaintext_password));
    
    /* Log security-relevant events */
    if (result != ARGON2_OK) {
        log_message(LOG_WARN, "Failed password validation attempt");
    }
    
    return (result == ARGON2_OK);
}

/**
 * Update a password with a new hash
 */
bool account_update_password(account_t *acc, const char *new_plaintext_password) {
    /* Input validation */
    if (acc == NULL || new_plaintext_password == NULL) {
        log_message(LOG_WARN, "NULL parameter passed to account_update_password");
        return false;
    }

    /* Validate password complexity */
    if (!is_password_strong(new_plaintext_password)) {
        log_message(LOG_WARN, "Password change rejected due to insufficient complexity");
        return false;
    }

    /* Argon2 parameters (tuned for security) */
    uint32_t t_cost = 3;        /* Number of iterations */
    uint32_t m_cost = 1 << 16;  /* 64 MiB memory cost */
    uint32_t parallelism = 1;   /* Number of threads */
    
    /* Generate a cryptographically secure random salt */
    unsigned char salt[16];
    if (!generate_secure_random(salt, sizeof(salt))) {
        log_message(LOG_ERROR, "Failed to generate secure random salt for password hash");
        return false;
    }

    /* Create a buffer for the hashed password */
    char hash[HASH_LENGTH];
    memset(hash, 0, HASH_LENGTH);

    /* Password length for hash calculation */
    size_t password_len = strlen(new_plaintext_password);

    /* Hash the password using Argon2id */
    int result = argon2id_hash_encoded(
        t_cost, m_cost, parallelism,
        new_plaintext_password, password_len,
        salt, sizeof(salt),
        32, /* Hash length in bytes */
        hash, HASH_LENGTH - 1 /* Ensure space for null terminator */
    );

    if (result != ARGON2_OK) {
        log_message(LOG_ERROR, "Failed to hash password during update: %s", 
                   argon2_error_message(result));
        return false;
    }

    /* Update the account with the new hash */
    strncpy(acc->password_hash, hash, HASH_LENGTH - 1);
    acc->password_hash[HASH_LENGTH - 1] = '\0'; /* Ensure null termination */
    
    /* Log the password change event */
    log_message(LOG_INFO, "Password successfully updated");
    
    /* Reset login failure count after successful password update */
    acc->login_fail_count = 0;
    acc->unban_time = 0;  // Remove any ban
    
    return true;
}

void account_record_login_success(account_t *acc, ip4_addr_t ip) { //DONE
  if (acc == NULL) {
    log_message(LOG_ERROR, "account_record_login_success: NULL account pointer");
    return;
  }
  acc->login_count += 1; // increment successful‐login count
  acc->login_fail_count = 0; // reset consecutive failures
  acc->last_login_time = time(NULL);
  acc->last_ip = ip;
}

void account_record_login_failure(account_t *acc) { //DONE
  if (acc == NULL) {
    log_message(LOG_ERROR, "account_record_login_failure: NULL account pointer");
    return;
  }
  acc->login_fail_count += 1; // increment consecutive failures
  acc->login_count = 0; // reset success count
}

void account_set_unban_time(account_t *acc, time_t t) { //DONE
  if (acc == NULL) {
    log_message(LOG_ERROR, "account_set_unban_time: NULL account pointer");
    return;
  } 
  acc->unban_time = t;

void account_set_expiration_time(account_t *acc, time_t t) { //DONE
  if (acc == NULL) {
    log_message(LOG_ERROR, "account_set_expiration_time: NULL account pointer");
    return;
  }
  acc->expiration_time = t;
}

void account_set_email(account_t *acc, const char *new_email) {
  if (!acc || !new_email) {
    log_message("account_set_email: Null input error");
    return;
  }
  if (validate_email(new_email)) {
    log_message("account_set_email: Invalid email");
    return;
  }
  // email is just an array
  strncpy(acc->email, new_email, EMAIL_LENGTH - 1);
  acc->email[EMAIL_LENGTH - 1] = '\0'; 
}

bool account_print_summary(const account_t *acct, int fd) {
  /* Write a brief humanreadable summary of the account’s current status to the given file descriptor. The exact format
     is up to you, but it should include user ID, email, and login-related statistics (dates, counts and
     IP addresses). Return true on success, or false if the write fails.
  */
  FILE *filepointer = fdopen(fd, "a");
  
  if (!filepointer) {
    log_message(LOG_ERROR, "account_print_summary: Failed to open file");
    return false;
  }

  int charswritten = fprintf(filepointer, "User %s, contactable at %s, has had %d successful logins and %d unsuccessful login attempts.\n", acct->userid, acct->email, acct->login_count, acct->login_fail_count);
  if (charswritten <= 0) {
    log_message(LOG_ERROR, "account_print_summary: Failed to write to file");
    return false;
  }
 
  charswritten = fprintf(filepointer, "%s last logged in at %d with IP address %d.", acct->userid, acct->last_login_time, acct->last_ip);
  if (charswritten <= 0) {
    log_message(LOG_ERROR, "account_print_summary: Failed to write to file");
    return false;
  }
  if (fflush(filepointer) != 0) {
    log_message(LOG_ERROR, "account_print_summary: Failed to write to file");
    return false;
  }
 
  fclose(filepointer);
  return true;
}
