#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

#define BUFFER_SIZE 1024

void sendTransactionHistory(int sock, int customer_id) { 
    char transaction_file[50];
    // getTransactionFilePath(transaction_file, customer_id);
    sprintf(transaction_file, "transactions_%d.txt", customer_id);

    FILE *file = fopen(transaction_file, "r");
    if (file == NULL) {
        perror("Error opening transaction log file");
        char error_message[] = "No transaction history available.\n";
        send(sock, error_message, strlen(error_message), 0);
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        line[strlen(line)] = '\0';
        send(sock, line, strlen(line), 0);
    }

    fclose(file);

    // Send an end-of-message signal
    // char end_message[] = "End of transaction history.\n";
    char end_message[] = "\nEnd of transaction history.\n";
    send(sock, end_message, strlen(end_message), 0);
}