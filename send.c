#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QKEY (key_t)0111
#define BUFSIZE 128

struct msgbuf {
    long type;
    char text[BUFSIZE];
};

int main() {
    int qid;
    struct msgbuf send_data;
    send_data.type = 1;

    if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }

    printf("메시지: ");
    while (fgets(send_data.text, BUFSIZE, stdin) != NULL) {
        send_data.text[strcspn(send_data.text, "\n")] = '\0';
        if (strcmp(send_data.text, "exit") == 0) {
            break;
        }
        if (msgsnd(qid, &send_data, strlen(send_data.text) + 1, 0) == -1) {
            perror("msgsnd failed");
            exit(1);
        }
        printf("메시지: ");
    }

    return 0;
}

