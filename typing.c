#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define PASSWORDSIZE 12

int main(void) {
    int fd;
    int nread, cnt = 0, errcnt = 0;
    char ch, text[] = "good afternoon, good evening, and good night";
    struct termios init_attr, new_attr;
    time_t start_time, end_time;

    fd = open(ttyname(fileno(stdin)), O_RDWR);
    tcgetattr(fd, &init_attr);
    new_attr = init_attr;
    new_attr.c_lflag &= ~ICANON;
    new_attr.c_lflag &= ~ECHO; // Echo off to hide user input
    new_attr.c_cc[VMIN] = 1;
    new_attr.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
        fprintf(stderr, "터미널 속성을 설정할 수 없음.\n");
    }

    printf("다음 문장을 그대로 입력하세요.\n%s\n", text);
    start_time = time(NULL);

    while ((nread = read(fd, &ch, 1)) > 0 && ch != '\n') {
        // 사용자가 입력한 문자가 정답과 같으면 그대로 출력
        if (ch == text[cnt]) {
            write(fd, &ch, 1);
            cnt++;
        } else {
            // 잘못 입력한 경우 별표 대신에 문자를 그대로 출력
            write(fd, &ch, 1);
            errcnt++;
            cnt++; // 에러 카운트 증가, 다음 문자로 넘김
        }
    }
    
    end_time = time(NULL);
    double time_taken = difftime(end_time, start_time);
    int typing_speed = (cnt) / (time_taken / 60); // 수정된 입력 문자 수로 타자 속도 계산

    printf("\n타이핑 오류의 횟수는 %d입니다.\n", errcnt);
    printf("평균 분당 타자 수는 %d입니다.\n", typing_speed);

    tcsetattr(fd, TCSANOW, &init_attr);
    close(fd);
    return 0;
}

