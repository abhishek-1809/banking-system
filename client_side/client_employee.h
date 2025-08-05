#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "client_login.h"
#include "cli_add_customer.h"
#include "modify_customer.h"

// #include "modify_customer.h"
#define PORT 8080
#define BUFFER_SIZE 1024

void emp_menu() {
    printf("\n1. Add New Customer\n");
    printf("2. Modify Customer Detail\n");
    printf("3. Process Loan Application\n");
    printf("4. Approve/Reject Loan\n");
    printf("5. View Assigned Loan Applications\n");
    printf("6. View Customer Transactions\n");
    printf("7. Change Password\n");
    printf("8. Exit\n");

    printf("Enter your choice: ");
}
/*
void add_customer(int sock)
{
    // struct Customer {
    //     int id;
    //     char name[50];
    //     float balance;
    //     char password[20];
    // }new_customer;
    int id;
    char name[50];
    float balance;
    char password[20];

    printf("Enter customer id: ");
    scanf("%d", &id);
    send(sock,&id,sizeof(int),0);

    printf("Enter customer name: ");
    scanf("%s", name);
    send(sock,name,sizeof(name),0);

    printf("Enter customer password: ");
    scanf("%s", password);
    send(sock,password,sizeof(password),0);

    printf("Enter customer account balance: ");
    scanf("%f", &balance);
    send(sock,&balance,sizeof(float),0);

    // send(sock,&new_customer,sizeof(new_customer),0);

    int flag = 0;
    recv(sock, &flag, sizeof(flag), 0);
    if(flag){
        printf("\nNew customer added successfully.\n");
    }
    else{
        printf("\nFailed to add new customer.\n");
    }
}
*/
void employee(int sock) {
    char server_reply[BUFFER_SIZE];
    int emp_id;

    // Receive validation result from server
    login(sock,&emp_id);

    printf("Login successful!\n");

    // Main menu loop
    int emp_choice;
    while (1) {
        emp_menu();
        scanf("%d", &emp_choice);

        // Send the user's choice to the server
        send(sock, &emp_choice, sizeof(emp_choice), 0);
        
        switch (emp_choice) {
            case 1: {
                // Add New Customer
                add_customer(sock);
                break;
            }
            case 2: {
                // Modify Customer Detail
                modify_customer(sock);
                break;
            }
            case 3: {
                // Process Loan Application
                break;
            }
            case 4: {
                // Approve/Reject Loan
                break;
            }
            case 5: {
                // View Assigned Loan Applications
                break;
            }

            case 6: {
                // View Customer Transactions
                break;
            }
            case 7: {
                // Change Password
                char new_password[20];
                printf("Enter new password: ");
                scanf("%s",new_password);
                send(sock,new_password,sizeof(new_password),0);
                printf("Password changed successfully.\n");

                break;
            }
            case 8: {
                // Exit
                printf("Exiting...\n");
                return;
            }
            default:{
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    }

}