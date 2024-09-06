#include "proj3.h"

void dis_func(int shm_fd1, int shm_fd2, sem_t *sem_cshm1, sem_t *sem_dshm1, sem_t *sem_dshm2, sem_t *sem_sshm2, sem_t *sem_done, const char *filename) {
    if (fork() == 0) {
        server_func(shm_fd2, sem_dshm2, sem_sshm2, filename);
        exit(0);
    }

    shared_data *shm_ptr1 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    shared_data *shm_ptr2 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

    printf("Dispacher active\n");
    int termination_count = 0;

    while (1) {
        printf("Dispatcher: Waiting for a client request...\n");
        sem_wait(sem_dshm1);  // Περιμένει για αίτημα από client

        if (shm_ptr1->line_num == -1) {  
            termination_count++;
            if (termination_count == N) {  // Όλοι οι πελάτες τελείωσαν
                shm_ptr2->line_num = -1;
                printf("All clients have been served\n");
                sem_post(sem_sshm2);  
                break;
            } else {
                printf("Client terminated\n");
                sem_post(sem_done);  // Επιτρέπει στους επόμενους πελάτες να τερματίσουν
                continue;
            }
        }

        printf("Dispatcher:  Recieved a client request!\n");

        shm_ptr2->line_num = shm_ptr1->line_num;
        sem_post(sem_sshm2);  // Ενημέρωση του server για το αίτημα

        sem_wait(sem_dshm2);  // Αναμονή για την απάντηση του server

        strcpy(shm_ptr1->line, shm_ptr2->line);
        printf("Dispatcher: Received response from server\n");

        sem_post(sem_cshm1);  // Ενημέρωση του client για την απάντηση

        //sem_post(sem_done);  // Απελευθέρωση για τον επόμενο client
    }

    printf("Dispacher closing\n");
    munmap(shm_ptr1, sizeof(shared_data));
    munmap(shm_ptr2, sizeof(shared_data));
    exit(0);
}