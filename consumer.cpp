//  Matthew Wear
//  Operating Systems

#include "producerconsumer.hpp"

// Shared memory size and names (match the producer)
const int   SIZE = 256;
const char* name = "/pc_shm_test";   
const char* SEM_EMPTY = "/sem_Empty";   // counts empty slots (1-slot buffer -> init 1)
const char* SEM_FULL  = "/sem_Full";    // counts full  slots (init 0)

// Open a named semaphore with brief retries in case producer creates it slightly later
static sem_t* open_sem_with_retry(const char* sem_name, int tries = 50, useconds_t us = 100'000) {
    for (int i = 0; i < tries; ++i) {
        sem_t* s = sem_open(sem_name, 0);
        if (s != SEM_FAILED) return s;
        if (errno != ENOENT) break;
        usleep(us);
    }
    return SEM_FAILED;
}

void* consume(void* arg) {
    (void)arg;

    int shm_fd;
    char* ptr;

    // Opens the shared memory file
    shm_fd = shm_open(name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return NULL;
    }

    // Maps the shared memory object into the process's address space
    ptr = (char*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return NULL;
    }

    // Opens the semaphores
    sem_t* sem_empty = open_sem_with_retry(SEM_EMPTY);
    sem_t* sem_full  = open_sem_with_retry(SEM_FULL);

    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
        perror("sem_open");
        if (sem_empty != SEM_FAILED) sem_close(sem_empty);
        if (sem_full  != SEM_FAILED) sem_close(sem_full);
        munmap(ptr, SIZE);
        close(shm_fd);
        return NULL;
    }

    for (int i = 0; i < 10; ++i) {
        // sem_wait blocks until sem_full > 0 (NOT busy-wait)
        if (sem_wait(sem_full) == -1) {
            perror("sem_wait(sem_Full)");
            break;
        }

        printf("Consumed: %s\n", ptr);

        memset(ptr, 0, SIZE); // Clears the shared memory after consuming

        if (sem_post(sem_empty) == -1) {
            perror("sem_post(sem_Empty)");
            break;
        }

        sleep(1); // Simulate some work
    }

    // Cleans up and closes everything
    sem_close(sem_empty);
    sem_close(sem_full);

    munmap(ptr, SIZE);
    close(shm_fd);

    // Unlink IPC objects once here (choose one side to unlink; this consumer does it)
    if (shm_unlink(name) == -1) {
        perror("shm_unlink");
    }
    if (sem_unlink(SEM_EMPTY) == -1) {
        perror("sem_unlink(/sem_Empty)");
    }
    if (sem_unlink(SEM_FULL) == -1) {
        perror("sem_unlink(/sem_Full)");
    }

    return NULL;
}

int main() {
    pthread_t consumer_thread;

    // Creates consumer thread
    if (pthread_create(&consumer_thread, NULL, consume, NULL) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    // Waits for consumer thread to finish
    pthread_join(consumer_thread, NULL);

    std::cout << "Consumer process has terminated." << std::endl;
    return 0;
}
