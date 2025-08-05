#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

// Lock the file for a specific customer
void lockCustomerFile(int fd) {
    // int fd = fileno(file);
    // flock(fd, LOCK_EX);  // Exclusive lock for writing

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("acquire_write_lock failed");
        exit(EXIT_FAILURE);
    }
}

// Unlock the file for a specific customer
void unlockCustomerFile(int fd) {
    // int fd = fileno(file);
    // flock(fd, LOCK_UN);  // Unlock the file

    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1)
    {
        perror("release_lock failed");
        exit(EXIT_FAILURE);
    }
}