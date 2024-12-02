#include <unistd.h>
#include <stdio.h>

int main() {
    if (access("ex.txt", R_OK) == 0) {
        printf("Readable\n");
    } else {
        perror("access");
    }
    return 0;
}
