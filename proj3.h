#ifndef MEMORY_COMM_H
#define MEMORY_COMM_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <ctype.h>

// Μέγεθος διαμοιραζόμενης μνήμης
#define BUF_SIZE 1024
#define N 15

// Ονόματα για τα semaphores
#define SEM_REQUEST_NAME "/sem_request"
#define SEM_RESPONSE_NAME "/sem_response"
#define SEM_MUTEX_NAME "/sem_mutex"

#define SHM_NAME1 "/shm1"
#define SHM_NAME2 "/shm2"

// Δομές δεδομένων
typedef struct {
    int line_num; // Ο αριθμός της γραμμής που ζητάει ο πελάτης
    char line[BUF_SIZE]; // Η γραμμή κειμένου που θα επιστραφεί από τον εξυπηρετητή
} shared_data;

// Δηλώσεις συναρτήσεων
void server_func(int shm_fd1, int shm_fd2, sem_t *sem_request, sem_t *sem_response, const char *filename);
void dis_func(int shm_fd1, int shm_fd2, sem_t *sem_request, sem_t *sem_response, sem_t *sem_mutex,const char *filename);
void client_func(int shm_fd1, int shm_fd2, sem_t *sem_request, sem_t *sem_respond);

#endif // MEMORY_COMM_H