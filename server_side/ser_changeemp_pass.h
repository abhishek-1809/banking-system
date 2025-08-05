void change_emp_pass(int sock,int emp_id){
    struct Employee {

        char name[50];
        int id;
        char password[20];
    };
    struct Employee emp;

    char file_path[20];
    sprintf(file_path,"employee_%d.dat", emp_id);
    char new_password[20];
    recv(sock, new_password, sizeof(new_password), 0);

    int fd = open(file_path, O_RDWR, 0644);
    lockCustomerFile(fd);  // Lock the file for writing

    read(fd, &emp, sizeof(struct Employee));

    strcpy(emp.password,new_password);

    lseek(fd, 0, SEEK_SET);
    write(fd, &emp, sizeof(struct Employee));

    unlockCustomerFile(fd);  // Unlock the file after writing
    close(fd);
}