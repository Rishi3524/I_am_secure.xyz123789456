#include "db.h"
#include "logging.h"
#include <string.h>
#include <stdbool.h>

#define MAX_ACCOUNTS 100

// In-memory dummy database for demonstration
static account_t dummy_db[MAX_ACCOUNTS];
static int num_accounts = 0;

// called automatically before main to preload a test account
__attribute__((constructor))
static void preload_dummy_accounts() {
    account_t acc = {0};
    strncpy(acc.userid, "cappuccinoassassino", USER_ID_LENGTH);
    acc.userid[USER_ID_LENGTH - 1] = '\0';
    strncpy(acc.email, "cappuccinoassassino@example.com", EMAIL_LENGTH);
    strncpy(acc.password_hash, "dummyhash", HASH_LENGTH);  // replace with real hash if needed
    acc.account_id = 1;
    dummy_db[num_accounts++] = acc;
}

/**
 * Looks up an account by user ID.
 *
 * Returns true and copies into *result if found.
 * Returns false otherwise.
 */
bool account_lookup_by_userid(const char *userid, account_t *acc) {
    if (userid == NULL || acc == NULL) {
        log_message(LOG_ERROR, "account_lookup_by_userid: NULL argument(s)");
        return false;
    }

    for (int i = 0; i < num_accounts; ++i) {
        if (strncmp(dummy_db[i].userid, userid, USER_ID_LENGTH) == 0) {
            *acc = dummy_db[i];  // Safe because it's a struct copy
            log_message(LOG_INFO, "User '%s' found in database", userid);
            return true;
        }
    }

    log_message(LOG_WARN, "User '%s' not found", userid);
    return false;
}
