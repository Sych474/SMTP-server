#include <stdlib.h>
#include <stdio.h>

#include "maildir_test.h"
#include "address_test.h"

int main (int argc, char *argv[])
{
    printf("Common tests started...\n");
    test_maildir();
    test_address();
    printf("Common tests end.\n");
    return 0;
}