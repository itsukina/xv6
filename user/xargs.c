//
// Created by chenjiajun1999 on 2023/2/2.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"


int
main(int argc, char *argv[]) {

    char *argv_buf[MAXARG];
    for (int i = 1; i < argc; ++i) {
        argv_buf[i - 1] = argv[i];
    }

    char buf[1024] = {0};
    int len = read(0, buf, sizeof(buf));
    char *p, *q;
    for (p = buf, q = buf; q <= buf + len; q++) {
        if ('\n' == *q) {
            *q = '\0';
            q++;
            argv_buf[argc - 1] = p;

            int pid, status;
            pid = fork();
            if (pid > 0) {
                wait(&status);
            } else if (pid == 0) {
                exec(argv_buf[0], argv_buf);
                printf("xargs: exec failed!\n");
                exit(1);
            }
            p = q;
        }
    }

    exit(0);
}