#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

#include "server_login.h"
#include "lock_file.h"
#include "logTransaction.h"
#include "send_transaction.h"
#include "save_to_file.h"


sem_t semaphore;
#define BUFFER_SIZE 1024

int loadCustomerFromFile(int customer_id, struct Customer *customer) {
    char file_path[50];
    // getCustomerFilePath(file_path, customer_id);
    sprintf(file_path, "customer_%d.dat", customer_id);

    FILE *file = fopen(file_path, "rb");
    printf("check1\n");
    if (file == NULL) {
        perror("Error opening customer file");
        return -1;
    }

    int readCount = fread(customer, sizeof(struct Customer), 1, file);
    if (readCount != 1) {
        if (feof(file)) {
            fprintf(stderr, "End of file reached while reading customer data\n");
        } else {
            perror("Error reading customer data");
        }
        fclose(file);
        return -1; // Return an error code if reading fails
    }
    fclose(file);
    return 0;
}

void server_customer(int sock) {
    // Receive customer ID
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
        exit(0);
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
                // getCustomerFilePath(file_path, customer.id);
                sprintf(file_path, "customer_%d.dat", customer_id);

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
                // getCustomerFilePath(file_path, customer.id);
                sprintf(file_path, "customer_%d.dat", customer_id);

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

                // getCustomerFilePath(file_path, customer.id);
                sprintf(file_path, "customer_%d.dat", customer_id);
                // getCustomerFilePath(rec_file_path, rec_id);
                sprintf(rec_file_path, "customer_%d.dat", rec_id);

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
                return;
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
    return;
}