#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void check_access(const char* path) {
    printf("Перевірка доступу до файлу: %s\n", path);
    printf("Читання дозволено: %s\n", access(path, R_OK) == 0 ? "Так" : "Ні");
    printf("Запис дозволено: %s\n", access(path, W_OK) == 0 ? "Так" : "Ні");
}

int main() {
    char template[] = "/tmp/5";
    int fd = mkstemp(template);

    if (fd == -1) {
        perror("Помилка створення тимчасового файлу");
        return 1;
    }

    printf("Тимчасовий файл створено: %s\n", template);

    const char *text = "jgoiuhv\n";
    write(fd, text, 17);
    close(fd);

    printf("\nДоступ до файлу до зміни прав:\n");
    check_access(template);

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "sudo chmod 000 %s", template);
    system(cmd);

    printf("\nДоступ після chmod 000:\n");
    check_access(template);

    snprintf(cmd, sizeof(cmd), "sudo chown root:root %s", template);
    system(cmd);

    snprintf(cmd, sizeof(cmd), "sudo chmod 644 %s", template);
    system(cmd);

    printf("\n[3] Доступ після 'chown root:root' і 'chmod 644':\n");
    check_access(template);

    snprintf(cmd, sizeof(cmd), "sudo rm -f %s", template);
    system(cmd);

    printf("\nТимчасовий файл видалено.\n");

    return 0;
}
//gcc -Wall 5.c -o 5