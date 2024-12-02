#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SHMSIZE 1024
#define SHMKEY  (key_t)1234

int main() {
    int shmid;
    void *shmaddr;

    if ((shmid = shmget(SHMKEY, SHMSIZE, IPC_CREAT | 0666)) == -1) {
        perror("shmget failed");
        exit(1);
    }

    if ((shmaddr = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    if (fork() == 0) {
        strcpy((char *)shmaddr, "Shared Memory");
        shmdt(shmaddr);
        exit(0);
    } else {
        wait(NULL);
        printf("!!!: %s\n", (char *)shmaddr);
        shmdt(shmaddr);
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl failed");
            exit(1);
        }
    }
    return 0;
}

