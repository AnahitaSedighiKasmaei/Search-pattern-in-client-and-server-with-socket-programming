#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <time.h>

int port = 5000;

#define die(e)                        \
    do                                \
    {                                 \
        fprintf(stderr, "\n%s\n", e); \
        exit(EXIT_FAILURE);           \
    } while (0);

void runCommand(int socket, char pattern[], char filePath[])
{
    char string[1024]; // For very long path

    // This is the second way different from client side. We used system just to colorize the grep.
    sprintf(string, "export GREP_COLORS='ms=01;34'; grep --color=always --line-number --ignore-case --text --with-filename %s %s", pattern, filePath);
    system(string);
    // char *program = "grep";
    // char *args[] = {program, "--color=always", "--text", "--with-filename", "-w", pattern, filePath, NULL};

    // // Run Command
    // if (execvp(program, args) < 0)
    //     die("Error in execvp");
}

void handle_client(int socket)
{
    int b, tot, pid, n;
    char pattern[129];
    char fileName[] = "server-file";

    // Client Pattern
    int valPat = read(socket, pattern, sizeof(pattern));
    if (valPat <= 0)
        die("No pattern was given");

    printf("\n Reading finished. \n");
    printf("\n Client Pattern is: `%s` \n \n ", pattern);

    // memset(buff, '0', sizeof(buff));

    int fd;
    if ((fd = open(fileName, O_CREAT | O_RDWR, 0666)) > -1)
    {
        unsigned char sBuff[1024] = {0};
        while ((b = read(socket, sBuff, sizeof(sBuff))) > 0)
        {
            n = write(fd, sBuff, b);
            if (b < sizeof(sBuff))
                break;
        }

        dup2(socket, STDOUT_FILENO);

        int pid = fork();

        if (pid == 0)
        {
            runCommand(socket, pattern, fileName);
        }
        else
        {
            waitpid(pid, NULL, 0);
            write(fd, "\n", 1);
        }
        exit(0);
    }
    else
        die("Error in server file open");

    close(fd);
}

int start_socket()
{
    int fSocket = 0;
    struct sockaddr_in serv_addr;

    char buff[1025];
    int num;

    fSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fSocket < 0)
        die("Error in Socket.");

    printf("\n Socket created\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buff, '0', sizeof(buff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    int bi = bind(fSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (bi < 0)
        die("Error in Bind.");
    printf("\n Binding successfull.\n");

    if (listen(fSocket, 10) == 0)
    {
        printf("\n Listening....\n");
    }
    else
        die("Error in Listening.");

    return fSocket;
}

int main(int argc, char *argv[])
{
    int sock = 0;
    int fSocket = start_socket();

    while (1)
    {
        sock = accept(fSocket, (struct sockaddr *)NULL, NULL);
        if (sock == -1)
        {
            perror("Accept");
            return -1;
        }
        printf("\n Connection Established. \n");
        printf("\n Reading client messages ... \n");
        int pid = fork();
        if (pid == 0)
        {
            handle_client(sock);

            close(sock);
        }

        if (pid < 0)
            die("Error in handle_client fork.");
    }

    return 0;
}