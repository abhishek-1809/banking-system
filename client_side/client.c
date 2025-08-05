// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "client_customer.h"
#include "client_employee.h"
#include "client_manager.h"


#define PORT 8080
#define BUFFER_SIZE 1024

void client_menu(){
    printf("\n1. Customer\n");
    printf("2. Bank Employee\n");
    printf("3. Manager\n");
    printf("4. Administrator\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int main()
{
    int sock;
    struct sockaddr_in server;
    char server_reply[BUFFER_SIZE];
    int customer_id;
    char password[20];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
        return -1;
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP address
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return -1;
    }
    printf("Connected to server\n");
    while(1) {
        client_menu();
        int cli_choice;
        scanf("%d",&cli_choice);

        send(sock, &cli_choice, sizeof(cli_choice), 0);

        switch (cli_choice)
        {
        case 1:
            customer(sock);
            break;

        case 2:
            employee(sock);
            break;

        case 3:
            manager(sock);
            break;

        case 4:
        //     administrator(sock);
            break;

        case 5:
            //Exit
            close(sock);
            return 0;

        default:
            break;
        }
        
    }
    return 0;
}