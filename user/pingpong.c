/* pingpongs a byte between the parent and child process */
#include "kernel/types.h"
#include "user/user.h"

int main(void) 
{
    int pipefd[2];
    char buf[5];

    if (pipe(pipefd) == -1) {
        fprintf(1, "pipe() error\n");
        exit(1);
    }

    int process_id = fork();
    if (process_id < 0) {
        fprintf(1, "fork() error\n");
        exit(1);
    }
    else if (process_id == 0) {
        // receives 5 byes from the parent process
        read(pipefd[0], &buf, 5);
        printf("%d: received %s\n", getpid(), buf);
                
        // sends 5 bytes to the parent process
        write(pipefd[1], "pong", 5);
    }
    else {
        // sends 5 bytes to the child process
        write(pipefd[1], "ping", 5);        
        wait((int *) 0);

        // receives 5 bytes from the child process
        read(pipefd[0], &buf, 5);
        printf("%d: received %s\n", getpid(), buf);
    }  

    exit(0);
}
