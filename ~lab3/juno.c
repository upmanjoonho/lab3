#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 1024

void process_command(char *cmd);
void execute_command(char *cmd);
void redirect_output(char *cmd, char *file);
void redirect_input(char *cmd, char *file);
void pipe_command(char *cmd1, char *cmd2);

// exit
void exit_shell() {
    printf("프로그램을 종료합니다.\n");
    exit(0);
}

// 파일 재지향 기능
void redirect_output(char *cmd, char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("파일 열기 실패");
        return;
    }

    dup2(fd, STDOUT_FILENO);  // 표준 출력 재지향
    close(fd);
    execute_command(cmd);  // 재지향된 출력으로 명령어 처리
}

void redirect_input(char *cmd, char *file) {
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("파일 열기 실패");
        return;
    }

    dup2(fd, STDIN_FILENO);  // 표준 입력 재지향
    close(fd);
    execute_command(cmd);  // 재지향된 입력으로 명령어 처리
}

// 파이프 기능
void pipe_command(char *cmd1, char *cmd2) {
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("파이프 생성 실패");
        return;
    }

    if ((pid1 = fork()) == 0) {  // 첫 번째 자식 프로세스
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execute_command(cmd1);
        exit(0);
    }

    if ((pid2 = fork()) == 0) {  // 두 번째 자식 프로세스
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        execute_command(cmd2);
        exit(0);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
}

// ls
void ls_command() {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("디렉토리 열기 실패");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s  ", entry->d_name);
    }
    printf("\n");

    closedir(dir);
}

// pwd
void pwd_command() {
    char cwd[MAX_CMD_LEN];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("현재 디렉토리 확인 실패");
    }
}

// 명령어 실행
void execute_command(char *cmd) {
    char *args[MAX_CMD_LEN / 2 + 1];
    int i = 0;

    args[i] = strtok(cmd, " ");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " ");
    }

    if (execvp(args[0], args) == -1) {
        perror("명령어 실행 실패");
    }
    exit(1);  // 자식 프로세스 종료
}

// 명령어 처리
void process_command(char *cmd) {
    char *pipe_pos = strchr(cmd, '|');
    char *redir_out = strchr(cmd, '>');
    char *redir_in = strchr(cmd, '<');

    if (pipe_pos) {
        *pipe_pos = '\0';
        char *cmd1 = cmd;
        char *cmd2 = pipe_pos + 1;
        pipe_command(cmd1, cmd2);
    } else if (redir_out) {
        *redir_out = '\0';
        char *file = redir_out + 1;
        while (*file == ' ') file++;  // 공백 제거
        redirect_output(cmd, file);
    } else if (redir_in) {
        *redir_in = '\0';
        char *file = redir_in + 1;
        while (*file == ' ') file++;  // 공백 제거
        redirect_input(cmd, file);
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            execute_command(cmd);
        } else {
            wait(NULL);
        }
    }
}

int main() {
    char cmd[MAX_CMD_LEN];

    while (1) {
        printf("myshell> ");
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            perror("입력 실패");
            exit(1);
        }

        cmd[strcspn(cmd, "\n")] = 0;

        if (strcmp(cmd, "exit") == 0) {
            break;
        }

        process_command(cmd);
    }

    return 0;
}
