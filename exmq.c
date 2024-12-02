#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct msgbuf {
    long type;
    char text[16];
};

int main() {
    key_t key = 1234;
    int msqid = msgget(key, IPC_CREAT | 0666);
    if (fork() == 0) {
        struct msgbuf msg = {1, "Message Queue"};
        msgsnd(msqid, &msg, strlen(msg.text) + 1, 0);
        exit(0);
    } else {
        struct msgbuf msg;
        msgrcv(msqid, &msg, sizeof(msg.text), 0, 0);
        printf("!!!: %s\n", msg.text);
        msgctl(msqid, IPC_RMID, NULL);
    }
    return 0;
}
