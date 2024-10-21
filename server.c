#include "proj3.h"

void server_func(int shm_fd2, sem_t *sem_dis, sem_t *sem_ser, const char *filename) {
    shared_data *shm_ptr2 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
    if (shm_ptr2 == MAP_FAILED) {
        perror("mmap shm_fd2");
        exit(1);
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    printf("Server active\n");
    while (1) {
        printf("Server: Waiting for a request from the dispatcher...\n");
        sem_wait(sem_ser);  // Περιμένει για αίτημα από τον dispatcher

        if (shm_ptr2->line_num == -999) {  
            break;
        }

        printf("Server: Received a request from the dispatcher!\n");
        int line_num = shm_ptr2->line_num;
        rewind(file);

        char line[1024];
        int curr_line = 0;
        int found = 0;

        while (fgets(line, sizeof(line), file)) {
            if (curr_line == line_num) {
                found = 1;
                break;
            }
            curr_line++;
        }

        if (found == 1) {
            strcpy(shm_ptr2->line, line);
            printf("Server: Line %d found, sending back the result\n", curr_line);
        } else {
            strcpy(shm_ptr2->line, "Line not found, please request a line that exists\n");
            printf("Server: Line %d not found. The Client should request a number from 0 to %d\n", line_num, curr_line - 1);
        }

        sem_post(sem_dis);  // Ενημέρωση του dispatcher ότι το αίτημα ολοκληρώθηκε
    }
    printf("Server closing\n");
    fclose(file);
    munmap(shm_ptr2, sizeof(shared_data));
    exit(0);
}