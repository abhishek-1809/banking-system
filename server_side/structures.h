#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

struct Employee
{
    int id;
    char name[50];
    char password[20];
};

struct Customer {
    int id;
    char name[50];
    float balance;
    char password[20];
};

struct New_Customer {
    int id;
    char name[50];
    float balance;
    char password[20];
};