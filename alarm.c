#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void alarm_handler(int sig) {
    printf("SIGALRM 시그널을 받음\n");
}

int main() {
    struct sigaction act;
    act.sa_handler = alarm_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGALRM, &act, NULL);

    printf("5초 후에 알람 시그널이 발생\n");
    alarm(5);

    pause();

    printf("프로그램을 종료\n");
    return 0;
}
