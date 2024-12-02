#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void execute_command(const char *command) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        char *argv[] = {"sh", "-c", (char *)command, NULL};
        execvp(argv[0], argv);
    } else {
        waitpid(pid, &status, 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    char command[256] = {0};
    for (int i = 1; i < argc; i++) {
        strcat(command, argv[i]);
        if (i < argc - 1) strcat(command, " ");
    }

    execute_command(command);

    return 0;
}
