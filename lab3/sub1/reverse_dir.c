#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>

#define MAX_PATH 1024
#define BUFFER_SIZE 1024

void split_path(const char *input, char **dir_path, char **dir_name) {
    char resolved_path[MAX_PATH];

    if (realpath(input, resolved_path) == NULL) {
        printf("Ошибка при создании абсолютного пути\n");
        exit(EXIT_FAILURE);
    }

    *dir_name = strdup(basename(resolved_path));
    *dir_path = strdup(dirname(resolved_path));

    int path_len = strlen(*dir_path);
    if ((*dir_path)[path_len - 1] == '/') {
        (*dir_path)[path_len - 1] = '\0';
    }
}

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
        perror("Ошибка открытия файла\n");
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

    char buffer[BUFFER_SIZE];
    long remaining = size;
    while (remaining > 0) {
        long chunk_size = remaining >= BUFFER_SIZE ? BUFFER_SIZE : remaining;
        long offset = remaining - chunk_size;

        fseek(src, offset, SEEK_SET);
        fread(buffer, 1, chunk_size, src);

        for (long i = chunk_size - 1; i >= 0; i--) {
            fputc(buffer[i], dest);
        }

        remaining -= chunk_size;
    }

    fclose(src);
    fclose(dest);
}

int reverse_dir(const char *src_dir, const char *dest_path) {
    char* src_path;
    char* src_name;
    split_path(src_dir, &src_path, &src_name);

    char reversed_name[MAX_PATH];
    reverse_string(src_name, reversed_name);

    char dest_dir[MAX_PATH];
    snprintf(dest_dir, MAX_PATH, "%s/%s", dest_path, reversed_name);

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
    char path[MAX_PATH];

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

            snprintf(path, MAX_PATH, "%s/%s", dest_dir, reversed_name);

            reverse_file(src_path, path);
        }

        if(S_ISDIR(statbuf.st_mode)) {
            reverse_dir(src_path, dest_dir);
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
    char dest_path[MAX_PATH];

    char* src_path;
    char* src_name;
    split_path(src_dir, &src_path, &src_name);

    memcpy(dest_path, src_path, MAX_PATH);
    reverse_dir(src_dir, dest_path);

    return 0;
}

