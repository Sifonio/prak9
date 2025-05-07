#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Виконуємо команду 'whoami':\n");
    int whoami_status = system("whoami");
    if (whoami_status != 0) {
        printf("Не вдалося виконати команду 'whoami'\n");
    }

    printf("\nВиконуємо команду 'id':\n");
    int id_status = system("id");
    if (id_status != 0) {
        printf("Не вдалося виконати команду 'id'\n");
    }

    return 0;
}
//gcc -Wall 4.c -o 4