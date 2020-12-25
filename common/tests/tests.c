#include <stdlib.h>
#include <stdio.h>

#include "maildir_test.h"

int main (int argc, char *argv[])
{
    printf("Common tests started...\n");
    test_maildir();
    printf("Common tests end.\n");
    return 0;
}