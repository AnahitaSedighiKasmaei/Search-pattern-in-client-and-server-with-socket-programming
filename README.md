# Search-pattern-in-client-and-server-with-socket-programming
Summary:
Search a specific keyword in the client, send the keyword and the file to the server, and then search that keyword in the server. Then send the search result to the client and show it on the console.

Description:
Two programs server.c (server) and dgrep.c (client) were written in C to implement a simple
distributed grep command to search whole words in two files in a distributed manner.
- The server and the client are required to run on two different machines.
- The communication between the client and the server will be through sockets
- The server must start running before the client, and wait for a connection from the
client.
- The server must run in an infinite loop.
- The clients’s name is dgrep and must accept the following (three) command line
arguments


> argv[1]=pattern (pattern to be searched in whole words)
> argv[2]= file1(Relative or absolute path of the first file)
> argv[3]= file2 (Relative or absolute path of the second file)

Ex: $ ./dgrep peace file1.txt file2.txt

Validations:
> If either file1 or file2 cannot be found by the client, the client program is
terminated with a suitable message.
> If the client is unable to connect to the server for any reason, the client program
is aborted and a suitable message is displayed.
> If the client successfully connects to the server:
> The client sends file2 to the server
> the client runs grep -w pattern file1
> The server runs grep -w pattern file2 and returns the result to the client. (use
the system() library function to execute commands)
> The communication between the server and the client must be through sockets
> The client should finally display the combined results (from both the client and
the server – in that order) on the standard output.


- command:

Command for building the client:
> gcc -o dgrep dgrep.c
Command for runing client:
> ./dgrep peace file1.txt file2.txt

Command for running client:

Command for building the Server:
> gcc -o server server.c
Command for runing server:
> ./server
