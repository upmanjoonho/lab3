#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 100

void execute_command(char *cmd);
void redirect_output(char *cmd, char *file);
void redirect_input(char *cmd, char *file);
void pipe_command(char *cmd1, char *cmd2);
void process_command(char *cmd);


// 명령어 인자 파싱 함수
int getargs(char *cmd, char **argv)
{
    int narg = 0;
    while (*cmd)
    {
        if (*cmd == ' ' || *cmd == '\t')
            *cmd++ = '\0';
        else
        {
            argv[narg++] = cmd++;
            while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t')
                cmd++;
        }
    }
    argv[narg] = NULL;
    return narg;
}

int go_flag = 0;

// SIGINT 시그널 처리
void signal_exit(int sig)
{
    printf("\n종료\n");
    exit(0);
}

// SIGTSTP 시그널 처리 (일시 중지)
void signal_stop(int sig)
{
    printf("\n일시중지\n");
    go_flag = 0; // 일시 중지 상태
}

// 파일 재지향
void redirect_output(char *cmd, char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("파일 열기 실패");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {  // 자식 프로세스
        dup2(fd, STDOUT_FILENO);  // 표준 출력 재지향
        close(fd);

        // 명령 실행
        execute_command(cmd);

        exit(0);  // 자식 프로세스 종료
    } else {  // 부모 프로세스
        close(fd);
        wait(NULL);  // 자식 프로세스가 끝날 때까지 대기
    }
}

void redirect_input(char *cmd, char *file) {
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("파일 열기 실패");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {  // 자식 프로세스
        dup2(fd, STDIN_FILENO);  // 표준 입력 재지향
        close(fd);

        // 명령 실행
        execute_command(cmd);

        exit(0);  // 자식 프로세스 종료
    } else {  // 부모 프로세스
        close(fd);
        wait(NULL);  // 자식 프로세스가 끝날 때까지 대기
    }
}

// 파이프
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

// ls 명령어 처리 함수
void ls_test()
{
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        perror("디렉토리 열기 실패");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s  ", entry->d_name);
    }
    printf("\n");

    closedir(dir);
}

// pwd 명령어 처리 함수
void pwd_test()
{
    char cwd[MAX_CMD_LEN];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("현재 디렉토리 확인 실패");
    }
}

// cd 명령어 처리 함수
void cd_test(int arg_num, char **arg_list)
{
    const char *home_path = getenv("HOME");
    char current_path[1024];

    if (arg_num == 1)
    {
        if (chdir(home_path) < 0)
        {
            printf("홈 디렉토리 이동 실패\n");
            return;
        }
    }
    else if (strcmp(arg_list[1], "..") == 0)
    {
        if (chdir("..") < 0)
        {
            printf("부모 디렉토리로 이동 실패\n");
            return;
        }
    }
    else
    {
        if (chdir(arg_list[1]) < 0)
        {
            printf("디렉토리 이동 실패\n");
            return;
        }
    }

    if (getcwd(current_path, sizeof(current_path)) == NULL)
    {
        printf("현재 디렉토리 경로 가져오기 실패");
        return;
    }

    printf("디렉토리 변경 성공\n");
    printf("변경된 디렉토리 : %s\n", current_path);
}

// mkdir 명령어 처리 함수
void mkdir_test(int arg_num, char **arg_list)
{
    if (arg_num < 2)
    {
        printf("디렉토리명을 작성하세요\n");
    }
    else
    {
        if (mkdir(arg_list[1], 0755) == -1)
        {
            if (errno == EEXIST)
            {
                printf("디렉토리가 이미 존재\n");
            }
            else
            {
                printf("오류 발생\n");
            }
        }
        else
        {
            printf("디렉토리 생성\n");
        }
    }
}

// rmdir 명령어 처리 함수
void rmdir_test(int arg_num, char **arg_list)
{
    if (arg_num < 2)
    {
        printf("디렉토리 명을 작성하세요\n");
    }
    else
    {
        if (rmdir(arg_list[1]) == -1)
        {
            printf("디렉토리 삭제 실패\n");
        }
        else
        {
            printf("디렉토리 삭제 완료\n");
        }
    }
}

// ln 명령어 처리 함수
void ln_test(int arg_num, char **arg_list)
{
    if (arg_num < 3)
    {
        printf("두 파일명 필요\n");
        return;
    }
    else if (symlink(arg_list[1], arg_list[2]) == -1)
    {
        printf("링크 생성 실패\n");
    }
    else
    {
        printf("링크 생성 성공\n");
        printf("%s -> %s\n", arg_list[2], arg_list[1]);
    }
}

// cp 명령어 처리 함수
void cp_file(char *source, char *destination)
{
    int src_fd = open(source, O_RDONLY);
    if (src_fd == -1)
    {
        perror("source 파일 열기 실패");
        return;
    }

    int dest_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1)
    {
        perror("destination 파일 열기 실패");
        close(src_fd);
        return;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0)
    {
        if (write(dest_fd, buffer, bytes_read) != bytes_read)
        {
            perror("파일 복사 실패");
            break;
        }
    }

    close(src_fd);
    close(dest_fd);
    printf("파일 복사 완료: %s -> %s\n", source, destination);
}

// rm 명령어 처리 함수
void rm_file(char *file)
{
    if (remove(file) == 0)
    {
        printf("파일 삭제 완료: %s\n", file);
    }
    else
    {
        perror("파일 삭제 실패");
    }
}

