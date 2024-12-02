#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s src_file dest_file\n", argv[0]);
        return 1;
    }
    
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        perror("open src");
        return 1;
    }

    int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("open dest");
        close(src_fd);
        return 1;
    }

    char buf[BUF_SIZE];
    ssize_t bytes;
    while ((bytes = read(src_fd, buf, BUF_SIZE)) > 0) {
        if (write(dest_fd, buf, bytes) != bytes) {
            perror("write");
            close(src_fd);
            close(dest_fd);
            return 1;
        }
    }

    close(src_fd);
    close(dest_fd);
    return 0;
}
