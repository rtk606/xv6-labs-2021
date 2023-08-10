#include "../kernel/types.h"
#include "../user/user.h"

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        fprintf(2, "Example usage: sleep 10\n");
        exit(1);
    }

    sleep(atoi(argv[1]));
    exit(0);
}