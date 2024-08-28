#include "proj3.h"
#include <time.h>

int total_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    fclose(file);
    return count;
}

void dis_func(int shm_fd1, int shm_fd2, sem_t *sem_request, sem_t *sem_respond, sem_t *sem_mutex, const char *filename) {
    // Δημιουργία της διεργασίας Server
    if (fork() == 0) {
        server_func(shm_fd1, shm_fd2, sem_request, sem_respond, filename);
        exit(0);
    }

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

    while (1) {
        int line_num;
        struct timespec start, end;
        printf("Please enter a line number to request or -1 to end the process: ");
        scanf("%d", &line_num);
        if(line_num == -1){
            break;
        }
        int num_of_lines = total_lines(filename);
        if(line_num >= num_of_lines || line_num < -1) {
            printf("The total lines of the file are %d, please request a line that exists \n", num_of_lines);
            continue;
        }

        //sem_wait(sem_mutex);
        clock_gettime(CLOCK_MONOTONIC, &start);

        shm_ptr1->line_num = line_num;
        sem_post(sem_request);

        sem_wait(sem_respond);
        printf("Requested line: %s", shm_ptr2->line);
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        double process_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Time taken to receive response: %.7f seconds\n", process_time);
        //sem_post(sem_mutex);
    }
    munmap(shm_ptr1, sizeof(shared_data));
    munmap(shm_ptr2, sizeof(shared_data));
}