
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int port = 5000;

struct Server
{
    double sockerFileDescriptor, connection, error;
    char message[0];
};

int checkIfFileExists(const char *filename)
{
    FILE *file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }

    return 0;
}

int startServer(double *serverConnection, double *socketFileDescriptor, int port)
{
    // struct Server server;
    struct sockaddr_in serv_addr;
    int sockfd = 0;
    // struct hostent *server;

    char buffer[256];

    /* create socket and get file descriptor */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n %s \n ", "Error : Could not create socket");
        return 1001;
    }

    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int conection = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (conection < 0)
    {
        printf("\n %s \n ", "Error : Connect Failed");
        return 1002;
    }

    *socketFileDescriptor = sockfd;
    *serverConnection = conection;

    return 0;
}

int main(int argc, char *argv[])
{
    printf("Client Start ...");
    char sendbuffer[100];

    if (!checkIfFileExists(argv[2]))
    {
        printf("\n ERROR, %s cannot be found...\n", argv[2]);
        exit(0);
    }

    if (!checkIfFileExists(argv[3]))
    {
        printf("\n ERROR, %s cannot be found...\n", argv[3]);
        exit(0);
    }

    double serverConnection, socketFd;

    int error = startServer(&serverConnection, &socketFd, port);

    printf("\n Error: %d \n", error);
    if (error > 0)
    {
        exit(error);
    }

    FILE *fp = fopen(argv[3], "rb");
    if (fp == NULL)
    {
        perror("\n ERROR, File is not valid...");
        return 2;
    }

    int size;
    write(socketFd, argv[1], sizeof(sendbuffer));

    while ((size = fread(sendbuffer, 1, sizeof(sendbuffer), fp)) > 0)
    {
        send(socketFd, sendbuffer, size, 0);
    }

    fclose(fp);

    printf("\n << File data sent successfully >> \n");
    printf("\n pattern for search: %s\n", argv[1]);
    printf("\n The search source is: %s\n", argv[2]);
    printf("\n--------------------------------------------");

    printf("\n Reading...\n");
    int pid1 = fork();
    if (pid1 == 0) // child1
    {

        printf("\n \n Parent22222====================================.\n");
        char *program = "grep";
        char *args[] = {program, "--color", argv[1], argv[2], NULL};
        execvp(program, args);
        system("grep");
    }

    int pid2 = fork();
    if (pid2 == 0) // child2
    {

        printf("\n \n Child 222222====================================.\n");
        // getting server results
        // char finalServerResult[SIZE] = {0};
        // read(sockfd, finalServerResult, sizeof(finalServerResult));
        // printf("\n%s: ", argv[3]);
        // for(int i=0 ; i<sizeof(finalServerResult) ; i++) {
        // if(finalServerResult[i] == '\n')
        // printf("\n%s: ", argv[3]);
        // else
        // printf("%c", finalServerResult[i]);
        // }

        char serverResponseBuf[4096] = {0};
        read(socketFd, serverResponseBuf, sizeof(serverResponseBuf));
        printf("%s", serverResponseBuf);
        // for (int i = 0; i < sizeof(serverResponseBuf); i++)
        // {
        //     printf("%s", serverResponseBuf[i]);
        // }

        printf("\n \n Closing the connection.\n");
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    close(socketFd);
    return 0;
}
