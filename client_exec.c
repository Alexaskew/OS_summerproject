#include "proj3.h"
#include <time.h>

int main() {
    int shm_fd1 = shm_open(SHM_NAME1, O_RDWR, 0666);
    if (shm_fd1 == -1) {
        perror("shm_open shm1");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_cshm1 = sem_open("/sem_cshm1", 0);   // mutex για την shared memory
    sem_t *sem_dshm1 = sem_open("/sem_dshm1", 0);   // semaphore για τον dispatcher
    sem_t *sem_done  = sem_open("/sem_done", 0);    // semaphore για συγχρονισμό clients

    if (sem_cshm1 == SEM_FAILED || sem_dshm1 == SEM_FAILED || sem_done == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    shared_data *shm_ptr1 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    if (shm_ptr1 == MAP_FAILED) {
        perror("mmap shm_fd1");
        exit(1);
    }

    printf("Client is running\n");

    while (1) {
        int line_num;
        sem_wait(sem_done);   // Περιμένει να ολοκληρωθεί το προηγούμενο αίτημα
        sem_wait(sem_cshm1);  // Κλείδωμα της shared memory 1

        printf("Client: Please enter a line number to request or -1 to end this client process: ");
        scanf("%d", &line_num);

        if (line_num == -1) {
            shm_ptr1->line_num = -1;
            sem_post(sem_dshm1);  
            sem_post(sem_cshm1);
            printf("Client exits\n");  
            break;
        }

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        shm_ptr1->line_num = line_num;

        sem_post(sem_dshm1);  // Ενημέρωση του dispatcher για το αίτημα

        sem_wait(sem_cshm1);  // Αναμονή για την απάντηση του server

        printf("Client received line: %s", shm_ptr1->line);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Time taken: %.7f seconds\n", time_taken);

        sem_post(sem_cshm1);  // Απελευθέρωση της shared memory 1
        
        sem_post(sem_done);
    }

    munmap(shm_ptr1, sizeof(shared_data));
    sem_close(sem_cshm1);
    sem_close(sem_dshm1);
    sem_close(sem_done);
    exit(0);
}