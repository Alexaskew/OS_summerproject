#include "proj3.h"
#include <time.h>

int main() {
    int shm_fd1 = shm_open(SHM_NAME1, O_RDWR, 0666);
    if (shm_fd1 == -1) {
        perror("shm_open shm1");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_client = sem_open("/sem_client", 0);   // mutex για την shared memory
    sem_t *sem_dis = sem_open("/sem_dis", 0);   // semaphore για τον dispatcher
    sem_t *sem_done = sem_open("/sem_done", 0);

    if (sem_client == SEM_FAILED || sem_dis == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    shared_data *shm_ptr1 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    if (shm_ptr1 == MAP_FAILED) {
        perror("mmap shm_fd1");
        exit(1);
    }

    printf("Client is running\n");

    char input[64];
    int line_num;

    printf("Client: Please enter a non negative line number to request or type 'exit' to end the dispatcher and server processes: ");
    scanf("%s", input);

    if (strcmp(input, "exit") == 0) {
        line_num = -999;        //Χρησιμοποιουμε το -999 ως κωδικο για τερματισμο
    } else {
        line_num = atoi(input);  // Μετατροπή του input σε αριθμό γραμμής
    }

    sem_wait(sem_done);
    sem_wait(sem_client);  // Κλείδωμα της shared memory 1

    if(shm_ptr1->line_num == -999) {      //Ελεγχος αν καποιος αλλος πελατης εχει τερματισει τις αλλες διεργασιες
        printf("Another client has already terminated the dispatcher and the server!\n");
        line_num = -999;
    }

    if (line_num == -999) {
        shm_ptr1->line_num = -999;
        sem_post(sem_dis);  
        sem_post(sem_client);
        printf("Client exits\n");
    } else {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        shm_ptr1->line_num = line_num;
        sem_post(sem_dis);  // Ενημέρωση του dispatcher για το αίτημα

        sem_wait(sem_client);  // Αναμονή για την απάντηση του server
        printf("Client received line: %s", shm_ptr1->line);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Time taken: %.7f seconds\n", time_taken);

        sem_post(sem_client);  // Απελευθέρωση της shared memory 1
    }

    munmap(shm_ptr1, sizeof(shared_data));
    sem_close(sem_client);
    sem_close(sem_dis);
    exit(0);
}