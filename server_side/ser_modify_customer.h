// #include "lock_file.h"

struct Mod_Customer
{
    int id;
    char name[50];
    float balance;
    char password[20];
    int active;
};

void modify_customer(int sock)
{
    struct Mod_Customer c;
    int customer_id;
    char file_path[50];
    recv(sock,&customer_id,sizeof(int),0);
    sprintf(file_path,"customer_%d.dat",customer_id);

    int emp_choice;
    recv(sock, &emp_choice, sizeof(int),0);
    switch (emp_choice)
    {
        case 1:{//Change Customer id
            int new_id;
            recv(sock, &new_id, sizeof(int), 0);

            int fd = open(file_path, O_RDWR, 0644);

            lockCustomerFile(fd);

            read(fd, &c, sizeof(struct Mod_Customer));
            c.id = new_id;
            lseek(fd, 0, SEEK_SET);
            write(fd, &c, sizeof(struct Mod_Customer));
            
            unlockCustomerFile(fd);

            close(fd);

            break;
        }
        case 2:{//Customer Name
            char new_name[50];
            recv(sock, new_name, sizeof(new_name), 0);

            int fd = open(file_path, O_RDWR, 0644);

            lockCustomerFile(fd);

            read(fd, &c, sizeof(struct Mod_Customer));
            strcpy(c.name, new_name);
            lseek(fd, 0, SEEK_SET);
            write(fd, &c, sizeof(struct Mod_Customer));
            
            unlockCustomerFile(fd);

            close(fd);
            break;
        }
        case 3:{//Customer Password
            char new_password[20];
            recv(sock, new_password, sizeof(new_password), 0);

            int fd = open(file_path, O_RDWR, 0644);

            lockCustomerFile(fd);

            read(fd, &c, sizeof(struct Mod_Customer));
            strcpy(c.password,new_password);
            lseek(fd, 0, SEEK_SET);
            write(fd, &c, sizeof(struct Mod_Customer));
            
            unlockCustomerFile(fd);

            close(fd);
            break;
        }
        case 4:{//Customer Balance
            float new_balance;
            recv(sock, &new_balance, sizeof(float), 0);

            int fd = open(file_path, O_RDWR, 0644);

            lockCustomerFile(fd);

            read(fd, &c, sizeof(struct Mod_Customer));
            c.balance = new_balance;
            lseek(fd, 0, SEEK_SET);
            write(fd, &c, sizeof(struct Mod_Customer));
            
            unlockCustomerFile(fd);

            close(fd);
            break;
        }
        default:
            break;
    }
}