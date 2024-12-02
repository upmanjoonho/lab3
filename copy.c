#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SHMSIZE 1024
#define SHMKEY (key_t)0111
#define SEMKEY (key_t)0222

void sem_op(int semid, int semnum, int op) {
    struct sembuf sb;
    sb.sem_num = semnum;
    sb.sem_op = op;
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        exit(1);
    }

    int shmid, semid, src_fd, dest_fd;
    char *shmaddr;
    pid_t pid;

    shmid = shmget(SHMKEY, SHMSIZE, IPC_CREAT | 0666);
    semid = semget(SEMKEY, 2, IPC_CREAT | 0666);

    if (shmid == -1 || semid == -1) {
        exit(1);
    }

    semctl(semid, 0, SETVAL, 1);
    semctl(semid, 1, SETVAL, 0);

    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1) {
        exit(1);
    }

    src_fd = open(argv[1], O_RDONLY);
    dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (src_fd == -1 || dest_fd == -1) {
        exit(1);
    }

    pid = fork();
    if (pid == -1) {
        exit(1);
    }

    if (pid == 0) {
        while (1) {
            sem_op(semid, 1, -1);
            if (strlen(shmaddr) == 0) break;
            write(dest_fd, shmaddr, strlen(shmaddr));
            sem_op(semid, 0, 1);
        }
        close(dest_fd);
        exit(0);
    } else {
        ssize_t bytes_read;
        while ((bytes_read = read(src_fd, shmaddr, SHMSIZE - 1)) > 0) {
            shmaddr[bytes_read] = '\0';
            sem_op(semid, 1, 1);
            sem_op(semid, 0, -1);
        }
        shmaddr[0] = '\0';
        sem_op(semid, 1, 1);
        wait(NULL);
        close(src_fd);

        shmdt(shmaddr);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
    }

    return 0;
}

