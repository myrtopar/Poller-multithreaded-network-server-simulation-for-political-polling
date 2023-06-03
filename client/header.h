#include <stdio.h>
#include <sys/wait.h>   /* sockets */
#include <sys/types.h>  /* sockets */
#include <sys/socket.h> /* sockets */
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 100
#define BUFFER_SIZE 128

typedef struct
{
    unsigned int serverlen;
    struct sockaddr_in server;
    struct sockaddr *serverptr;
    struct hostent *rem;
    int server_port;
} server_info;

typedef struct
{
    char fullname[30];
    char vote[20];
    int i;
} voter_info;

// Global variables
server_info poller;
int line_count;

// Function declarations
void *clientThread(void *argp);
int file_line_count(char *input_file);
void read_file(char *input_file);
voter_info parse_line(char *line);
void message_exchange(int clientSocket, voter_info *voter);