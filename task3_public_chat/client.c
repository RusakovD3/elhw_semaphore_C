#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SHM_NAME "/chat_shm"
#define SEM_NAME "/chat_sem"
#define MAX_MESSAGES 100
#define MAX_MESSAGE_LENGTH 256

typedef struct {
    int message_count;
    char messages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
} Chat;

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    Chat *chat = (Chat *)mmap(0, sizeof(Chat), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem = sem_open(SEM_NAME, 0);

    initscr();
    echo();
    cbreak();

    int rows, cols;
    getmaxyx(stdscr, rows, cols); // Получение размеров терминала
    WINDOW *input_win = newwin(3, cols, rows - 3, 0); // Окно для ввода
    WINDOW *chat_win = newwin(rows - 3, cols, 0, 0); // Окно для вывода
    scrollok(chat_win, TRUE);
    int scroll_pos = 0;
    keypad(input_win, TRUE);
    char input_buffer[MAX_MESSAGE_LENGTH];
    while (1) {
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "(press Enter to refresh chat) > ");
        wgetnstr(input_win, input_buffer, MAX_MESSAGE_LENGTH - 1);
        wrefresh(input_win);

        if (strcmp(input_buffer, "exit") == 0) break;
        wclear(input_win);
        sem_wait(sem);
        strcpy(chat->messages[chat->message_count % MAX_MESSAGES], input_buffer);
        chat->message_count++;
        sem_post(sem);


        werase(chat_win);
        int max_messages = (rows - 3 < chat->message_count) ? rows - 3 : chat->message_count;
        for (int i = 0; i < max_messages; i++) {
            int message_index = chat->message_count - max_messages + i + scroll_pos;
            if (message_index >= 0 && message_index < chat->message_count) {
                mvwprintw(chat_win, i, 1, "%s", chat->messages[message_index]);
            }
        }

        wrefresh(chat_win);
    }

    endwin();
    munmap(chat, sizeof(Chat));
    close(shm_fd);
    sem_close(sem);

    return 0;
}
