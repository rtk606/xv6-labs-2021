#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define RD 0
#define WR 1
#define stderr 2

int readline(int fd, char *buffer, int size) {
    char *p = &buffer[0];
    
    // Read one byte from fd into p
    while ((p < buffer + size) && (read(fd, p, 1) == 1)) {
        if (*p == ' ' || *p == '\n' || *p == '\t') {
            // Create delineator
            *p = '\0';
            return p - buffer;
        }
        p++;
    }

    if (p >= buffer + size) {
        fprintf(stderr, "Read error: input line is too long\n");
        return 0;
    }

    *p = '\0';
    return p - buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: xarg<cmd>\n");
        exit(1);
    }
    if (argc > MAXARG) {
        fprintf(stderr, "Error: too many arguments, the maximum # of arguments xv6 supports is 32\n");
        exit(1);
    } 

    char buffer[512];

    // Construct a new arg list
    char *nargv[MAXARG];
    memcpy(nargv, argv + 1, sizeof(char *) * (argc - 1));
    nargv[argc - 1] = &buffer[0];

    while (readline(RD, &buffer[0], sizeof(buffer))) {
        int pid = fork();
        if (pid < 0) {
            // Error
            fprintf(stderr, "xargs: fork failed\n");
            exit(1);
        } else if (pid == 0) {
            // Child
            exec(nargv[0], nargv);
            fprintf(stderr, "xargs: exec %s failed\n", nargv[0]);
            exit(1);
        } else {
            // Parent
            wait(0);
        }
    }

    exit(0);
}



/* 
    Test command: echo hello too | xargs echo bye

    Intuition:
    
    The output "hello too" from the echo hello too command is sent to the standard input (stdin) 
    of the xargs command because of the pipe | operator. Your xargs implementation needs to read 
    from stdin to get this output.

    Check argv debug:    
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    Confirm new argv debug:
    for (int i = 0; i < nargc; i++) {
        printf("nargv[%d] = %s\n", i, nargv[i]);
    }    
*/

