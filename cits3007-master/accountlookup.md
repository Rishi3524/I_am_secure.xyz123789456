# account_lookup_by_userid() Implementation Summary

## Function Overview

**Function Signature:**
`bool account_lookup_by_userid(const char *userid, account_t *acc); `



**Defined In:** 
`src/db.c`

**Purpose:** 
<p>Searches the internal account database for a user with the provided userid. If a match is found, copies the corresponding account_t struct into the caller-provided pointer and returns true. Returns false if the user does not exist or if the input is invalid.</p>


## Key Features
*   Performs strict string comparison (strncmp) to match userid

*   Safely handles NULL and empty inputs

*   Logs results using log_message() at appropriate levels (INFO, WARNING, ERROR)

*   Operates on an in-memory dummy database for testing

*   Complies with secure coding practices: input validation, clear failure modes, no memory corruption

## Testing Implementation
**Test File:** 
`src/alternate_main.c`

**Compile command:** 
`make CFLAGS='-DALTERNATE_MAIN -fsanitize=address,undefined -g' clean all`

**Execution:** `./bin/app`

<p>The test program defines individual tests and tracks how many tests pass.</p>



| Test Name          | Description                                                     | Expected Result |
| ------------------ | --------------------------------------------------------------- | --------------- |
| Valid user         | Looks up "cappuccinoassassino" (exists in dummy DB)             | `true`          |
| Nonexistent user   | Looks up "ballerinacappuccina" (not in DB)                      | `false`         |
| Empty string       | Looks up "" (invalid input)                                     | `false`         |
| NULL input         | Passes NULL pointer for userid                                  | `false`         |
| Case sensitivity   | Looks up "CappuccinoAssassino" (case mismatch)                  | `false`         |
| Trailing spaces    | Looks up "cappuccinoassassino   " (with extra whitespace)       | `false`         |
| Embedded null      | Looks up "cappuccinoassassino\0hacked" (null terminator test)   | `true`          |
| Overly long userid | Passes 255-character string of 'a' to test string length limits | `false`         |
