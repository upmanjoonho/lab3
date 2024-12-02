#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd[2];
    pid_t pid;
    char readbuffer[80];

    pipe(fd);

    pid = fork();

    if (pid > 0) {
        close(fd[0]);
        write(fd[1], "Hello World", 14);
        close(fd[1]);
    } else {
        close(fd[1]);
        read(fd[0], readbuffer, sizeof(readbuffer));
        printf("parent: %s\n", readbuffer);
        close(fd[0]);
    }
    
    return 0;
}
