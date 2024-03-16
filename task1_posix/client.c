#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>         
#include <sys/stat.h>      
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define SHM_NAME "/my_shm"
#define SEM_CLIENT "/sem_client"
#define SEM_SERVER "/sem_server"
#define SEM_SIZE 1024

int main() {
    // Открываем разделяемую память
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return EXIT_FAILURE;
    }

    // Отображаем разделяемую память
    char* shm_ptr = mmap(0, SEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    // Открываем семафоры
    sem_t* sem_client = sem_open(SEM_CLIENT, 0);
    sem_t* sem_server = sem_open(SEM_SERVER, 0);
    if (sem_client == SEM_FAILED || sem_server == SEM_FAILED) {
        perror("sem_open");
        return EXIT_FAILURE;
    }

    // Отправляем сообщение серверу
    strcpy(shm_ptr, "Hi");
    sem_post(sem_client); // Уведомляем сервер

    printf("Client sent message and is waiting for server response...\n");
    sem_wait(sem_server); // Ожидаем ответа от сервера
    printf("Client received: %s\n", shm_ptr);

    // Закрытие
    munmap(shm_ptr, SEM_SIZE);
    close(shm_fd);
    sem_close(sem_client);
    sem_close(sem_server);

    return EXIT_SUCCESS;
}
