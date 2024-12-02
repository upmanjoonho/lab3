#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void parent_handler(int signo) {
    printf("부모 핸들러\n");
}

void child_handler(int signo) {
    printf("자식 핸들러\n");
}

int main() {
    pid_t pid = fork();
    struct sigaction act;

    if (pid == 0) { 
        act.sa_handler = child_handler;
        sigaction(SIGUSR1, &act, NULL);

        while (1) {
            sleep(1);
            kill(getppid(), SIGUSR1);
        }
    } else if (pid > 0) { 
        act.sa_handler = parent_handler;
        sigaction(SIGUSR1, &act, NULL);

        while (1) {
            pause();
            kill(pid, SIGUSR1);
        }
    } else {
        perror("fork 실패");
    }

    return 0;
}
