#include <fcntl.h>
#include <unistd.h>

int main() {
    char buf[128];
    int fd = open("ex.txt", O_RDONLY);
    ssize_t bytesRead = read(fd, buf, sizeof(buf));
    write(STDOUT_FILENO, buf, bytesRead);
    close(fd);
    return 0;
}

