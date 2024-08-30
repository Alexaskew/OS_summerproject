#include "proj3.h"

void server_func(int shm_fd2, sem_t *sem_dshm2, sem_t *sem_sshm2, const char *filename) {
    shared_data *shm_ptr2 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

    while (1) {
        sem_wait(sem_sshm2);  // Περιμένει για αίτημα από τον dispatcher

        if (shm_ptr2->line_num == -1) {  // Έλεγχος για το ειδικό μήνυμα τερματισμού
            sem_post(sem_dshm2);  // Σηματοδότηση στον dispatcher ότι ο server τερματίζει
            break;
        }

        int line_num = shm_ptr2->line_num;

        // Ανοίγουμε το αρχείο για να διαβάσουμε τη γραμμή που ζήτησε ο client
        FILE *file = fopen(filename, "r");
        if (!file) {
            perror("fopen");
            exit(1);
        }

        char line[256];
        int curr_line = 0;
        int found = 0;

        // Ανάγνωση του αρχείου γραμμή-γραμμή
        while (fgets(line, sizeof(line), file)) {
            if (curr_line == line_num) {
                found = 1;
                break;
            }
            curr_line++;
        }
        fclose(file);

        // Γράφουμε την απάντηση στη shared memory shm2
        if (found == 1) {
            strcpy(shm_ptr2->line, line);
        } else {
            strcpy(shm_ptr2->line, "Line not found\n");
        }

        // Σήμα στον dispatcher ότι το αίτημα διεκπεραιώθηκε
        sem_post(sem_dshm2);
    }

    munmap(shm_ptr2, sizeof(shared_data));
    exit(0);  // Τερματισμός του server
}