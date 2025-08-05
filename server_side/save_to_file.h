#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

struct Customer {
    int id;
    char name[50];
    float balance;
    char password[20];
};

// Function to save customer data from memory to the file
void saveCustomerToFile(struct Customer *customer) {
    char file_path[50];
    // getCustomerFilePath(file_path, customer->id);
    sprintf(file_path, "customer_%d.dat", customer->id);

    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        perror("Error opening customer file for writing");
        return;
    }

    fwrite(customer, sizeof(struct Customer), 1, file);
    fclose(file);
}