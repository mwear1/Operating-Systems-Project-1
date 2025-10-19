//  Matthew Wear
//  Operating Systems

#include "producerconsumer.hpp"

const int   SIZE = 256;                     // shared buffer size (bytes)
const char* name = "/pc_shm_test";
const char* SEM_EMPTY = "/sem_Empty";       // counts empty slots (1-slot buffer -> init 1)
const char* SEM_FULL  = "/sem_Full";        // counts full  slots (init 0)

void* produce(void* arg) {
    (void)arg;

    // Minimal reset to start from known-good state
    // Ignore errors if they don't exist yet.
    shm_unlink(name);
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);

    int   shm_fd;
    char* ptr;

    // Create shared memory file
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Size the shared memory
    if (ftruncate(shm_fd, SIZE) == -1) {
        perror("ftruncate");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    // Map shared memory
    ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    std::memset(ptr, 0, SIZE);

    // Create/open semaphores with FRESH initial values 
    sem_t* sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, 1);
    if (sem_empty == SEM_FAILED) {
        perror("sem_open (sem_Empty)");
        munmap(ptr, SIZE);
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    sem_t* sem_full = sem_open(SEM_FULL, O_CREAT, 0666, 0);
    if (sem_full == SEM_FAILED) {
        perror("sem_open (sem_Full)");
        sem_close(sem_empty);
        munmap(ptr, SIZE);
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    // Generate random numbers for produced values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);

    for (int i = 0; i < 10; ++i) {
        // sem_wait blocks until there's an empty slot (NOT busy-wait)
        if (sem_wait(sem_empty) == -1) {
            perror("sem_wait(sem_Empty)");
            break;
        }

        int random_number1 = dis(gen);
        int random_number2 = dis(gen);

        // Safe, bounded write to shared buffer
        std::snprintf(ptr, SIZE, "%d, %d", random_number1, random_number2);
        std::cout << "Produced: " << random_number1 << ", " << random_number2 << std::endl;

        // Signal a full slot is available
        if (sem_post(sem_full) == -1) {
            perror("sem_post(sem_Full)");
            break;
        }

        sleep(1); // simulate work
    }

    // Close handles
    sem_close(sem_empty);
    sem_close(sem_full);
    munmap(ptr, SIZE);
    close(shm_fd);

    return NULL;
}

int main() {
    pthread_t producer_thread;

    if (pthread_create(&producer_thread, NULL, produce, NULL) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    pthread_join(producer_thread, NULL);

    std::cout << "Producer process has terminated." << std::endl;
    return 0;
}
