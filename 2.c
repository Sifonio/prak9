#include <stdio.h>
#include <stdlib.h>

int main() {
    int ret;
    ret = system("sudo cat /etc/shadow");

    if (ret == -1) {
        perror("system");
        return 1;
    }

    return 0;
}

//gcc -Wall 2.c -o 2