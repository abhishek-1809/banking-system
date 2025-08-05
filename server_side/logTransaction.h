#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

// Function to log transactions
void logTransaction(int customer_id, const char *type, float amount) {
    char transaction_file[50];
    // getTransactionFilePath(transaction_file, customer_id);
    sprintf(transaction_file, "transactions_%d.txt", customer_id);

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