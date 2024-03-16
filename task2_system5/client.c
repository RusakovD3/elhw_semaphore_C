#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    const char *filepath = "/home/dmitr/eltex_hw/elhw_semaphore_C/task2_system5/sharedfile"; // Тот же файл, что и у сервера
    const int project_id = 22; // Тот же идентификатор проекта

    key_t key = ftok(filepath, project_id);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int semid = semget(key, 1, 0666);
    int shmid = shmget(key, 1024, 0666);
    char *message = (char *)shmat(shmid, NULL, 0);

    struct sembuf operation = {0, -1, 0}; // Блокировка

    sprintf(message, "Hi");

    operation.sem_op = 1; // Разблокировка
    semop(semid, &operation, 1);

    operation.sem_op = -1; // Блокировка для чтения ответа от сервера
    sleep(3);

    semop(semid, &operation, 1);

    printf("Сообщение от сервера: %s\n", message);

    shmdt(message);

    return 0;
}
