#include "kernel/types.h"
#include "user/user.h"

#define RD 0        // stdin
#define WR 1        // stdout
#define stderr 2    // stderr

void primes(int left_pipe[2]);

int main() {
    int p[2];

    pipe(p);
    for (int i = 2; i < 36; ++i) {
        write(p[WR], &i, sizeof(int));
    }
    close(p[WR]);

    int pid;
    if ((pid = fork()) < 0) {
        close(p[RD]);
        fprintf(stderr, "Fork() error");
        exit(1);
    } else if (pid == 0) {
        // child
        primes(p);
    } else {
        // parent
        close(p[RD]);
        wait(0);
    }
    exit(0);
}

void primes(int left_pipe[2]) {
    int first;
    if (read(left_pipe[RD], &first, sizeof(int)) == sizeof(int)) {
        printf("prime %d\n", first);
    } else {
        close(left_pipe[RD]);
        exit(0);
    }

    int right_pipe[2], next;
    pipe(right_pipe);

    while (read(left_pipe[RD], &next, sizeof(int)) != 0) {
        if (next % first) {
            write(right_pipe[WR], &next, sizeof(int));
        }
    }

    close(left_pipe[RD]);
    close(right_pipe[WR]);
    /* 
        On the end of the first iteration right_pipe pipe now contains:
        [2,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33]
    
        int buf; 
        while (read(right_pipe[RD], &buf, sizeof(int)) != 0) {
                printf("Prime %d\n", buf);
        }
    */

    int pid;
    if ((pid = fork()) < 0) {
        close(right_pipe[RD]);
        fprintf(stderr, "Fork() error");
        exit(1);
    } else if (pid == 0) {
        // child
        primes(right_pipe);
    } else {
        // parent
        close(right_pipe[RD]);
        wait(0);
    } 
}

/* 
    5 % 3 = 2
    7 % 3 = 1
    9 % 3 = 0
    13 % 3 = 1
    15 % 3 = 0
*/