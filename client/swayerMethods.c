#include "header.h"

int file_line_count(char *input_file)
{
    int line_count = 0;
    FILE *file_stream;
    char line[MAX_LINE_LENGTH];
    if ((file_stream = fopen(input_file, "r")) == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    while (fgets(line, sizeof(line), file_stream) != NULL)
    {
        line_count++;
    }

    fclose(file_stream);

    return line_count;
}

voter_info parse_line(char *line)
{
    voter_info voter;

    char *ptr = strchr(line, ' ');
    char *sec_ptr = strchr(ptr + 1, ' ');
    *sec_ptr = '_';

    char *voter_name = strtok(line, "_");
    char *voter_vote = strtok(NULL, "_");

    strcpy(voter.fullname, voter_name);
    strcpy(voter.vote, voter_vote);

    return voter;
}

void read_file(char *input_file)
{
    pthread_t *clientThreads = (pthread_t *)malloc(line_count * sizeof(pthread_t));
    voter_info *voterArray[line_count];

    FILE *file_stream;
    char line[MAX_LINE_LENGTH];
    if ((file_stream = fopen(input_file, "r")) == NULL)
    {
        perror("Error opening file");
        return;
    }

    int i = 0;
    while (fgets(line, sizeof(line), file_stream) != NULL)
    {
        voter_info voter = parse_line(line);
        voter_info *voteNode = (voter_info *)malloc(sizeof(voter_info));
        strcpy(voteNode->fullname, voter.fullname);
        strcpy(voteNode->vote, voter.vote);
        voteNode->i = i;

        voterArray[i] = voteNode;
        i++;
    }

    for (int i = 0; i < line_count; i++)
    {
        pthread_create(clientThreads + i, NULL, clientThread, (void *)voterArray[i]);
    }

    fclose(file_stream);

    for (int i = 0; i < line_count; i++)
    {
        if (pthread_join(*(clientThreads + i), NULL) != 0)
        {
            fprintf(stderr, "Failed to join thread %d\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < line_count; i++)
    {
        free(voterArray[i]); // Remember to free allocated memory
        voterArray[i] = NULL;
    }

    free(clientThreads);

    return;
}

void message_exchange(int clientSocket, voter_info *voter)
{
    // Send fullname to the server
    int bytesSent = write(clientSocket, voter->fullname, strlen(voter->fullname));
    if (bytesSent < 0)
    {
        perror("Failed to send fullname to client");
        exit(EXIT_FAILURE);
    }

    // Read the response
    char response[256];
    memset(response, 0, sizeof(response));
    int bytesRead = read(clientSocket, response, sizeof(response) - 1);
    if (bytesRead < 0)
    {
        perror("Failed to read from client socket");
        exit(EXIT_FAILURE);
    }

    // If the voter has already voted and the server responses negatively, the client will not procceed into sending a vote
    if (!strcmp(response, "ALREADY VOTED\n"))
    {
        return;
    }

    // Send vote to server
    bytesSent = write(clientSocket, voter->vote, strlen(voter->vote));
    if (bytesSent < 0)
    {
        perror("Failed to send vote to client");
        exit(EXIT_FAILURE);
    }
    return;
}

void *clientThread(void *argp)
{
    voter_info *voter = (voter_info *)argp;
    printf("Thread %ld voter info: %s %s %d\n", pthread_self(), voter->fullname, voter->vote, voter->i);

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    if (connect(clientSocket, poller.serverptr, poller.serverlen) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }

    // printf("Thread %ld connected to the server. You can start sending/receiving messages. %s %s\n", pthread_self(), voter->fullname, voter->vote);

    message_exchange(clientSocket, voter);

    // Close socket and exit
    close(clientSocket);
    pthread_exit(NULL);
}