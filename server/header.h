#ifndef SERVER
#define SERVER
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

// Structs
typedef struct voteNode
{
    char voter_name[50];
    char party[50];
} voteNode;

typedef struct statNode
{
    char party[50];
    int vote_count;
} statNode;

typedef struct
{
    int *buffer;
    int bufferSize;
    int port;
    int connection_count;
    int numWorkerthreads;
    char pollLog_filename[20];
    voteNode *pollLog[10000];
    char pollStats_filename[20];
    int voted; // number of people that have voted
} buffer_info;

typedef struct
{
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    struct sockaddr *serverptr;
    struct sockaddr *clientptr;
    socklen_t clientLength;
    int serverSocket;
} server_info;

// Global variables
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
buffer_info buff;
server_info server;
pthread_t *workerThreads;
int terminate_flag;
pthread_mutex_t terminate_mtx;
statNode *pollStats[50]; // maximum number of 50 political parties
int num_of_parties;

// Function declarations
void *masterThread(void *argp);
void *workerThread(void *argp);

void place_socket(int socket);
int obtain_socket();

void server_setup();

bool search_pollLog(char *name, voteNode **pollLog);
void print_pollLog(voteNode **pollLog);
void write_pollLog(voteNode **pollLog);

void message_exchange(int clientSocket);
void record_vote(char *name, char *vote);

void signalHandler(int signal);
#endif