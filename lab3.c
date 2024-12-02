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
/usr/bin/ld: cannot open output file lab3: Is a directory
collect2: error: ld returned 1 exit status    int fd = open(file, O_RDONLY);
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

// 백그라운드 명령어 처리 함수
void execute_command(char *cmd)
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
void process_command(char *cmd)
{
    if (strcmp(cmd, "go") == 0)
    {
        go_flag = 1;
    }
    else if (strcmp(cmd, "ls") == 0)
    {
        ls_test();
    }
    else if (strcmp(cmd, "pwd") == 0)
    {
        pwd_test();
    }
    else if (strncmp(cmd, "cd", 2) == 0)
    {
        char *args[MAX_ARGS];
        int narg = getargs(cmd, args);
        cd_test(narg, args);
    }
    else if (strncmp(cmd, "mkdir", 5) == 0)
    {
        char *args[MAX_ARGS];
        int narg = getargs(cmd, args);
        mkdir_test(narg, args);
    }
    else if (strncmp(cmd, "rmdir", 5) == 0)
    {
        char *args[MAX_ARGS];
        int narg = getargs(cmd, args);
        rmdir_test(narg, args);
    }
    else if (strncmp(cmd, "ln", 2) == 0)
    {
        char *args[MAX_ARGS];
        int narg = getargs(cmd, args);
        ln_test(narg, args);
    }
    else if (strncmp(cmd, "cp", 2) == 0)
    {
        char *args[MAX_ARGS];
        int narg = getargs(cmd, args);
        cp_file(args[1], args[2]);
    }
    else if (strncmp(cmd, "rm", 2) == 0)
    {
        char *args[MAX_ARGS];
        int narg = getargs(cmd, args);
        rm_file(args[1]);
    }
    else if (strncmp(cmd, "mv", 2) == 0)
    {
        char *args[MAX_ARGS];
        int narg = getargs(cmd, args);
        mv_file(args[1], args[2]);
    }
    else if (strncmp(cmd, "cat", 3) == 0)
    {
        char *args[MAX_ARGS];
        int narg = getargs(cmd, args);
        cat_file(args[1]);
    }
    else
    {
        execute_command(cmd);
    }
}

// main 함수
int main()
{
    char buf[MAX_CMD_LEN];
    char *argv[MAX_ARGS];
    int narg;
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
            narg = getargs(buf, argv);

            process_command(buf);
        }
    }

    return 0;
}
