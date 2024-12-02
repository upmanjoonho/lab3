#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int fd = open("ex.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    close(fd);
    return 0;
}
