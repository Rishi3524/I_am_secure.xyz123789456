#include "account.h"
#include "logging.h"
#include <argon2.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

/**
 * Validate password complexity
 * 
 * Lecture reference: "Terminology and techniques" slides discuss input validation
 * and "Defense in depth" is mentioned in "Design processes and principles" slides
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
 * 
 * Lecture reference: "Cryptography basics" slides discuss the importance of
 * proper random number generation for security applications
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

/**
 * Create a new account with the specified parameters.
 *
 * Lecture reference: "Input validation" slides on validating and sanitizing inputs
 */
account_t *account_create(const char *userid, const char *plaintext_password,
                          const char *email, const char *birthdate)
{
    /* Input validation */
    if (userid == NULL || plaintext_password == NULL || 
        email == NULL || birthdate == NULL) {
        log_message(LOG_ERROR, "NULL parameter passed to account_create");
        return NULL;
    }
    
    /* Validate password strength */
    if (!is_password_strong(plaintext_password)) {
        log_message(LOG_ERROR, "Password does not meet complexity requirements");
        return NULL;
    }
    
    /* Allocate memory for the account */
    account_t *acc = calloc(1, sizeof(account_t));
    if (acc == NULL) {
        log_message(LOG_ERROR, "Failed to allocate memory for account");
        return NULL;
    }
    
    /* Copy user information with bounds checking */
    strncpy(acc->userid, userid, USER_ID_LENGTH - 1);
    acc->userid[USER_ID_LENGTH - 1] = '\0';
    
    strncpy(acc->email, email, EMAIL_LENGTH - 1);
    acc->email[EMAIL_LENGTH - 1] = '\0';
    
    strncpy(acc->birthdate, birthdate, BIRTHDATE_LENGTH - 1);
    acc->birthdate[BIRTHDATE_LENGTH - 1] = '\0';
    
    /* Default values */
    acc->unban_time = 0;
    acc->expiration_time = 0;
    acc->login_count = 0;
    acc->login_fail_count = 0;
    acc->last_login_time = 0;
    acc->last_ip = 0;
    
    /* Hash and set the password */
    if (!account_update_password(acc, plaintext_password)) {
        log_message(LOG_ERROR, "Failed to set initial password");
        free(acc);
        return NULL;
    }
    
    return acc;
}

/**
 * Free memory and resources used by the account
 */
void account_free(account_t *acc) {
    if (acc != NULL) {
        free(acc);
    }
}

/**
 * Check if an account is currently banned
 *
 * Lecture reference: "Complete mediation" principle from "Design processes and principles"
 * slides - every access to a resource should be checked for authorization
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
 *
 * Lecture reference: "Complete mediation" principle from "Design processes and principles"
 * slides - checking access to resources consistently
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
 * 
 * Lecture reference: "Defense in depth" principle from "Design processes and principles" 
 * slides - implementing multiple layers of security
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
 * 
 * Lecture reference: "Password storage" slides discuss the importance
 * of properly hashing passwords and validating them
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
 * 
 * Lecture reference: "Password storage" slides discuss password hashing
 * and salt generation
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
    
    return true;
}

/**
 * Record a successful login
 * 
 * Lecture reference: "Accountability" principle mentioned in security goals
 */
void account_record_login_success(account_t *acc, ip4_addr_t ip) {
    if (acc == NULL) {
        log_message(LOG_ERROR, "NULL account passed to account_record_login_success");
        return;
    }
    
    /* Update login count */
    acc->login_count++;
    
    /* Update last login time */
    acc->last_login_time = time(NULL);
    
    /* Update last IP */
    acc->last_ip = ip;
    
    /* Reset failed login counter */
    acc->login_fail_count = 0;
    
    log_message(LOG_INFO, "Successful login recorded for user %s", acc->userid);
}

/**
 * Record a failed login
 * 
 * Lecture reference: "Defense in depth" principle - implementing security in layers
 */
void account_record_login_failure(account_t *acc) {
    if (acc == NULL) {
        log_message(LOG_ERROR, "NULL account passed to account_record_login_failure");
        return;
    }
    
    /* Increment failed login counter */
    acc->login_fail_count++;
    
    /* Log the failure */
    log_message(LOG_WARN, "Failed login attempt recorded for user %s (count: %u)", 
               acc->userid, acc->login_fail_count);
    
    /* Implement automatic temporary ban if too many failures */
    if (acc->login_fail_count >= 10) {
        /* Ban for 30 minutes after 10 failures */
        time_t ban_time = time(NULL) + (30 * 60);
        account_set_unban_time(acc, ban_time);
        log_message(LOG_WARN, "Account temporarily banned due to excessive login failures");
    }
}

