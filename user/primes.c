//
// Created by chenjiajun1999 on 2023/2/1.
//
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void find_primes(int *father_fd) {

    int primes;
    // read() is block, get a number from left neighbor
    // Hint: read returns zero when the write-side of a pipe is closed.
    if(read(father_fd[0], &primes, sizeof(primes)) == 0){
        close(father_fd[0]);
        exit(0);
    }
    printf("prime %d\n", primes);

    // get all numbers that send to right neighbor and close father_fd

    int children_fd[2];
    if (pipe(children_fd) == -1) {
        printf("Create a pipe failed.\n");
        exit(1);
    }
    int pid = fork(), status;
    if (pid > 0) {
        close(children_fd[0]);
        while (1) {
            int num;
            // Hint: read returns zero when the write-side of a pipe is closed.
            if(read(father_fd[0], &num, sizeof(num)) == 0){
                break;
            }
            if (num % primes != 0) {
                write(children_fd[1], &num, sizeof(num));
            }
        }
        close(children_fd[1]);
        wait(&status);
    }else if(pid == 0){
        close(children_fd[1]);
        find_primes(children_fd);
    }
    exit(0);
}


int
main(int argc, char *argv[]) {

    int pipe_fd[2];
    // create a pipe before fork()
    if (pipe(pipe_fd) == -1) {
        printf("Create a pipe failed.\n");
        exit(1);
    }
    int pid = fork(), status;
    if (pid > 0) {
        // write data into pipe
        close(pipe_fd[0]);
        for (int i = 2; i <= 35; i++) {
            write(pipe_fd[1], &i, sizeof(i));
        }
        // wait until the entire pipeline terminate
        close(pipe_fd[1]);
        wait(&status);
    } else if (pid == 0) {
        close(pipe_fd[1]);
        find_primes(pipe_fd);
    }
    exit(0);
}