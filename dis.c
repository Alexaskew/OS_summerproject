#include "proj3.h"
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if(argc !=2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *filename = argv[1];

    int shm_fd1 = shm_open(SHM_NAME1, O_CREAT | O_RDWR, 0666);
    if (shm_fd1 == -1) {
        perror("shm_open shm1");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd1, sizeof(shared_data)) == -1) {
        perror("ftruncate shm1");
        exit(EXIT_FAILURE);
    }

    int shm_fd2 = shm_open(SHM_NAME2, O_CREAT | O_RDWR, 0666);
    if (shm_fd2 == -1) {
        perror("shm_open shm2");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd2, sizeof(shared_data)) == -1) {
        perror("ftruncate shm2");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_client = sem_open("/sem_client", O_CREAT, 0644, 1);
    sem_t *sem_dis = sem_open("/sem_dis", O_CREAT, 0644, 0);
    sem_t *sem_ser = sem_open("/sem_ser", O_CREAT, 0644, 0);
    sem_t *sem_done = sem_open("/sem_done", O_CREAT, 0644, 1);


    if (sem_client == SEM_FAILED || sem_dis == SEM_FAILED || sem_ser == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        server_func(shm_fd2, sem_dis, sem_ser, filename);
        exit(0);
    }

    shared_data *shm_ptr1 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    shared_data *shm_ptr2 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
    if (shm_ptr1 == MAP_FAILED || shm_ptr2 == MAP_FAILED) {
        perror("mmap shm");
        exit(1);
    }

    printf("Dispacher active\n");

    while (1) {
        printf("Dispatcher: Waiting for a client request...\n");
        sem_wait(sem_dis);  // Περιμένει για αίτημα από client

        if (shm_ptr1->line_num == -999) {  
                shm_ptr2->line_num = -999;
                printf("All clients have been served\n");
                sem_post(sem_ser);  
                break;
        }

        printf("Dispatcher:  Recieved a client request!\n");

        shm_ptr2->line_num = shm_ptr1->line_num;
        sem_post(sem_ser);  // Ενημέρωση του server για το αίτημα

        sem_wait(sem_dis);  // Αναμονή για την απάντηση του server

        strcpy(shm_ptr1->line, shm_ptr2->line);
        printf("Dispatcher: Received response from server\n");

        sem_post(sem_client);  // Ενημέρωση του client για την απάντηση
        sem_post(sem_done);
    }

    printf("Dispacher closing\n");
    sem_close(sem_client);
    sem_close(sem_dis);
    sem_close(sem_ser);
    sem_unlink("/sem_client");
    sem_unlink("/sem_dis");
    sem_unlink("/sem_ser");

    munmap(shm_ptr1, sizeof(shared_data));
    munmap(shm_ptr2, sizeof(shared_data));
    shm_unlink(SHM_NAME1);
    shm_unlink(SHM_NAME2);

    exit(0);
}