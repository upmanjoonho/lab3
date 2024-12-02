#include <stdlib.h>
#include <stdio.h>

void exitfunc1(void) {
    printf("1.\n");
}

void exitfunc2(void) {
    printf("2.\n");
}

int main() {
    atexit(exitfunc1);
    atexit(exitfunc2);

    return 0;
}
