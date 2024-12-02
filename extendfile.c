#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("ex.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (lseek(fd, 1024, SEEK_END) == -1) {
        perror("lseek");
        close(fd);
        return 1;
    }

    if (write(fd, "", 1) == -1) {  // EOF 작성
        perror("write");
    }

    close(fd);
    return 0;
}
