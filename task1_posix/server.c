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
    // Создаем разделяемую память
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return EXIT_FAILURE;
    }
    ftruncate(shm_fd, SEM_SIZE);

    // Отображаем разделяемую память в адресное пространство процесса
    char* shm_ptr = mmap(0, SEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    // Создаем семафоры
    sem_t* sem_client = sem_open(SEM_CLIENT, O_CREAT, 0666, 0);
    sem_t* sem_server = sem_open(SEM_SERVER, O_CREAT, 0666, 0);
    if (sem_client == SEM_FAILED || sem_server == SEM_FAILED) {
        perror("sem_open");
        return EXIT_FAILURE;
    }

    printf("Server is waiting for client message...\n");
    sem_wait(sem_client); // Ожидаем сообщение от клиента
    printf("Server received: %s\n", shm_ptr);

    // Ответ клиенту
    strcpy(shm_ptr, "Hello");
    sem_post(sem_server); // Уведомляем клиента

    // Закрытие и очистка
    munmap(shm_ptr, SEM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    sem_close(sem_client);
    sem_close(sem_server);
    sem_unlink(SEM_CLIENT);
    sem_unlink(SEM_SERVER);

    return EXIT_SUCCESS;
}
