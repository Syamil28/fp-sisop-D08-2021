#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <wait.h>
#include <time.h>

#define PORT 8080
#define OK "200"
#define FAIL "100"

char activeUser[20] = "";
char activeDB[20] = "";
const char delimiter[10] = " ,=();\n";

int splitString(char splitted[][100], char str[]) {
    int counter=0;
    char *token = strtok(str, delimiter);

    while (token != NULL) {
        strcpy(splitted[counter], token);
        counter++;
        token = strtok(NULL, delimiter);
    }
    return counter;
}

int doHaveAccess(char dbname[]) {
    FILE *fp = fopen("databases/credentials/access.csv", "r");
    char line[100];
    int haveAccess = 0;
    char data[100];
    sprintf(data, "%s,%s", dbname, activeUser);

    while (fscanf(fp, "%s", line) != EOF) {
        if (!strcmp(data, line)) {
            haveAccess = 1;
            break;
        }
    }
    return haveAccess;
}

void clearBuffer(char* b) {
    for (int i = 0; i < BUFSIZ; i++)
        b[i] = '\0';
}

int main(int argc, char const *argv[]) {
    int socketfd = launchServer();
    char buffer[BUFSIZ];
    read(socketfd, buffer, BUFSIZ);
    int res = login(socketfd, buffer);
    if (res) 
        send(socketfd, OK, strlen(OK), 0);
    else 
        send(socketfd, FAIL, strlen(FAIL), 0);

    if (res)
        handleQuery(socketfd);
    return 0;
}
