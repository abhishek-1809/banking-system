void modify_menu()
{
    printf("\n1. Customer id\n");
    printf("2. Customer Name\n");
    printf("3. Customer Password\n");
    printf("4. Customer Balance\n");
    printf("Enter your choice: ");
}

void modify_customer(int sock)
{
    int customer_id;
    printf("Enter Customer's id you want to modify: ");
    scanf("%d",&customer_id);
    send(sock, &customer_id, sizeof(int),0);

    int emp_choice;
    modify_menu();
    scanf("%d",&emp_choice);

    send(sock, &emp_choice, sizeof(int),0);
    switch (emp_choice)
    {
        case 1:{//Customer id
            int new_id;
            printf("Enter new new_id: ");
            scanf("%d",&new_id);
            send(sock, &new_id, sizeof(int), 0);
            break;
        }
        case 2:{//Customer Name
            char name[50];
            printf("Enter new name: ");
            scanf("%s",name);
            send(sock, name, sizeof(name), 0);
            break;
        }
        case 3:{//Customer Password
            char password[20];
            printf("Enter new password: ");
            scanf("%s",password);
            send(sock, password, sizeof(password), 0);
            break;
        }
        case 4:{//Customer Balance
            float balance;
            printf("Enter new balance: ");
            scanf("%f",&balance);
            send(sock, &balance, sizeof(float), 0);
            break;
        }

        default:
            break;
    }
    
}