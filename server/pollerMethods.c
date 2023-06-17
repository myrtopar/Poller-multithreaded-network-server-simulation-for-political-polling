#include "header.h"

void server_setup()
{
    server.serverptr = (struct sockaddr *)&server.serverAddress;
    server.clientptr = (struct sockaddr *)&server.clientAddress;
    server.clientLength = sizeof(server.clientAddress);

    // Setting up server address
    server.serverAddress.sin_family = AF_INET;
    server.serverAddress.sin_addr.s_addr = INADDR_ANY;
    server.serverAddress.sin_port = htons(buff.port);

    // Creating socket for server
    server.serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (server.serverSocket < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Binding socket to port
    if (bind(server.serverSocket, server.serverptr, sizeof(server.serverAddress)) < 0)
    {
        perror("Error on bind");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server.serverSocket, buff.bufferSize) < 0)
    {
        perror("Error listening on socket");
        exit(1);
    }

    printf("server listening on port: %d\n", buff.port);
    return;
}

int obtain_socket()
{
    pthread_mutex_lock(&mtx);

    // don't do anything while the buffer is empty. Wait until there is a socket in the buffer to start consuming
    while (buff.connection_count <= 0 && terminate_flag == 0)
    {
        pthread_cond_wait(&cond_nonempty, &mtx);
    }

    if (terminate_flag == 1)
    {
        // printf("thread %ld woke up from broadcast, about to break\n", pthread_self());
        pthread_mutex_unlock(&mtx);
        return -2;
    }

    // Consuming a socket descriptor from the buffer
    int clientSocket = buff.buffer[--buff.connection_count];

    pthread_mutex_unlock(&mtx);
    pthread_cond_signal(&cond_nonfull);

    return clientSocket;
}

void place_socket(int socket)
{
    pthread_mutex_lock(&mtx);

    // wait until the buffer is not full
    while (buff.connection_count == buff.bufferSize)
    {
        pthread_cond_wait(&cond_nonfull, &mtx);
    }

    // place the socket descriptor of the new connection in the buffer
    buff.buffer[buff.connection_count++] = socket;

    pthread_mutex_unlock(&mtx);
    pthread_cond_signal(&cond_nonempty);

    return;
}

void *masterThread(void *argp)
{
    // printf("in master thread\n");

    server_setup();
    int clientSocket;

    while (1)
    {
        if ((clientSocket = accept(server.serverSocket, server.clientptr, &server.clientLength)) < 0)
        {
            perror("Error accepting connection");
            exit(1);
        }
        // printf("accepted connection with socket %d\n", clientSocket);
        place_socket(clientSocket);
    }

    pthread_exit(NULL);
}

void *workerThread(void *argp)
{
    int clientSocket;

    while (!terminate_flag)
    {
        if ((clientSocket = obtain_socket()) == -2)
        {
            break;
        }

        message_exchange(clientSocket);
        close(clientSocket);
    }

    pthread_exit(NULL);
}
