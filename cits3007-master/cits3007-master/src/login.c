#define _GNU_SOURCE
#include "login.h"
#include "logging.h"
#include "db.h"
#include "account.h"

#include <unistd.h>    // for write(), dprintf()
#include <string.h>    // for strlen()
#include <time.h>      // for time_t
#include "banned.h"


login_result_t handle_login(const char *userid, const char *password,
                            ip4_addr_t client_ip, time_t login_time,
                            int client_output_fd, int log_fd,
                            login_session_data_t *session)
{
    if (!userid || !password || !session) {
        const char *msg = "Login failed: internal error.\n";
        dprintf(client_output_fd, "%s", msg);
        log_message(LOG_ERROR, "ERROR: handle_login: NULL input\n");
        return LOGIN_FAIL_INTERNAL_ERROR;
    }

    account_t acc;
    if (!account_lookup_by_userid(userid, &acc)) {
        const char *msg = "Login failed: user not found.\n";
        dprintf(client_output_fd, "%s", msg);
        log_message(LOG_ERROR, "ERROR: User '%s' not found\n", userid);
        return LOGIN_FAIL_USER_NOT_FOUND;
    }

    if (account_is_banned(&acc)) {
        const char *msg = "Login failed: account banned.\n";
        dprintf(client_output_fd, "%s", msg);
        log_message(LOG_WARN, "WARNING: User '%s' is banned\n", userid);
        return LOGIN_FAIL_ACCOUNT_BANNED;
    }

    if (account_is_expired(&acc)) {
        const char *msg = "Login failed: account expired.\n";
        dprintf(client_output_fd, "%s", msg);
        log_message(LOG_WARN, "WARNING: user '%s' is expired\n", userid);
        return LOGIN_FAIL_ACCOUNT_EXPIRED;
    }

    if (!account_validate_password(&acc, password)) {
        const char *msg = "Login failed: incorrect password.\n";
        dprintf(client_output_fd, "%s", msg);
        log_message(LOG_WARN, "WARNING: incorrect password for user '%s'\n", userid);
        account_record_login_failure(&acc);
        return LOGIN_FAIL_BAD_PASSWORD;
    }

    account_record_login_success(&acc, client_ip);

    dprintf(client_output_fd, "Login successful!\n");
    dprintf(log_fd, "INFO: user '%s' logged in successfully\n", userid);

    session->account_id = acc.account_id;
    session->session_start = login_time;
    session->expiration_time = acc.expiration_time;

    return LOGIN_SUCCESS;
}
