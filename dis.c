#include "proj3.h"

void dis_func(int shm_fd1, int shm_fd2, sem_t *sem_cshm1, sem_t *sem_dshm1, sem_t *sem_dshm2, sem_t *sem_sshm2, const char *filename) {
    // Δημιουργία διεργασίας Server
    if (fork() == 0) {
        server_func(shm_fd2, sem_dshm2, sem_sshm2, filename);
        exit(0);
    }

    shared_data *shm_ptr1 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    shared_data *shm_ptr2 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

    while (1) {
        sem_wait(sem_dshm1);  // Περιμένει για αίτημα από client

        if (shm_ptr1->line_num == -1) {  // Έλεγχος για το ειδικό μήνυμα τερματισμού
            shm_ptr2->line_num = -1;
            sem_post(sem_sshm2);  // Σηματοδότηση στον server για τερματισμό
            break;
        }

        // Μεταφορά αιτήματος από shm1 σε shm2 για τον server
        shm_ptr2->line_num = shm_ptr1->line_num;
        sem_post(sem_sshm2);  // Σήμα στον server να διαβάσει το αίτημα από shm2

        // Περιμένει την απάντηση από τον server
        sem_wait(sem_dshm2);

        // Μεταφορά της απάντησης από το shm2 στο shm1
        strcpy(shm_ptr1->line, shm_ptr2->line);

        // Σήμα στον client ότι η απάντηση είναι έτοιμη στο shm1
        sem_post(sem_cshm1);
    }

    // Περιμένουμε τον server να τερματίσει
    sem_wait(sem_dshm2);

    munmap(shm_ptr1, sizeof(shared_data));
    munmap(shm_ptr2, sizeof(shared_data));
    exit(0);  // Τερματισμός του dispatcher
}