// mv 명령어 처리 함수
void mv_file(char *source, char *destination)
{
    struct stat dest_stat;
    if (stat(destination, &dest_stat) == 0 && S_ISDIR(dest_stat.st_mode))
    {
        char new_dest[1024];
        snprintf(new_dest, sizeof(new_dest), "%s/%s", destination, basename(source));
        destination = new_dest;
    }

    if (rename(source, destination) == 0)
    {
        printf("파일 이동 완료: %s -> %s\n", source, destination);
    }
    else
    {
        perror("파일 이동 실패");
    }
}

// cat 명령어 처리 함수
void cat_file(char *file)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        perror("파일 열기 실패");
        return;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    close(fd);
}

// 명령어 실행
void execute_command(char *cmd)
{
    char *args[MAX_CMD_LEN / 2 + 1];
    int i = 0;

    args[i] = strtok(cmd, " ");
    while (args[i] != NULL)
    {
        args[++i] = strtok(NULL, " ");
    }

    if (execvp(args[0], args) == -1)
    {
        perror("명령어 실행 실패");
    }
    exit(1); // 자식 프로세스 종료
}

// 백그라운드 명령어 처리 함수
void background_command(char *cmd)
{
    char *args[MAX_ARGS];
    char *token;
    int i = 0;
    int background = 0;

    if (cmd[strlen(cmd) - 1] == '&')
    {
        background = 1;
        cmd[strlen(cmd) - 1] = '\0'; // & 제거
    }

    token = strtok(cmd, " ");
    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("execvp 실패");
            exit(1);
        }
    }
    else if (pid > 0)
    {
        if (background)
        {
            printf("백그라운드에서 실행 중: PID %d\n", pid);
        }
        else
        {
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        perror("fork 실패");
    }
}

// 명령어 처리 함수
void process_command(char *cmd) {
    char *pipe_pos = strchr(cmd, '|');
    char *redir_out = strchr(cmd, '>');
    char *redir_in = strchr(cmd, '<');

    // 파이프 처리
    if (pipe_pos) {
        *pipe_pos = '\0';  // 파이프 앞뒤로 명령어 분리
        char *cmd1 = cmd;
        char *cmd2 = pipe_pos + 1;
        while (*cmd2 == ' ') cmd2++;  // 공백 제거
        pipe_command(cmd1, cmd2);
        return;
    }

    // 출력 리다이렉션 처리
    if (redir_out) {
        *redir_out = '\0';  // 출력 기호로 명령어 분리
        char *file = redir_out + 1;
        while (*file == ' ') file++;  // 공백 제거
        redirect_output(cmd, file);
        return;
    }

    // 입력 리다이렉션 처리
    if (redir_in) {
        *redir_in = '\0';  // 입력 기호로 명령어 분리
        char *file = redir_in + 1;
        while (*file == ' ') file++;  // 공백 제거
        redirect_input(cmd, file);
        return;
    }

    // 내부 명령어 처리
    char *args[MAX_ARGS];
    int narg = getargs(cmd, args);

    if (strcmp(args[0], "ls") == 0) {
        ls_test();
    } else if (strcmp(args[0], "pwd") == 0) {
        pwd_test();
    } else if (strcmp(args[0], "cd") == 0) {
        cd_test(narg, args);
    } else if (strcmp(args[0], "mkdir") == 0) {
        mkdir_test(narg, args);
    } else if (strcmp(args[0], "rmdir") == 0) {
        rmdir_test(narg, args);
    } else if (strcmp(args[0], "ln") == 0) {
        ln_test(narg, args);
    } else if (strcmp(args[0], "cp") == 0) {
        if (narg < 3) {
            printf("cp 명령어는 파일 두 개가 필요합니다.\n");
        } else {
            cp_file(args[1], args[2]);
        }
    } else if (strcmp(args[0], "rm") == 0) {
        if (narg < 2) {
            printf("rm 명령어는 파일명이 필요합니다.\n");
        } else {
            rm_file(args[1]);
        }
    } else if (strcmp(args[0], "mv") == 0) {
        if (narg < 3) {
            printf("mv 명령어는 파일 두 개가 필요합니다.\n");
        } else {
            mv_file(args[1], args[2]);
        }
    } else if (strcmp(args[0], "cat") == 0) {
        if (narg < 2) {
            printf("cat 명령어는 파일명이 필요합니다.\n");
        } else {
            cat_file(args[1]);
        }
    }
    // 외부 명령어 처리
    else {
        pid_t pid = fork();
        if (pid == 0) {  // 자식 프로세스
            execute_command(cmd);
            exit(0);
        } else {  // 부모 프로세스
            wait(NULL);  // 자식 프로세스가 끝날 때까지 대기
        }
    }
}

// main 함수
int main()
{
    char buf[MAX_CMD_LEN];

    struct sigaction sigaction_exit, sigaction_stop;

    sigaction_exit.sa_handler = signal_exit;
    sigfillset(&sigaction_exit.sa_mask);
    sigaction_exit.sa_flags = 0;
    sigaction(SIGINT, &sigaction_exit, NULL);

    sigaction_stop.sa_handler = signal_stop;
    sigfillset(&sigaction_stop.sa_mask);
    sigaction_stop.sa_flags = 0;
    sigaction(SIGTSTP, &sigaction_stop, NULL);

    while (1)
    {
        if (go_flag)
        {
            printf("go\n");
            sleep(2);
        }
        else
        {
            printf("입력> ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';

       if (strcmp(buf, "exit") == 0) {
                printf("프로그램을 종료합니다.\n");
                break;
            }

            process_command(buf);
        }
    }

    return 0;
}
