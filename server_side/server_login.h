
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

#include "structures.h"

#define BUFFER_SIZE 1024

// struct Customer {
//     int id;
//     char name[50];
//     float balance;
//     char password[20];
// };

int loadCustomerFromFile(int customer_id, struct Customer *customer) {
    char file_path[50];
    // getCustomerFilePath(file_path, customer_id);
    sprintf(file_path, "customer_%d.dat", customer_id);

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error opening customer file");
        return -1;
    }

    fread(customer, sizeof(customer), 1, file);
    fclose(file);
    return 0;
}

// Function to validate customer credentials (ID and password)
int validateCustomerCredentials(int customer_id, char *password) {
    struct Customer customer;
    if (loadCustomerFromFile(customer_id, &customer) == -1) {
        return 0;  // File not found or error reading file
    }
    return strcmp(customer.password, password) == 0;
}

int login_fun(int sock, int *id){
    // Recieve customer id
    recv(sock, id, sizeof(int), 0);

    // Password verification
    char password[20];
    recv(sock, password, sizeof(password), 0);

    return validateCustomerCredentials(*id, password);
}