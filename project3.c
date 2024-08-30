#include "proj3.h"
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    const char *filename = "OS.txt";

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

    // Δημιουργία semaphores
    sem_t *sem_cshm1 = sem_open("/sem_cshm1", O_CREAT, 0644, 1);
    sem_t *sem_dshm1 = sem_open("/sem_dshm1", O_CREAT, 0644, 0);
    sem_t *sem_dshm2 = sem_open("/sem_dshm2", O_CREAT, 0644, 0);
    sem_t *sem_sshm2 = sem_open("/sem_sshm2", O_CREAT, 0644, 0);

    if (sem_cshm1 == SEM_FAILED || sem_dshm1 == SEM_FAILED || sem_dshm2 == SEM_FAILED || sem_sshm2 == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Δημιουργία Dispatcher και Server Process
    if (fork() == 0) {
        dis_func(shm_fd1, shm_fd2, sem_cshm1, sem_dshm1, sem_dshm2, sem_sshm2, filename);
        exit(0);
    }

    // Δημιουργία Clients
    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            client_func(i, shm_fd1, sem_cshm1, sem_dshm1, sem_dshm2);
            exit(0);
        }
    }

    // Αναμονή για τις διεργασίες
    for (int i = 0; i < N + 1; i++) {
        wait(NULL);
    }

    // Καθαρισμός των semaphores και shared memory
    sem_close(sem_cshm1);
    sem_close(sem_dshm1);
    sem_close(sem_dshm2);
    sem_close(sem_sshm2);

    sem_unlink("/sem_cshm1");
    sem_unlink("/sem_dshm1");
    sem_unlink("/sem_dshm2");
    sem_unlink("/sem_sshm2");

    shm_unlink(SHM_NAME1);
    shm_unlink(SHM_NAME2);

    return 0;
}