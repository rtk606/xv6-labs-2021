#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define RD 0
#define WR 1
#define stderr 2

void find(const char* path, const char* target);

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: find <folder> <file>\n");
        exit(1);
    }

    //   path   | file (target)
    find(argv[1], argv[2]);    
    exit(0);
}

void find(const char* path, const char* target) 
{
    char buffer[512];
    char* p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, RD)) < 0) {
        fprintf(stderr, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0) {
		fprintf(stderr, "find: cannot stat %s\n", path);
		exit(1);
	}

    // Path : "."
    switch(st.type) {
        case T_FILE:
            fprintf(stderr, "Usage: find <folder> <file>\n");
            exit(1);            
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buffer) {
                printf("ls: path too long\n");
                break;
            }
            strcpy(buffer, path);               // ['.', '\0']
            p = buffer + strlen(buffer);        // we're now at the null terminator 
            *p++ = '/';                         // ['.', '/'] but we now point one past '/'

            // Read directory entries 'file1.txt', '.', '..', 'dir1/' etc etc
            while(read(fd, &de, sizeof(de)) == sizeof(de)) {
                if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);    // dest, src, n bytes DIRSIZ = 14, xv6 specific restriction (file names were restricted to 14 char max in early iterations of UNIX)
                p[DIRSIZ] = 0;

                // Buffer will now look something like: ./fileA.txt or dir2/ etc after memmove, stat will retrieve information about the file or directory specified by buffer
                if(stat(buffer, &st) < 0) {
                    printf("find: cannot stat %s\n", buffer);
                    continue;
                }
                if (st.type == T_DIR) {
                    find(buffer, target);
                } else if (st.type == T_FILE) {
                    if (strcmp(de.name, target) == 0) {
                        printf("%s\n", buffer);
                    }
                }
            }
            break;  
    }
    close(fd);
}

/*

    find - The program's name (argv[0]).
    . - The first argument (argv[1]).
    b - The second argument (argv[2]).



    Expected outputs:  

    $ make qemu
    ...
    init: starting sh
    $ echo > b
    $ mkdir a
    $ echo > a/b
    $ find . b
    ./b
    ./a/b
    $ 

    
    while(read(fd, &de, sizeof(de)) == sizeof(de))

    .               (refers to current directory)
    ..              (refers to parent directory)
    file1.txt
    file2.jpg
    dir1/
    file3.md
    dir2/
*/