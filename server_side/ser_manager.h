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

struct Active_Customer {
    int id;
    char name[50];
    float balance;
    char password[20];
    int active;
};

struct Manager
{
    char name[50];
    int id;
    char password[20];
};

int loadManagerFromFile(int mng_id, struct Manager *mng) {
    char file_path[50];
    // getCustomerFilePath(file_path, customer_id);
    // sprintf(file_path, "manager.dat", mng_id);

    // FILE *file = fopen(file_path, "rb");
    int fd = open("manager.dat", O_RDONLY, 0644);
    if (fd < 0) {
        perror("Error opening manager file");
        return -1;
    }
    printf("password in loadmng: %s\n",mng->name);

    read(fd,mng, sizeof(struct Manager));
    close(fd);
    return 0;
}

// Function to validate customer credentials (ID and password)
int validateManagerCredentials(int mng_id, char *password) {
    struct Manager mng;
    memset(&mng, 0, sizeof(struct Manager));
    if (loadManagerFromFile(mng_id, &mng) == -1) {
        printf("Error loading file.");
        return 0;  // File not found or error reading file
    }
    printf("file checked entered id: %d & emp.id: %d\n",mng_id,mng.id);
    printf("file checked entered pass: %s & emp.password: %s\n\n\n",password,mng.password);

    return strcmp(mng.password, password) == 0;
}

int login_fun_mng(int sock, int *id){
    // Recieve customer id
    recv(sock, id, sizeof(int), 0);

    // Password verification
    char password[20];
    recv(sock, password, sizeof(password), 0);

    return validateManagerCredentials(*id, password);
}

// activate/deactivate
void change_active(int sock)
{
    struct Active_Customer c;
    char file_path[50];
    int cust_id;
    recv(sock,&cust_id,sizeof(int),0);
    sprintf(file_path,"customer_%d.dat",cust_id);

    int fd = open(file_path,O_RDWR,0644);
    read(fd, &c, sizeof(struct Active_Customer));

    send(sock,&c.active,sizeof(int),0);

    lockCustomerFile(fd); 

    recv(sock,&c.active,sizeof(int),0);
    
    lseek(fd, 0, SEEK_SET);
    write(fd,&c,sizeof(struct Active_Customer));

    unlockCustomerFile(fd);  

    close(fd);
}

//change manager password
void change_mng_pass(int sock,int mng_id){

    // char file_path[20];
    // sprintf(file_path,"manager.dat", mng_id);
    struct Manager mngp;
    char new_password[20];
    recv(sock, new_password, sizeof(new_password), 0);

    // FILE *file = fopen(file_path, "rb+");
    int fd = open("manager.dat", O_RDWR, 0644);
    lockCustomerFile(fd);  // Lock the file for writing

    read(fd, &mngp, sizeof(struct Manager));

    strcpy(mngp.password,new_password);

    lseek(fd, 0, SEEK_SET);
    write(fd, &mngp, sizeof(struct Manager));

    unlockCustomerFile(fd);  // Unlock the file after writing
    close(fd);
}

// Manager
void server_manager(int sock) {
    int mng_id;
    int isValid = login_fun_mng(sock,&mng_id);
    send(sock, &isValid, sizeof(isValid), 0);

    int mng_choice;
    while (1) {
        // Send the user's choice to the server
        recv(sock, &mng_choice, sizeof(mng_choice), 0);
        
        switch (mng_choice) {
            case 1: {
                // // Activate/Deactivate Customer Accounts
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
                change_mng_pass(sock, mng_id);
                break;
            }
            case 5: {
                // Exit
                printf("Exiting...\n");
                return;
            }
            default:
                printf("%d  Invalid choice. Please try again.\n",mng_choice);
                // exit(0);
                return;
        }
    }
}