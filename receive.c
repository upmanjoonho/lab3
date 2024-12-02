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
    struct msgbuf recv_data;

    if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }

    while (1) {
        if (msgrcv(qid, &recv_data, BUFSIZE, 0, 0) == -1) {
            perror("msgrcv failed");
            exit(1);
        }
        printf("%s\n", recv_data.text);
        if (strcmp(recv_data.text, "exit") == 0) {
            break;
        }
    }

    if (msgctl(qid, IPC_RMID, NULL) == -1) {
        perror("msgctl failed");
        exit(1);
    }

    return 0;
}
