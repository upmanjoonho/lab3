#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t set, oldset;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    printf("SIGINT를 5초 동안 차단합니다.\n");
    sigprocmask(SIG_BLOCK, &set, &oldset);

    sleep(5);

    printf("SIGINT 차단 해제.\n");
    sigprocmask(SIG_UNBLOCK, &set, NULL);

    while (1) {
        sleep(1);
        printf("실행 중...\n");
    }
}

