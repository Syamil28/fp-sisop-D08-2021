#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#define PORT 8080
#define OK "200"
#define FAIL "100"

char activeUser[20];

int login(int socketfd, char *username, char *password, int root) {
    char data[100], buffer[BUFSIZ];
    sprintf(data, "%s,%s", username, password);
    if (root)
        send(socketfd, "root", 4, 0);
    else 
        send(socketfd, data, strlen(data), 0);

    read(socketfd, buffer, BUFSIZ);
    if (strcmp(buffer, OK))
        return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    int socketfd = createSocket();

    if (socketfd == -1)
        exit(0);

    int res, root; // root is 1 if program run by root
    if (geteuid() != 0) {
        root = 0;
        res = login(socketfd, argv[2], argv[4], root);
    }
    else {
        root = 1;
        res = login(socketfd, argv[2], argv[4], root);
    }

    if (!res) {
        printf("Credential not valid. Try again.\n");
        exit(0);
    }
    
    printf("Login berhasil\nYou can create query\n");

    interface(socketfd, root);

    return 0;
}
