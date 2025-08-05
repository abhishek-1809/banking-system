#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

void login(int sock,int *id)
{
    // Login procedure
    char password[20];

    printf("\nEnter your ID: ");
    scanf("%d", id);

    printf("Enter your password: ");
    scanf("%s", password);

    // Send customer ID and password to the server
    send(sock, id, sizeof(int), 0);
    send(sock, password, sizeof(password), 0);

    int isValid;
    recv(sock, &isValid, sizeof(isValid), 0);
    
    if (isValid == 0) {
        printf("Invalid credentials. Exiting...\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
}