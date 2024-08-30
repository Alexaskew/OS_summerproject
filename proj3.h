#ifndef PROJ3_H
#define PROJ3_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

// Ονόματα για τα semaphores και shared memory
// #define SEM_REQUEST_NAME "/sem_cshm1"
// #define SEM_RESPONSE_NAME "/sem_sshm2"
// #define SEM_MUTEX_NAME "/sem_mutex"
#define SHM_NAME1 "/shm1"
#define SHM_NAME2 "/shm2"

// Μέγεθος διαμοιραζόμενης μνήμης
#define BUF_SIZE 1024
#define N 1  // Αριθμός πελατών

// Δομή δεδομένων που αποθηκεύεται στη shared memory
typedef struct {
    int line_num;         // Ο αριθμός της γραμμής που ζητάει ο πελάτης
    char line[BUF_SIZE];  // Το κείμενο της γραμμής που θα επιστραφεί
} shared_data;

// Δηλώσεις συναρτήσεων
void dis_func(int shm_fd1, int shm_fd2, sem_t *sem_cshm1, sem_t *sem_dshm1, sem_t *sem_dshm2, sem_t *sem_sshm2,const char *filename);
void server_func(int shm_fd2, sem_t *sem_ready, sem_t *sem_response, const char *filename);
void client_func(int client_num, int shm_fd1, sem_t *sem_cshm1, sem_t *sem_dshm1, sem_t *sem_dshm2);

#endif // PROJ3_H