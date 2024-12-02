#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        exit(1);
    } else if (pid == 0) {
        execlp("ls", "ls", NULL);
        exit(1);
    } else { 
        wait(NULL); 
    }
    return 0;
}
