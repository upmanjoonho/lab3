#include <stdio.h>
#include <stdlib.h>

void my_assert(int condition, const char *message);

int main() {
    int x = 5;

    my_assert(x > 0, "x는 양수여야 합니다.");
    my_assert(x < 0, "x는 음수여야 합니다.");

    printf("x의 값: %d\n", x);
    return 0;
}

void my_assert(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "Assertion failed: %s\n", message);
        exit(EXIT_FAILURE);
    }
}
