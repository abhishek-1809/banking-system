// customer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void customer_menu() {
    printf("\n1. View Balance\n");
    printf("2. Deposit Money\n");
    printf("3. Withdraw Money\n");
    printf("4. Transfer Money\n");
    printf("5. Change Password\n");
    printf("6. Transaction History\n");
    printf("7. Exit\n");
    printf("Enter your choice: ");
}

void customer(int sock) {

    char server_reply[BUFFER_SIZE];
    int customer_id;
    char password[20];
    
    // Login procedure
    printf("\nEnter your customer ID: ");
    scanf("%d", &customer_id);

    printf("Enter your password: ");
    scanf("%s", password);

    // Send customer ID and password to the server
    send(sock, &customer_id, sizeof(customer_id), 0);
    send(sock, password, sizeof(password), 0);

    // Receive validation result from server
    int isValid;
    recv(sock, &isValid, sizeof(isValid), 0);

    if (isValid == 0) {
        printf("Invalid credentials. Exiting...\n");
        close(sock);
        return;
    }

    printf("Login successful!\n");

    // Main menu loop
    int choice;
    while (1) {
        customer_menu();
        scanf("%d", &choice);

        // Send the user's choice to the server
        send(sock, &choice, sizeof(choice), 0);
        
        switch (choice) {
            case 1: {
                // View balance
                float balance;
                recv(sock, &balance, sizeof(balance), 0);
                printf("Your balance: %.2f\n", balance);
                break;
            }
            case 2: {
                // Deposit money
                float deposit_amount;
                printf("Enter amount to deposit: ");
                scanf("%f", &deposit_amount);

                // Send the deposit amount to the server
                send(sock, &deposit_amount, sizeof(deposit_amount), 0);

                // Receive the updated balance
                float updated_balance;
                recv(sock, &updated_balance, sizeof(updated_balance), 0);

                printf("Deposit successful. Updated balance: %.2f\n", updated_balance);
                break;
            }
            case 3: {
                // Withdraw money
                float withdraw_amount;
                printf("Enter amount to withdraw: ");
                scanf("%f", &withdraw_amount);

                // Send the withdrawal amount to the server
                send(sock, &withdraw_amount, sizeof(withdraw_amount), 0);

                // Receive the updated balance
                float updated_balance;
                recv(sock, &updated_balance, sizeof(updated_balance), 0);
                printf("Withdrawal processed. Updated balance: %.2f\n", updated_balance);
                break;
            }
            case 4: {
                // Transfer money
                int id;
                printf("Enter recipient id:");
                scanf("%d",&id);

                send(sock, &id, sizeof(id), 0);

                float transfer_amt;
                printf("Enter amount to transfer: ");
                scanf("%f", &transfer_amt);

                // Send the withdrawal amount to the server
                send(sock, &transfer_amt, sizeof(transfer_amt), 0);

                // Receive the updated balance
                float updated_balance;
                recv(sock, &updated_balance, sizeof(updated_balance), 0);
                printf("Transfer successfu! Transferred Rs.%f to %d.\n", transfer_amt,id);
                printf("Your updated balance is:%.2f\n", updated_balance);
                break;
            }
            case 5: {
                // Change password
                char password[20];
                printf("Enter your new password: ");
                scanf("%s", password);

                // Send the password to the server
                send(sock, password, sizeof(password), 0);

                printf("Password Change Successfully\n");
                break;
            }
            case 6: {
                // Transaction History
                printf("Transaction history:\n");

                while (recv(sock, server_reply, sizeof(server_reply)-1, 0) > 0) {
                    server_reply[BUFFER_SIZE - 1] = '\0';
                    printf("%s", server_reply);

                    if (strstr(server_reply, "End of transaction history") != NULL) {
                        break;
                    }
                    if (strstr(server_reply, "No transaction history available") != NULL) {
                        break;
                    }
                    
                    for (int i = 0; i < sizeof(server_reply); i++) {
                        server_reply[i] = '\0';
                    }
                }
                break;
            }
            case 7: {
                // Exit
                printf("Exiting...\n");
                // close(sock);
                return;
            }
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    close(sock);
}
