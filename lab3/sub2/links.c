#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#define MAX_PATH 1024

void create_directory(const char *path) {
    if(mkdir(path, 0755) == -1) {
        printf("mkdir\n");
    }
}

void list_directory(const char *path) {
    DIR *dir = opendir(path);
    if(!dir) {
        printf("opendir\n");
        return;
    }
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}

void remove_directory(const char *path) {
    if(rmdir(path) == -1) {
        printf("rmdir\n");
    }
}

void create_file(const char *path) {
    int fd = open(path, O_CREAT | O_WRONLY, 0644);
    if(fd == -1) {
        printf("open\n");
    } else {
        close(fd);
    }
}

void cat_file(const char *path) {
    int fd = open(path, O_RDONLY);
    if(fd == -1) {
        printf("open\n");
        return;
    }
    char buffer[1024];
    ssize_t bytes;
    while((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes);
    }
    close(fd);
}

void remove_file(const char *path) {
    if(unlink(path) == -1) {
        printf("unlink\n");
    }
}

void create_symlink(const char *target, const char *linkpath) {
    if(symlink(target, linkpath) == -1) {
        printf("symlink\n");
    }
}

void read_symlink(const char *path) {
    char buffer[MAX_PATH];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if(len != -1) {
        buffer[len] = '\0';
        printf("%s\n", buffer);
    } else {
        printf("readlink\n");
    }
}

void cat_symlink(const char *path) {
    char target[MAX_PATH];
    ssize_t len = readlink(path, target, sizeof(target) - 1);
    if(len != -1) {
        target[len] = '\0';
        cat_file(target);
    } else {
        printf("readlink\n");
    }
}

void remove_symlink(const char *path) {
    if(unlink(path) == -1) {
        printf("unlink\n");
    }
}

void create_hardlink(const char *target, const char *linkpath) {
    if(link(target, linkpath) == -1) {
        printf("link\n");
    }
}

void remove_hardlink(const char *path) {
    if(unlink(path) == -1) {
        printf("unlink\n");
    }
}

void print_stat(const char *path) {
    struct stat sb;
    if(stat(path, &sb) == -1) {
        printf("stat\n");
        return;
    }
    printf("Permissions: %o\n", sb.st_mode & 0777);
    printf("Hard links: %lu\n", sb.st_nlink);
}

void change_permissions(const char *path, mode_t mode) {
    if (chmod(path, mode) == -1) {
        perror("chmod");
    }
}

int main(int argc, char *argv[]) {
    char *cmd = strrchr(argv[0], '/');
    cmd = cmd ? cmd + 1 : argv[0];

    if(strcmp(cmd, "mkdir") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        create_directory(argv[1]);
    } else if(strcmp(cmd, "ls") == 0) {
        if (argc < 2) {
            list_directory(".");
        }
        list_directory(argv[1]);
    } else if(strcmp(cmd, "rmdir") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        remove_directory(argv[1]);
    } else if(strcmp(cmd, "touch") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        create_file(argv[1]);
    } else if(strcmp(cmd, "cat") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        cat_file(argv[1]);
    } else if(strcmp(cmd, "rm") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        remove_file(argv[1]);
    } else if(strcmp(cmd, "ln-s") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        create_symlink(argv[1], argv[2]);
    } else if(strcmp(cmd, "readlink") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        read_symlink(argv[1]);
    } else if(strcmp(cmd, "cat-link") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        cat_symlink(argv[1]);
    } else if(strcmp(cmd, "rm-link") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        remove_symlink(argv[1]);
    } else if(strcmp(cmd, "ln") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        create_hardlink(argv[1], argv[2]);
    } else if(strcmp(cmd, "rm-hardlink") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        remove_hardlink(argv[1]);
    } else if(strcmp(cmd, "stat") == 0) {
        if (argc < 2) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        print_stat(argv[1]);
    } else if(strcmp(cmd, "chmod") == 0) {
        if (argc < 3) {
            printf("Использование: %s <path>\n", argv[0]);
            return 1;
        }
        mode_t mode = strtol(argv[1], NULL, 8);
        change_permissions(argv[2], mode);
    } else {
        printf("Неизвестная команда: %s\n", cmd);
        return 1;
    }
    return 0;
}

