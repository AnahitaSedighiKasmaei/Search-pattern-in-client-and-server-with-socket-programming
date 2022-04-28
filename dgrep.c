
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
#include <fcntl.h>
#include <sys/signal.h>

int port = 5000;

#define die(e)                        \
    do                                \
    {                                 \
        fprintf(stderr, "\n%s\n", e); \
        exit(EXIT_FAILURE);           \
    } while (0);

struct Server
{
    double sockerFileDescriptor, connection, error;
    char message[0];
};

int check_file_exist(const char *filename)
{
    FILE *file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }

    return 0;
}

void connect_server(double *serverConnection, double *socketFileDescriptor)
{
    // struct Server server;
    struct sockaddr_in serv_addr;
    int sockfd = 0;
    // struct hostent *server;

    char buffer[256];

    /* create socket and get file descriptor */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("Cannot create socket.");

    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int conection = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (conection < 0)
        die("Error in connect: Cannot find an open port on server.");

    *socketFileDescriptor = sockfd;
    *serverConnection = conection;
}

void run_command(int socket, char pattern[], char filePath[])
{
    char *program = "grep";
    char *args[] = {program, "--color=always", "--ignore-case", "--line-number", "--ignore-case", "--text", "--with-filename", pattern, filePath, NULL};
    // Run Command
    if (execvp(program, args) < 0)
        die("Error in execvp");

    // system("grep");
    exit(0);
}

void send_file(int socket, char filePath[])
{
    unsigned char content[1024] = {0};

    int fp = open(filePath, O_RDONLY);
    if (fp < 0)
        die("Error File not valid.");
    int size;
    while ((size = read(fp, content, sizeof(content))) > 0)
    {
        write(socket, content, size);
        if (size < sizeof(content))
            break;
    }

    close(fp);
}

void read_response(int socket)
{
    unsigned char message[1024];
    int size;
    while ((size = read(socket, message, sizeof(message))) > 0)
    {
        printf("%s", message);
        if (size < sizeof(message))
            break;
    }
}

int main(int argc, char *argv[])
{
    printf("\nClient Start ...\n");

    if (!check_file_exist(argv[2]))
        die("File 1 does not exist.");

    if (!check_file_exist(argv[3]))
        die("File 2 does not exist.");

    double serverConnection, socket;

    connect_server(&serverConnection, &socket);

    printf("\n\n\n==================\n\n");
    printf("\n pattern: %s\n", argv[1]);
    printf("\n source on client: %s\n", argv[2]);
    printf("\n source on server: %s\n", argv[3]);
    printf("\n==================\n\n");

    int pid = fork();
    if (pid == 0)
    {
        printf("\n\n");
        printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&[Client Result]&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n\n");
        run_command(socket, argv[1], argv[2]);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);

        // Send Pattern for search to the server
        write(socket, argv[1], strlen(argv[1]) + 1);

        // Send File to server
        send_file(socket, argv[3]);

        printf("\n\n");
        printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&[Server Result]&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n\n");

        // Receive the response of grep on file 2 from server
        read_response(socket);

        printf("\n\n ==== \n");
        printf("\n Search was done successfully! \n\n");
    }
    else
    {
        die("Error in forking.")
    }

    return 0;
}
