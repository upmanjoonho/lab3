#include <sys/stat.h>
#include <stdio.h>

int main() {
    struct stat st;
    if (stat("ex.txt", &st) == 0) {
        printf("File size: %ld\n", st.st_size);
    } else {
        perror("stat");
    }
    return 0;
}
