//
// Created by chenjiajun1999 on 2023/2/1.
//
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), '\0', DIRSIZ - strlen(p));
    return buf;
}

void
find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            if (strcmp(fmtname(path), target) == 0)
                printf("%s\n", path);
            break;

        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if (stat(buf, &st) < 0) {
                    printf("ls: cannot stat %s\n", buf);
                    continue;
                }

                switch (st.type) {
                    case T_FILE:
                        if (strcmp(fmtname(buf), target) == 0) {
                            printf("%s\n", buf);
                        }
                        break;

                    case T_DIR:
                        if (strcmp(".", fmtname(buf)) != 0 && strcmp("..", fmtname(buf)) != 0) {
                            find(buf, target);
                        }
                }
            }
            break;
    }
    close(fd);
}

int
main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("find: the number of arguments is error");
        exit(1);
    }
    char *path = argv[1], *target = argv[2];
    if (strcmp(".", target) == 0 || strcmp("..", target) == 0) {
        printf("find: don't recurse into \".\" and \"..\"");
        exit(1);
    }
    find(path, target);
    exit(0);
}