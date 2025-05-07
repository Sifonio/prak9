#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <errno.h>

void list_and_try_access(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char filepath[1024];

    printf("\nВміст каталогу: %s\n", path);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "ls -l %s", path);
    system(cmd);

    dir = opendir(path);
    if (!dir) {
        perror("Не вдалося відкрити каталог");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
        if (stat(filepath, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {

            snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

            if (stat(filepath, &file_stat) == -1) {
                perror("stat");
                continue;
            }

            struct passwd *pw = getpwuid(file_stat.st_uid);
            struct group  *gr = getgrgid(file_stat.st_gid);
            printf("\nФайл: %s\n", filepath);
            printf("Власник: %s, Група: %s\n", pw->pw_name, gr->gr_name);
            printf("Права доступу: %o\n", file_stat.st_mode & 0777);

            printf("Спроба читання... ");
            int fd = open(filepath, O_RDONLY);
            if (fd == -1) {
                perror("Не вдалося прочитати");
            } else {
                printf("Успіх\n");
                close(fd);
            }

            printf("Спроба запису... ");
            fd = open(filepath, O_WRONLY | O_APPEND);
            if (fd == -1) {
                perror("Не вдалося записати");
            } else {
                const char *msg = "\n# test write\n";
                if (write(fd, msg, strlen(msg)) == -1) {
                    perror("Помилка запису");
                } else {
                    printf("Успіх\n");
                }
                close(fd);
            }

            printf("Спроба виконання... ");
            if (access(filepath, X_OK) == 0) {
                printf("Файл виконуваний (але не запускаємо для безпеки)\n");
            } else {
                perror("Не вдалося виконати");
            }

            break;
        }
    }

    closedir(dir);
}

int main() {
    const char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "Не вдалося визначити домашній каталог\n");
        return 1;
    }

    list_and_try_access(home);
    list_and_try_access("/usr/bin");
    list_and_try_access("/etc");

    return 0;
}
//gcc -Wall 6.c -o 6