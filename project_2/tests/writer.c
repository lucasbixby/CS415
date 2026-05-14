#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int id = atoi(argv[1]);

    for (int i = 0; i < 10; i++) {
        printf("Process %d: line %d\n", id, i);
        fflush(stdout);  // force immediate write
        usleep(100000);  // 0.1 sec delay
    }

    return 0;
}