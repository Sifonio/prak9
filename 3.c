#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>

#define FILENAME "/tmp/3.txt"
#define COPY_FILENAME "/home/artem/3.txt"

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    uid_t original_uid = getuid();
    gid_t original_gid = getgid();

    struct passwd *pw = getpwnam("artem");
    if (!pw) {
        die("getpwnam");
    }

    uid_t user_uid = pw->pw_uid;
    gid_t user_gid = pw->pw_gid;

    printf("Створення файлу від імені звичайного користувача...\n");
    if (seteuid(user_uid) != 0) die("seteuid(user_uid)");

    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) die("open");

    const char *content = "jegikodw\n";
    if (write(fd, content, strlen(content)) != (ssize_t)strlen(content)) {
        die("write");
    }
    close(fd);

    if (seteuid(0) != 0) die("seteuid(0)");

    printf("Копіювання файлу до домашнього каталогу...\n");
    char command[512];
    snprintf(command, sizeof(command), "cp %s %s", FILENAME, COPY_FILENAME);
    if (system(command) != 0) die("system(cp)");

    printf("Зміна власника копії...\n");
    if (chown(COPY_FILENAME, user_uid, user_gid) != 0) die("chown");

    printf("[+] Тепер пробуємо змінити файл від імені звичайного користувача...\n");
    if (seteuid(user_uid) != 0) die("seteuid(user_uid)");

    fd = open(COPY_FILENAME, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("open for append (як користувач)");
    } else {
        const char *new_content = "Додаємо новий рядок\n";
        if (write(fd, new_content, strlen(new_content)) != (ssize_t)strlen(new_content)) {
            perror("write (як користувач)");
        } else {
            printf("Запис успішний\n");
        }
        close(fd);
    }

    printf("Тепер пробуємо видалити файл...\n");
    if (unlink(COPY_FILENAME) != 0) {
        perror("unlink (як користувач)");
    } else {
        printf("Файл успішно видалено\n");
    }

    return 0;
}

//gcc -Wall 3.c -o 3
