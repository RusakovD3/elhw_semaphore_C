#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main() {
    // Указывайте свой путь
    const char *filepath = "/home/dmitr/eltex_hw/elhw_semaphore_C/task2_system5/sharedfile";
    const int project_id = 22; // Произвольное число для ftok.

    key_t key = ftok(filepath, project_id);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int semid = semget(key, 1, 0666 | IPC_CREAT);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    char *message = (char *)shmat(shmid, NULL, 0);

    union semun u;
    u.val = 1;
    semctl(semid, 0, SETVAL, u);

    struct sembuf operation = {0, -1, 0}; // Блокировка

    printf("Сервер ожидает клиента.\n");
    sleep(3);

    semop(semid, &operation, 1);

    printf("Сообщение от клиента: %s\n", message);

    // Отправка сообщения клиенту
    sprintf(message, "Hello");

    operation.sem_op = 1; // Разблокировка
    semop(semid, &operation, 1);

    shmdt(message);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, u);

    return 0;
}
