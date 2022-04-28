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

int port = 5000;

#define die(e)                      \
    do                              \
    {                               \
        fprintf(stderr, "%s\n", e); \
        exit(EXIT_FAILURE);         \
    } while (0);

int main(int argc, char *argv[])
{
    int fSocket = 0, newSocket = 0, b, tot, pid, n;
    struct sockaddr_in serv_addr;

    char buff[1025];
    int num;

    fSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fSocket < 0)
    {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("\n Socket created\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buff, '0', sizeof(buff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    int bi = bind(fSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (bi < 0)
    {
        perror("\n Error in bind \n");
        exit(1);
    }
    printf("Binding successfull.\n");

    if (listen(fSocket, 10) == 0)
    {
        printf("Listening....\n");
    }
    else
    {
        perror("\n Error in listening \n");
        exit(1);
    }

    // if (pid = fork()) /* reading client messages */
    //  {
    printf("\n reading client messages ... \n");

    char clientPattern[1025];

    while (1)
    {
        newSocket = accept(fSocket, (struct sockaddr *)NULL, NULL);
        if (newSocket == -1)
        {
            perror("Accept");
            return -1;
        }

        // get pattern for search form Client
        read(newSocket, clientPattern, sizeof(clientPattern));
        printf("Client Pattern is: %s \n \n ", clientPattern);
        // open file for send to server

        FILE *fp = fopen("uploaded-file2.txt", "w");

        tot = 0;
        if (fp != NULL)
        {
            while ((b = recv(newSocket, buff, 1024, 0)) > 0)
            {
                tot += b;
                fwrite(buff, 1, b, fp);
            }

            printf("Received byte: %d\n", tot);
            if (b < 0)
                perror("\n Error in receiving file ...");

            fclose(fp);
        }
        else
        {
            perror("\n File in opening file ...");
        }

        int link[2];

        char responseBuff[4096];

        int pid1 = fork();
        if (pid1 == -1)
            return -1;

        if (pid1 == 0) // Child1
        {
             close(fSocket);
            printf("child2 in server ============================");
            // system("grep -w $clientPattern $filename")

            dup2(link[1], STDOUT_FILENO);
            close(link[0]);
            close(link[1]);
            char *program = "grep";
            char *args[] = {program, "--color", clientPattern, "uploaded-file2.txt", NULL};
            execvp(program, args);
            die("execvp");
        }

        int pid2 = fork();
        if (pid2 == -1)
            return -1;

        if (pid2 == 0) // child2
        {
            printf("Child2 in server===============================");
            char output[sizeof(responseBuff)];
            dup2(link[0], 0);
            close(link[0]);
            close(link[1]);
            read(link[0], &output, sizeof(responseBuff));
            int count = 0;
            for (int i = 0; i < sizeof(output); i++)
            {
                if (output[i] == '\0')
                {
                    break;
                }
                count++;
            }
            // close(link[1]);
            // int nbytes = read(link[0], responseBuff, sizeof(responseBuff));

            // int res = write(confd, responseBuff, nbytes);
            // printf("\n%.*s\n", nbytes, responseBuff);
            // printf("\n%d/%d\n", res, nbytes);

            // wait(NULL);
        }

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);

        close(newSocket);
    }

    return 0;
}