/**
 * Set a ban time for an account
 */
void account_set_unban_time(account_t *acc, time_t t) {
    if (acc == NULL) {
        log_message(LOG_ERROR, "NULL account passed to account_set_unban_time");
        return;
    }
    
    acc->unban_time = t;
    
    if (t == 0) {
        log_message(LOG_INFO, "Ban removed for user %s", acc->userid);
    } else {
        log_message(LOG_INFO, "User %s banned until %ld", acc->userid, (long)t);
    }
}

/**
 * Set an expiration time for an account
 */
void account_set_expiration_time(account_t *acc, time_t t) {
    if (acc == NULL) {
        log_message(LOG_ERROR, "NULL account passed to account_set_expiration_time");
        return;
    }
    
    acc->expiration_time = t;
    
    if (t == 0) {
        log_message(LOG_INFO, "Expiration removed for user %s", acc->userid);
    } else {
        log_message(LOG_INFO, "User %s will expire at %ld", acc->userid, (long)t);
    }
}

/**
 * Set account email address
 */
void account_set_email(account_t *acc, const char *new_email) {
    if (acc == NULL || new_email == NULL) {
        log_message(LOG_ERROR, "NULL parameter passed to account_set_email");
        return;
    }
    
    /* Copy with bounds checking */
    strncpy(acc->email, new_email, EMAIL_LENGTH - 1);
    acc->email[EMAIL_LENGTH - 1] = '\0';
    
    log_message(LOG_INFO, "Email updated for user %s", acc->userid);
}

/**
 * Print account information to the specified file descriptor
 */
bool account_print_summary(const account_t *acct, int fd) {
    if (acct == NULL || fd < 0) {
        log_message(LOG_ERROR, "Invalid parameters to account_print_summary");
        return false;
    }
    
    /* Format the account information */
    char summary[1024];
    time_t current_time = time(NULL);
    
    /* Format times in readable format */
    char last_login[64] = "Never";
    if (acct->last_login_time > 0) {
        struct tm *tm_info = localtime(&acct->last_login_time);
        if (tm_info) {
            strftime(last_login, sizeof(last_login), "%Y-%m-%d %H:%M:%S", tm_info);
        }
    }
    
    char unban_time[64] = "Not banned";
    if (acct->unban_time > 0) {
        if (current_time < acct->unban_time) {
            struct tm *tm_info = localtime(&acct->unban_time);
            if (tm_info) {
                strftime(unban_time, sizeof(unban_time), "Banned until %Y-%m-%d %H:%M:%S", tm_info);
            }
        } else {
            strncpy(unban_time, "Ban expired", sizeof(unban_time) - 1);
        }
    }
    
    char expiration[64] = "Never expires";
    if (acct->expiration_time > 0) {
        struct tm *tm_info = localtime(&acct->expiration_time);
        if (tm_info) {
            strftime(expiration, sizeof(expiration), "Expires on %Y-%m-%d %H:%M:%S", tm_info);
        }
    }
    
    /* Format IP address */
    char ip_str[16];
    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", 
             (acct->last_ip >> 24) & 0xff,
             (acct->last_ip >> 16) & 0xff,
             (acct->last_ip >> 8) & 0xff,
             acct->last_ip & 0xff);
    
    /* Create the summary */
    int len = snprintf(summary, sizeof(summary),
                      "Account Summary for: %s\n"
                      "Email: %s\n"
                      "Birth Date: %s\n"
                      "Login Count: %u\n"
                      "Failed Login Count: %u\n"
                      "Last Login: %s\n"
                      "Last IP: %s\n"
                      "Status: %s, %s\n",
                      acct->userid, 
                      acct->email,
                      acct->birthdate,
                      acct->login_count,
                      acct->login_fail_count,
                      last_login,
                      ip_str,
                      account_is_banned(acct) ? "Banned" : "Active",
                      account_is_expired(acct) ? "Expired" : expiration);
    
    if (len < 0 || (size_t)len >= sizeof(summary)) {
        log_message(LOG_ERROR, "Buffer overflow prevented in account_print_summary");
        return false;
    }
    
    /* Write to the file descriptor */
    if (write(fd, summary, len) != len) {
        log_message(LOG_ERROR, "Failed to write account summary to descriptor");
        return false;
    }
    
    return true;
}
