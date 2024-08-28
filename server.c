#include "proj3.h"

void server_func(int shm_fd1, int shm_fd2, sem_t *sem_request, sem_t *sem_respond, const char *filename) {
    shared_data *shm_ptr1 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    if (shm_ptr1 == MAP_FAILED) {
        perror("mmap shm_fd1");
        exit(1);
    }
    shared_data *shm_ptr2 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
    if (shm_ptr2 == MAP_FAILED) {
        perror("mmap shm_fd1");
        exit(1);
    }

    while(1) {
        sem_wait(sem_request);
        int line_num = shm_ptr1->line_num;

        FILE *file = fopen(filename, "r");
        if (!file) {
            perror("fopen");
            exit(1);
        }

        char line[256];
        int curr_line = 0;

        while(fgets(line, sizeof(line), file)) {
            if(curr_line == line_num) {
                break;
            }
            curr_line++;
        }
        fclose(file);
        strcpy(shm_ptr2->line, line);
        sem_post(sem_respond);
    }
    munmap(shm_ptr1, sizeof(shared_data));
    munmap(shm_ptr2, sizeof(shared_data));
}