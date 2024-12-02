#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    int p[2];
    pipe(p);
    if (fork() == 0) {
        close(p[0]);
        write(p[1], "pipe", strlen("pipe") + 1);
        close(p[1]);
        exit(0);
    } else {
        char buf[16];
        close(p[1]);
        read(p[0], buf, 16);
        close(p[0]);
        printf("!!!: %s\n", buf);
    }
    return 0;
}
