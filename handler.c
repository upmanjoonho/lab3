#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signo) {
    printf("SIGINT (%d) 종료.\n", signo);
    _exit(0);
}

int main() {
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, NULL);

    while (1) {
        sleep(1);
        printf("실행 중...\n");
    }
}
