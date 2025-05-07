#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

#define UID_THRESHOLD 1000
#define MAX_LINE_LENGTH 1024

int main() {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    char *username, *uid_str;
    int uid;
    int found = 0;
    uid_t current_uid = getuid();
    struct passwd *pw = getpwuid(current_uid);
    if (pw == NULL) {
        perror("getpwuid");
        return 1;
    }
    const char *current_user = pw->pw_name;

    fp = popen("getent passwd", "r");
    if (fp == NULL) {
        perror("popen");
        return 1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, MAX_LINE_LENGTH);
        line_copy[MAX_LINE_LENGTH - 1] = '\0';

        username = strtok(line_copy, ":");// username
        strtok(NULL, ":");// password
        uid_str = strtok(NULL, ":");// uid

        if (username == NULL || uid_str == NULL) {
            continue;
        }

        uid = atoi(uid_str);

        if (uid >= UID_THRESHOLD && strcmp(username, current_user) != 0) {
            printf("Знайдено іншого користувача: %s (UID: %d) \n", username, uid);
            found = 1;
        }
    }

    pclose(fp);
    if (!found) {
        printf("Інших користувачів не знайдено\n");
    }
    
    return 0;
}
//gcc -Wall 1.c -o 1