#include "proj3.h"

void client_func(int shm_fd1, int shm_fd2, sem_t *sem_request, sem_t *sem_respond) {
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
        int line_num;
        printf("Client: Please enter a line number to request or -1 to end the process: ");
        scanf("%d", &line_num);
        if(line_num == -1){
            break;
        }
        shm_ptr1->line_num = line_num;
        sem_post(sem_request);
        sem_wait(sem_respond);
        printf("Client received line: %s\n", shm_ptr2->line);
    }
    munmap(shm_ptr1, sizeof(shared_data));
    munmap(shm_ptr2, sizeof(shared_data));
}