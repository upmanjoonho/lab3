#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) { 
        sleep(4);
        exit(5);
    }

    if (wait(&status) == -1) {
        perror("wait failed");
        exit(2);
    }
    
    if (WIFEXITED(status)) {
        printf("%d, %d\n", pid, WEXITSTATUS(status));
    }
    return 0;
}
