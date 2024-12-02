
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void p(int semid) {
    struct sembuf op = {0, -1, SEM_UNDO};
    semop(semid, &op, 1);
}

void v(int semid) {
    struct sembuf op = {0, 1, SEM_UNDO};
    semop(semid, &op, 1);
}

int main() {
    key_t key = 9012;
    int semid = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 1);
    if (fork() == 0) {
        p(semid);
        printf("Child using semaphore\n");
        sleep(2);
        printf("Child releasing semaphore\n");
        v(semid);
        exit(0);
    } else {
        p(semid);
        printf("Parent using semaphore\n");
        sleep(2);
        printf("Parent releasing semaphore\n");
        v(semid);
        semctl(semid, 0, IPC_RMID, 0);
    }
    return 0;
}
