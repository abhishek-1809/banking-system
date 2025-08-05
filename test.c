#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

struct Customer {
    int id;
    char name[50];
    // float balance;
    char password[20];
};

int main()
{
    struct Customer c;

    struct Customer *cust = &c;
    c.id = 9;
    strcpy(c.name,"lucky");
    strcpy(c.password,"pass9");
    // c.balance = 33;

    // FILE *file = fopen("manager.dat", "wb");
    int fd = open("manager.dat",O_RDWR|O_CREAT,0644);


    // fwrite(&c, sizeof(struct Customer), 1, file);
    lseek(fd, 0, SEEK_SET);
    write(fd,&c,sizeof(struct Customer));
    printf("Successful.\n");
    close(fd);
    printf("id: %d\n",c.id);
    printf("password: %s\n",c.password);

    return 0;
}

// int main()
// {
//     struct Customer c;
//     struct Customer *cust = &c;
//     printf("Successful 1.\n");
//     FILE *file = fopen("customer_4.dat", "rb");
//     printf("Successful 2.\n");
//     fread(cust, sizeof(struct Customer), 1, file);
//     printf("Successful 3.\n");

//     fclose(file);

//     return 0;
// }

/*int main() {
    struct Customer c;
    struct Customer *cust = &c;

    printf("Successful 1.\n");
    
    FILE *file = fopen("customer_4.dat", "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1; // Exit the program with an error code
    }
    
    printf("Successful 2.\n");
    
    size_t readCount = fread(cust, sizeof(struct Customer), 1, file);
    if (readCount != 1) {
        perror("Error reading customer data");
        fclose(file);
        return 1; // Exit if reading fails
    }

    printf("Successful 3.\n");

    fclose(file);
    return 0;
}*/