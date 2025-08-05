// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

// #include "server_side/ser_customer.h"
// #include "server_side/server_login.h"
#include "server_side/lock_file.h"
#include "server_side/send_transaction.h"
#include "server_side/ser_emp.h"
#include "server_side/ser_manager.h"

#define PORT 8080
#define BUFFER_SIZE 1024

//Structure to hold customer information
struct Customer {
    int id;
    char name[50];
    float balance;
    char password[20];
    int active;
};

// Function to load customer data from a file into memory
int loadCustomerFromFile(int customer_id, struct Customer *customer) {
    char file_path[50];
    sprintf(file_path, "customer_%d.dat", customer_id);

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error opening customer file");
        return -1;
    }

    fread(customer, sizeof(struct Customer), 1, file);
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

// Function to generate the file path for a given customer ID
void getCustomerFilePath(char *path, int customer_id) {
    sprintf(path, "customer_%d.dat", customer_id);
}

// Function to generate the transaction log file path
void getTransactionFilePath(char *path, int customer_id) {
    sprintf(path, "transactions_%d.txt", customer_id);
}

// Function to log transactions
void logTransaction(int customer_id, const char *type, float amount) {
    char transaction_file[50];
    getTransactionFilePath(transaction_file, customer_id);

    FILE *file = fopen(transaction_file, "a");
    if (file == NULL) {
        perror("Error opening transaction log file");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(file, "[%02d-%02d-%04d %02d:%02d:%02d] %s: %.2f\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec, type, amount);
    fclose(file);
}


// Function to save customer data from memory to the file
void saveCustomerToFile(struct Customer *customer) {
    char file_path[50];
    getCustomerFilePath(file_path, customer->id);

    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        perror("Error opening customer file for writing");
        return;
    }

    fwrite(customer, sizeof(struct Customer), 1, file);
    fclose(file);
}




// Server Customer
void server_customer(int sock) {
    // Receive customer ID
    int customer_id;
    
    // Validate the customer ID and password
    int isValid = login_fun(sock,&customer_id);
    send(sock, &isValid, sizeof(isValid), 0);

    if (isValid == 0) {
        // close(sock);  // Close the socket if credentials are invalid
        return;
    }

    struct Customer customer;

    loadCustomerFromFile(customer_id, &customer);

    while(1){
        int choice;
        recv(sock,&choice,sizeof(int),0);
        
        switch (choice) {
            case 1: {
                // View Balance (no locking required, only reading)
                // loadCustomerFromFile(customer_id, &customer);
                char file_path[50];
                sprintf(file_path, "customer_%d.dat", customer_id);
                int fd = open(file_path, O_RDONLY, 0644);
                read(fd, &customer, sizeof(struct Customer));


                float balance = customer.balance;
                
                close(fd);
                send(sock, &balance, sizeof(balance), 0);
                break;
            }
            case 2: {
                // Deposit Money (lock file to prevent race condition)
                float deposit_amount;
                
                recv(sock, &deposit_amount, sizeof(deposit_amount), 0);

                char file_path[50];
                getCustomerFilePath(file_path, customer.id);
            
                // FILE *file = fopen(file_path, "rb+");
                int fd = open(file_path, O_RDWR, 0644);

                lockCustomerFile(fd);  // Lock the file for writing

                // loadCustomerFromFile(customer_id, &customer);
                read(fd, &customer, sizeof(struct Customer));
                customer.balance += deposit_amount;
                saveCustomerToFile(&customer);
                logTransaction(customer.id, "Deposit", deposit_amount);
                
                // Save the updated balance to file
                lseek(fd, 0, SEEK_SET);
                write(fd, &customer, sizeof(struct Customer));

                unlockCustomerFile(fd);  // Unlock the file after writing

                close(fd);

                send(sock, &customer.balance, sizeof(customer.balance), 0);
                
                break;
            }
            case 3: {
                // Withdraw Money (lock file to prevent race condition)
                float withdraw_amount;
                recv(sock, &withdraw_amount, sizeof(withdraw_amount), 0);

                char file_path[50];
                getCustomerFilePath(file_path, customer.id);

                int fd = open(file_path, O_RDWR, 0644);
                lockCustomerFile(fd);  // Lock the file for writing

                // loadCustomerFromFile(customer_id, &customer);
                read(fd, &customer, sizeof(struct Customer));
                if (customer.balance >= withdraw_amount) {
                    customer.balance -= withdraw_amount;
                }

                logTransaction(customer.id, "Withdrawal", withdraw_amount);

                // Save the updated balance to file
                lseek(fd, 0, SEEK_SET);
                write(fd, &customer, sizeof(struct Customer));

                unlockCustomerFile(fd);  // Unlock the file after writing
                close(fd);

                send(sock, &customer.balance, sizeof(customer.balance), 0);
                break;
            }

            case 4: {
                // Transfer Money
                int rec_id;
                float transfer_amt;

                recv(sock, &rec_id, sizeof(rec_id), 0);//recievers id
                recv(sock, &transfer_amt, sizeof(transfer_amt), 0);//amount to be transfered

                char file_path[50];
                char rec_file_path[50];

                getCustomerFilePath(file_path, customer.id);
                getCustomerFilePath(rec_file_path, rec_id);

                int fd = open(file_path, O_RDWR, 0644);
                int rec_fd = open(rec_file_path, O_RDWR , 0644);

                //for reciever
                struct Customer reciever;

                lockCustomerFile(fd);  // Lock the file for writing
                lockCustomerFile(rec_fd);

                read(fd, &customer, sizeof(struct Customer));
                read(rec_fd, &reciever, sizeof(struct Customer));

                if (customer.balance >= transfer_amt) {
                    customer.balance -= transfer_amt;
                    reciever.balance += transfer_amt;
                }

                lseek(fd, 0, SEEK_SET);
                lseek(rec_fd, 0, SEEK_SET);

                // Save the updated balance to file
                write(fd, &customer, sizeof(struct Customer));
                write(rec_fd, &reciever, sizeof(struct Customer));

                logTransaction(customer.id, "Withdrawal", transfer_amt);
                logTransaction(rec_id, "Deposit", transfer_amt);

                
                unlockCustomerFile(fd);  // Unlock the file after writing
                unlockCustomerFile(rec_fd);

                close(fd);
                close(rec_fd);

                send(sock, &customer.balance, sizeof(customer.balance), 0);

                break;
            }
            case 5: {
                // Change password
                char password[20];
                recv(sock, password, sizeof(password), 0);

                char file_path[50];
                getCustomerFilePath(file_path, customer.id);

                int fd = open(file_path, O_RDWR, 0644);
                lockCustomerFile(fd);  // Lock the file for writing

                read(fd, &customer, sizeof(struct Customer));

                strcpy(customer.password,password);

                lseek(fd, 0, SEEK_SET);
                write(fd, &customer, sizeof(struct Customer));

                unlockCustomerFile(fd);  // Unlock the file after writing
                close(fd);
                break;
            }
            case 6: {
                // Transaction History
                char server_reply[BUFFER_SIZE];
                sendTransactionHistory(sock, customer.id);
                break;
            }

            case 7: {
                // Exit
                return;
            }
            default:
                break;
        }
    }

    close(sock);
    return;
}

// Function to handle customer operations
void *handleCustomer(void *socket_desc) {
    int sock = *(int *)socket_desc;
    // int read_size;
    char client_message[BUFFER_SIZE];

/*    // Receive customer ID
    int customer_id;
    // recv(sock, &customer_id, sizeof(int), 0);

    // // Password verification
    // char password[20];
    // recv(sock, password, sizeof(password), 0);

    // Validate the customer ID and password
    int isValid = login_fun(sock,&customer_id);
    send(sock, &isValid, sizeof(isValid), 0);
    printf("isvalid: %d\n",isValid);
    if (isValid == 0) {
        close(sock);  // Close the socket if credentials are invalid
        return NULL;
        printf("bunny\n");
    }

    struct Customer customer;

    loadCustomerFromFile(customer_id, &customer);

    // while ((read_size = recv(sock, client_message, sizeof(client_message), 0)) > 0) {
    while(1){
        int choice;
        recv(sock,&choice,sizeof(int),0);
        
        // sscanf(client_message, "%d", &choice);
        switch (choice) {
            case 1: {
                // View Balance (no locking required, only reading)
                sem_wait(&semaphore);  // Lock for memory access
                loadCustomerFromFile(customer_id, &customer);
                float balance = customer.balance;
                
                printf("%s's balance is: %f\n",customer.name,customer.balance);
                
                send(sock, &balance, sizeof(balance), 0);
                sem_post(&semaphore);  // Unlock after operation
                break;
            }
            case 2: {
                // Deposit Money (lock file to prevent race condition)
                float deposit_amount;
                
                recv(sock, &deposit_amount, sizeof(deposit_amount), 0);

                char file_path[50];
                getCustomerFilePath(file_path, customer.id);

                FILE *file = fopen(file_path, "rb+");

                lockCustomerFile(file);  // Lock the file for writing

                sem_wait(&semaphore);  // Lock for memory access

                loadCustomerFromFile(customer_id, &customer);
                customer.balance += deposit_amount;
                saveCustomerToFile(&customer);
                logTransaction(customer.id, "Deposit", deposit_amount);

                sem_post(&semaphore);  // Unlock after memory modification
                
                // saveCustomerToFile(&customer);  // Save the updated balance to file

                unlockCustomerFile(file);  // Unlock the file after writing

                fclose(file);

                send(sock, &customer.balance, sizeof(customer.balance), 0);
                
                break;
            }
            case 3: {
                // Withdraw Money (lock file to prevent race condition)
                float withdraw_amount;
                recv(sock, &withdraw_amount, sizeof(withdraw_amount), 0);

                char file_path[50];
                getCustomerFilePath(file_path, customer.id);

                FILE *file = fopen(file_path, "rb+");
                lockCustomerFile(file);  // Lock the file for writing

                sem_wait(&semaphore);  // Lock for memory access
                loadCustomerFromFile(customer_id, &customer);
                if (customer.balance >= withdraw_amount) {
                    customer.balance -= withdraw_amount;
                }
                saveCustomerToFile(&customer);
                logTransaction(customer.id, "Withdrawal", withdraw_amount);

                sem_post(&semaphore);  // Unlock after memory modification

                // saveCustomerToFile(&customer);  // Save the updated balance to file

                unlockCustomerFile(file);  // Unlock the file after writing
                fclose(file);

                send(sock, &customer.balance, sizeof(customer.balance), 0);
                break;
            }

            case 4: {
                // Transfer Money
                int rec_id;
                float transfer_amt;

                recv(sock, &rec_id, sizeof(rec_id), 0);//recievers id
                recv(sock, &transfer_amt, sizeof(transfer_amt), 0);//amount to be transfered

                char file_path[50];
                char rec_file_path[50];

                getCustomerFilePath(file_path, customer.id);
                getCustomerFilePath(rec_file_path, rec_id);

                FILE *file = fopen(file_path, "rb+");
                FILE *rec_file = fopen(rec_file_path,"rb+");

                //for reciever
                struct Customer reciever;
                loadCustomerFromFile(rec_id, &reciever);

                lockCustomerFile(file);  // Lock the file for writing
                lockCustomerFile(rec_file);
                sem_wait(&semaphore);  // Lock for memory access

                loadCustomerFromFile(customer_id, &customer);

                if (customer.balance >= transfer_amt) {
                    customer.balance -= transfer_amt;
                    reciever.balance += transfer_amt;
                }

                saveCustomerToFile(&customer);
                saveCustomerToFile(&reciever);

                logTransaction(customer.id, "Withdrawal", transfer_amt);
                logTransaction(rec_id, "Deposit", transfer_amt);

                sem_post(&semaphore);  // Unlock after memory modification
                // saveCustomerToFile(&customer);  // Save the updated balance to file
                unlockCustomerFile(file);  // Unlock the file after writing
                unlockCustomerFile(rec_file);

                fclose(file);
                fclose(rec_file);

                send(sock, &customer.balance, sizeof(customer.balance), 0);

                break;
            }
            case 5: {
                // Transaction History
                char server_reply[BUFFER_SIZE];
                sendTransactionHistory(sock, customer.id);
                break;
            }

            case 6: {
                // Exit
                close(sock);
                return NULL;
            }
            default:
                break;
        }
    }

    // if (read_size == 0) {
    //     puts("Client disconnected");
    // } else if (read_size == -1) {
    //     perror("recv failed");
    // }

    close(sock);
    return NULL;*/

    int choice;
    while (1) {
        // recieve the user's choice from the client
        recv(sock, &choice, sizeof(choice), 0);
        
        switch (choice) {
            case 1: {
                // Customer
                server_customer(sock);
                break;
            }
            case 2: {
                // Bank Employee
                server_employee(sock);
                break;
            }
            case 3: {
                // Manager
                server_manager(sock);
                break;
            }
            case 4: {
                // Administrator
                break;
            }
            case 5: {
                // Exit
                close(sock);
                // exit(0);
                return NULL;
            }

            default:
                break;
        }
    }

}

int main(int argc, char *argv[]) {
    int server_fd, client_sock, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        return 1;
    }

    listen(server_fd, 3);
    printf("Waiting for connections...\n");
    c = sizeof(struct sockaddr_in);

    while ((client_sock = accept(server_fd, (struct sockaddr *)&client, (socklen_t *)&c))) {
        printf("Connection accepted\n");

        if (pthread_create(&thread_id, NULL, handleCustomer, (void *)&client_sock) < 0) {
            perror("could not create thread");
            return 1;
        }

        printf("Handler assigned\n");
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;
}