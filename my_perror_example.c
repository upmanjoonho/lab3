#include <stdio.h>
#include <string.h>
#include <errno.h>

void my_perror(const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}

int main() {
    errno = 0;

    FILE *file = fopen("non_existent_file.txt", "r");
    if (file == NULL) {
        my_perror("파일 열기 오류");
        printf("errno의 값: %d\n", errno);
    } else {
        fclose(file);
    }
    
    return 0;
}

