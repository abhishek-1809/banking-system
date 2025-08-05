
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