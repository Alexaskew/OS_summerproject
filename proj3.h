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

// Ονόματα shared memory
#define SHM_NAME1 "/shm1"
#define SHM_NAME2 "/shm2"

#define BUF_SIZE 1024

typedef struct {
    int line_num;         // Ο αριθμός της γραμμής που ζητάει ο πελάτης
    char line[BUF_SIZE];  // Το κείμενο της γραμμής που θα επιστραφεί
} shared_data;

// Δηλώσεις συναρτήσεων
void server_func(int shm_fd2, sem_t *sem_dis, sem_t *sem_ser, const char *filename);

#endif // PROJ3_H