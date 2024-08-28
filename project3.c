#include "proj3.h"

int main(int argc, char *argv[]) {
    if(argc !=2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *filename = argv[1];

    int shm_fd1 = shm_open(SHM_NAME1, O_CREAT | O_RDWR, 0666);
    if (shm_fd1 == -1) {
        perror("shm_open shm1");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd1, sizeof(shared_data)) == -1) {
        perror("ftruncate shm1");
        exit(EXIT_FAILURE);
    }

    int shm_fd2 = shm_open(SHM_NAME2, O_CREAT | O_RDWR, 0666);
    if (shm_fd2 == -1) {
        perror("shm_open shm2");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd2, sizeof(shared_data)) == -1) {
        perror("ftruncate shm2");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_request = sem_open(SEM_REQUEST_NAME, O_CREAT, 0644, 0);
    if (sem_request == SEM_FAILED) {
        perror("sem_open request");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_response = sem_open(SEM_RESPONSE_NAME, O_CREAT, 0644, 0);
    if (sem_response == SEM_FAILED) {
        perror("sem_open response");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0644, 1);
    if (sem_mutex == SEM_FAILED) {
        perror("sem_open mutex");
        exit(EXIT_FAILURE);
    }

    dis_func(shm_fd1, shm_fd2, sem_request, sem_response, sem_mutex, filename);

    sem_close(sem_request);
    sem_close(sem_response);
    sem_close(sem_mutex);
    sem_unlink(SEM_REQUEST_NAME);
    sem_unlink(SEM_RESPONSE_NAME);
    sem_unlink(SEM_MUTEX_NAME);


    shm_unlink(SHM_NAME1);
    shm_unlink(SHM_NAME2);

    return 0;
}