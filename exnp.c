#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    char *fifo = "/tmp/myfifo";
    mkfifo(fifo, 0666);
    if (fork() == 0) {
        int fd = open(fifo, O_WRONLY);
        write(fd, "FIFO Message", 12);
        close(fd);
        exit(0);
    } else {
        char buf[12];
        int fd = open(fifo, O_RDONLY);
        read(fd, buf, 12);
        close(fd);
        printf("!!!: %s\n", buf);
        unlink(fifo);
    }
    return 0;
}
