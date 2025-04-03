#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH 1024

void reverse_string(char *str, char *result) {
    int len = strlen(str);
    for(int i = 0; i < len; i++) {
        result[i] = str[len - 1 - i];
    }
    result[len] = '\0';
}

void reverse_file(const char *src_path, const char *dest_path) {
    FILE *src = fopen(src_path, "rb");
    FILE *dest = fopen(dest_path, "wb");

    if(!src || !dest) {
        printf("Ошибка открытия файла\n");
        if(src) {
            fclose(src);
        }
        if(dest) {
            fclose(dest);
        }
        return;
    }

    fseek(src, 0, SEEK_END);
    long size = ftell(src);
    fseek(src, 0, SEEK_SET);

    char* buffer = (char*)malloc(size);
    if(!buffer) {
        printf("Ошибка выделения памяти\n");
        fclose(src);
        fclose(dest);
        return;
    }

    fread(buffer, 1, size, src);

    for(long i = size - 1; i >= 0; i--) {
        fputc(buffer[i], dest);
    }

    free(buffer);
    fclose(src);
    fclose(dest);
}

int reverse_dir(const char *src_dir, const char *dest_dir) {
    if (mkdir(dest_dir, 0755) != 0) {
        perror("Ошибка создания каталога");
        return 1;
    }

    DIR *dir = opendir(src_dir);

    if (!dir) {
        perror("Ошибка открытия исходного каталога");
        return 1;
    }

    struct dirent *entry;
    struct stat statbuf;
    char src_path[MAX_PATH];
    char dest_path[MAX_PATH];
    char reversed_name[MAX_PATH];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(src_path, MAX_PATH, "%s/%s", src_dir, entry->d_name);

        if (stat(src_path, &statbuf) == -1) {
            perror("Ошибка получения информации о файле");
            continue;
        }

        if (S_ISREG(statbuf.st_mode)) {

            reverse_string(entry->d_name, reversed_name);

            snprintf(dest_path, MAX_PATH, "%s/%s", dest_dir, reversed_name);

            reverse_file(src_path, dest_path);
        }

        if(S_ISDIR(statbuf.st_mode)) {
            reverse_string(entry->d_name, reversed_name);

            snprintf(dest_path, MAX_PATH, "%s/%s", dest_dir, reversed_name);

            reverse_dir(src_path, dest_path);
        }
    }

    closedir(dir);
    return 0;
}

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Использование: %s <путь_к_каталогу>\n", argv[0]);
        return 1;
    }

    char *src_dir = argv[1];
    char dest_dir[MAX_PATH];

    reverse_string(src_dir, dest_dir);

    return reverse_dir(src_dir, dest_dir);
}
