#include "proj3.h"
#include <time.h>

void client_func(int client_num, int shm_fd1, sem_t *sem_cshm1, sem_t *sem_dshm1, sem_t *sem_dshm2) {
    shared_data *shm_ptr1 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    if (shm_ptr1 == MAP_FAILED) {
        perror("mmap shm_fd1");
        exit(1);
    }

    while (1) {
        int line_num;
        sem_wait(sem_cshm1);  // Περιμένει για τον mutex

        // Ο client εισάγει τον αριθμό γραμμής που θέλει να ζητήσει
        printf("Client %d: Please enter a line number to request or -1 to end the process: ", client_num);
        scanf("%d", &line_num);

        if (line_num == -1) {
            shm_ptr1->line_num = -1;  // Ειδικό μήνυμα τερματισμού
            sem_post(sem_dshm1);  // Σηματοδότηση στον dispatcher
            break;
        }

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Τοποθέτηση του αριθμού γραμμής στη shared memory
        shm_ptr1->line_num = line_num;

        // Κάνει post στον dispatcher για να ξεκινήσει η διαδικασία
        sem_post(sem_dshm1);

        // Αναμονή για την απάντηση από τον dispatcher
        sem_wait(sem_cshm1);

        printf("Client %d received line: %s", client_num, shm_ptr1->line);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Time taken: %.7f seconds\n", time_taken);

        // Απελευθέρωση του mutex για τον επόμενο client
        sem_post(sem_cshm1);
    }

    // Καθαρισμός της shared memory
    munmap(shm_ptr1, sizeof(shared_data));
    exit(0);  // Τερματισμός του client
}