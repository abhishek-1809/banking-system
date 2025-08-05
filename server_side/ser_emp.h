#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/types.h>
#define BUFFER_SIZE 1024

// #include "server_login.h"
// #include "structures.h"
#include "ser_modify_customer.h"
#include "ser_changeemp_pass.h"

struct New_Customer {
    int id;
    char name[50];
    float balance;
    char password[20];
    int active;
};

struct Employee
{
    char name[50];
    int id;
    char password[20];
};


int loadEmployeeFromFile(int emp_id, struct Employee *emp) {
    char file_path[50];
    sprintf(file_path, "employee_%d.dat", emp_id);

    int fd = open(file_path, O_RDWR, 0644);
    if (fd < 0) {
        perror("Error opening employee file");
        return -1;
    }

    read(fd,emp, sizeof(struct Employee));
    close(fd);
    return 0;
}

// Function to validate customer credentials (ID and password)
int validateEmployeeCredentials(int emp_id, char *password) {
    struct Employee emp;
    if (loadEmployeeFromFile(emp_id, &emp) == -1) {
        return 0;  // File not found or error reading file
    }
    return strcmp(emp.password, password) == 0;
}

int login_fun_emp(int sock, int *id){
    // Recieve customer id
    recv(sock, id, sizeof(int), 0);

    // Password verification
    char password[20];
    recv(sock, password, sizeof(password), 0);

    return validateEmployeeCredentials(*id, password);
}

//Add new customer
void add_customer(int sock){

    struct New_Customer c;
    
    recv(sock,&c.id,sizeof(int),0);
    recv(sock,c.name,sizeof(c.name),0);
    recv(sock,c.password,sizeof(c.password),0);
    recv(sock,&c.balance,sizeof(float),0);
    c.active = 1;

    char file_path[50];
    sprintf(file_path, "customer_%d.dat", c.id);

    // FILE *file = fopen(file_path, "wb");
    int fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    int flag = 0;
    if (fd == -1) {
        // If open() fails, print an error and return a failure flag to the client
        perror("Error opening file");
        send(sock, &flag, sizeof(flag), 0);
        return;
    }


    struct New_Customer *temp = &c;
    
    if (write(fd, &c, sizeof(struct New_Customer)) == sizeof(struct New_Customer)) {
        flag = 1;  // Write success
    }

    send(sock,&flag,sizeof(flag),0);

    close(fd);
}

// Employee
void server_employee(int sock) {
    int emp_id;
    int isValid = login_fun_emp(sock,&emp_id);
    send(sock, &isValid, sizeof(isValid), 0);

    int emp_choice;
    while (1) {
        // Send the user's choice to the server
        recv(sock, &emp_choice, sizeof(emp_choice), 0);
        
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
                change_emp_pass(sock, emp_id);
                break;
            }
            case 8: {
                // Exit
                printf("Exiting...\n");
                return;
            }
            default: {
                printf("%d  Invalid choice. Please try again.\n",emp_choice);
                return;
            }
        }
    }
}