#include "test_account.h"
#include "../src/account.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        fprintf(stderr, "Failed to open input file\n");
        return 1;
    }

    char input[1024];
    size_t size = fread(input, 1, sizeof(input) - 1, f);
    fclose(f);
    input[size] = '\0';

    account_t *acc = account_create(
        "testuser", // userid
        input, // password
        "test@example.com", // email
        "20-01-01"  // birthdate
    );

    if (acc) {
        account_validate_password(acc, input);
        
        account_update_password(acc, input);
        
        account_free(acc);
    }

    return 0;
} 