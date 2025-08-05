
// #include "client_login.h"

void manager_menu() {
    printf("\n1. Activate/Deactivate Customer Accounts\n");
    printf("2. Assign Loan Application Processes to Employees\n");
    printf("3. Review Customer Feedback\n");
    printf("4. Change Password\n");
    printf("5. Exit\n");

    printf("Enter your choice: ");
}

void change_active(int sock)
{
    int id;
    int active;
    printf("Enter customer id to be activated/deactivated: ");
    scanf("%d",&id);
    send(sock,&id,sizeof(int),0);

    recv(sock,&active,sizeof(int),0);
    if(active == 1){
        printf("Customer is active enter 0 to deactivate: ");
    }
    else
        printf("Customer is deactive enter 1 to activate: ");

    scanf("%d",&active);

    send(sock,&active,sizeof(int),0);

}

void manager(int sock) {
    int manager_id;

    // Receive validation result from server
    login(sock,&manager_id);

    printf("Login successful!\n");

    // Main menu loop
    int manager_choice;
    while (1) {
        manager_menu();
        scanf("%d", &manager_choice);

        // Send the user's choice to the server
        send(sock, &manager_choice, sizeof(int), 0);
        
        switch (manager_choice) {
            case 1: {
                // Activate/Deactivate Customer Accounts
                change_active(sock);
                break;
            }
            case 2: {
                // Assign Loan Application Processes to Employees
                break;
            }
            case 3: {
                // Review Customer Feedback
                break;
            }
            case 4: {
                // Change Password
                char new_password[20];
                printf("Enter new password: ");
                scanf("%s",new_password);
                send(sock,new_password,sizeof(new_password),0);
                printf("Password changed successfully.\n");
                
                break;
            }
            case 5: {
                // Exit
                printf("Exiting...\n");
                // close(sock);
                return;
            }
            default:{
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    }
}