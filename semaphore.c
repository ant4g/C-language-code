#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 0x1234
#define NUM_PROCESSES 3
#define NUM_ITERATIONS 5

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void semaphore_lock(int sem_id) {
    struct sembuf sb = {0, -1, 0};
    if (semop(sem_id, &sb, 1) < 0) {
        perror("semop lock");
        exit(EXIT_FAILURE);
    }
}

void semaphore_unlock(int sem_id) {
    struct sembuf sb = {0, 1, 0};
    if (semop(sem_id, &sb, 1) < 0) {
        perror("semop unlock");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int shm_id;
    int sem_id;
    int *shared_counter;
    int *shared_counter2;

    shm_id = shmget(KEY, sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shared_counter = (int *)shmat(shm_id, NULL, 0);
    if (shared_counter == (int *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    *shared_counter = 0;

    shared_counter2 = (int *)shmat(shm_id, NULL, 0);
    if (shared_counter == (int *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    *shared_counter2 = 0;

    sem_id = semget(KEY, 1, IPC_CREAT | 0666);
    if (sem_id < 0) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    union semun sem_union;
    sem_union.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_union) < 0) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid_t pid = fork();
        pid_t child_proces = getpid();
	if (pid == 0) {
            for (int j = 0; j < NUM_ITERATIONS; j++) {
                printf("Wartość pierwszej wspólnej zmiennej: %d\n", *shared_counter);
		printf("Wartość drugiej  wspólnej zmiennej: %d\n", *shared_counter2);
		semaphore_lock(sem_id);
                (*shared_counter)++;
		(*shared_counter2)++;
                printf("Proces %d zwiększył wartość pierwszej zmiennej do %d\n", child_proces, *shared_counter);
		printf("Proces %d zwiększył wartość drugiej  zmiennej do %d\n", child_proces, *shared_counter2);
		semaphore_unlock(sem_id);
                sleep(1);
            }
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    shmdt(shared_counter);
    shmdt(shared_counter2);
    shmctl(shm_id, IPC_RMID, NULL);

    if (semctl(sem_id, 0, IPC_RMID) < 0) {
        perror("semctl remove");
        exit(EXIT_FAILURE);
    }

    return 0;
}
