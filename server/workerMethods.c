#include "header.h"

bool search_pollLog(char *name, voteNode **pollLog)
{
    pthread_mutex_lock(&mtx);
    for (int i = 0; pollLog[i] != NULL; i++)
    {
        if (!strcmp(name, pollLog[i]->voter_name))
        {
            pthread_mutex_unlock(&mtx);
            return true;
        }
    }
    pthread_mutex_unlock(&mtx);
    return false;
}

void record_vote(char *name, char *vote)
{
    voteNode *new_vote = (voteNode *)malloc(sizeof(struct voteNode));
    strcpy(new_vote->voter_name, name);
    strcpy(new_vote->party, vote);

    pthread_mutex_lock(&mtx);
    buff.pollLog[buff.voted++] = new_vote;
    buff.pollLog[buff.voted] = NULL;
    pthread_mutex_unlock(&mtx);
}

void message_exchange(int clientSocket)
{
    char name[256];
    char vote[256];

    // Request the name of the voter
    const char *name_request = "SEND NAME PLEASE\n";
    int bytesSent = write(clientSocket, name_request, strlen(name_request));
    if (bytesSent < 0)
    {
        perror("Failed to send SEND NAME PLEASE to client");
        exit(EXIT_FAILURE);
    }

    // Read the name of the voter
    memset(name, 0, sizeof(name));
    int bytesRead = read(clientSocket, name, sizeof(name) - 1);
    if (bytesRead < 0)
    {
        perror("Failed to read from client socket");
        exit(EXIT_FAILURE);
    }

    // Check if the person with this name has already voted
    if (search_pollLog(name, buff.pollLog) == true)
    {
        const char *negative_response = "ALREADY VOTED\n";
        bytesSent = write(clientSocket, negative_response, strlen(negative_response));
        if (bytesSent < 0)
        {
            perror("Failed to send ALREADY VOTED to client");
            exit(EXIT_FAILURE);
        }
        close(clientSocket);
        return;
    }

    // Request the vote
    const char *vote_request = "SEND VOTE PLEASE\n";
    bytesSent = write(clientSocket, vote_request, strlen(vote_request));
    if (bytesSent < 0)
    {
        perror("Failed to send SEND VOTE PLEASE to client");
        exit(EXIT_FAILURE);
    }

    // Read the vote
    memset(vote, 0, sizeof(vote));
    bytesRead = read(clientSocket, vote, sizeof(vote) - 1);
    if (bytesRead < 0)
    {
        perror("Failed to read from client socket");
        exit(EXIT_FAILURE);
    }

    // Record the vote
    record_vote(name, vote);

    // Final message
    char *final_message = "VOTE FOR PARTY RECORDED\n";
    bytesSent = write(clientSocket, final_message, strlen(final_message));
    if (bytesSent < 0)
    {
        perror("Failed to send message to client");
        exit(EXIT_FAILURE);
    }
    return;
}

void print_pollLog(voteNode **pollLog)
{
    pthread_mutex_lock(&mtx);
    for (int i = 0; pollLog[i] != NULL; i++)
    {
        printf("name: %s vote: %s\n", pollLog[i]->voter_name, pollLog[i]->party);
        fflush(stdout);
    }
    pthread_mutex_unlock(&mtx);
}

void write_pollLog(voteNode **pollLog)
{
    FILE *pollLog_file;
    pollLog_file = fopen(buff.pollLog_filename, "w");

    if (pollLog_file == NULL)
    {
        perror("Failed to open the file.\n");
        exit(1);
    }

    pthread_mutex_lock(&mtx);
    for (int i = 0; pollLog[i] != NULL; i++)
    {
        fprintf(pollLog_file, "%s %s\n", pollLog[i]->voter_name, pollLog[i]->party);
        fflush(pollLog_file);
    }
    pthread_mutex_unlock(&mtx);

    fclose(pollLog_file);
}

bool search_pollStats(char *party)
{

    for (int i = 0; pollStats[i] != NULL; i++)
    {
        if (!strcmp(party, pollStats[i]->party))
        {
            pollStats[i]->vote_count++;
            return true;
        }
    }
    return false;
}

void write_pollStats(voteNode **pollLog)
{
    FILE *pollStats_file;
    pollStats_file = fopen(buff.pollStats_filename, "w");

    if (pollStats_file == NULL)
    {
        perror("Failed to open the file.\n");
        exit(1);
    }

    for (int i = 0; pollLog[i] != NULL; i++)
    {
        if (!search_pollStats(pollLog[i]->party))
        {
            statNode *new_node = (statNode *)malloc(sizeof(struct statNode));
            strcpy(new_node->party, pollLog[i]->party);
            new_node->vote_count = 1;

            pollStats[num_of_parties++] = new_node;
            pollStats[num_of_parties] = NULL;
        }
    }

    for (int i = 0; pollStats[i] != NULL; i++)
    {
        fprintf(pollStats_file, "%s %d\n", pollStats[i]->party, pollStats[i]->vote_count);
        fflush(pollStats_file);
    }
    pthread_mutex_unlock(&mtx);

    fclose(pollStats_file);
    return;
}

void free_memory()
{
    free(workerThreads);
    free(buff.buffer);
    for (int i = 0; pollStats[i] != NULL; i++)
    {
        free(pollStats[i]);
        pollStats[i] = NULL;
    }

    for (int i = 0; buff.pollLog[i] != NULL; i++)
    {
        free(buff.pollLog[i]);
        buff.pollLog[i] = NULL;
    }

    pthread_cond_destroy(&cond_nonempty);
    pthread_cond_destroy(&cond_nonfull);
    pthread_mutex_destroy(&mtx);
    pthread_mutex_destroy(&terminate_mtx);
    return;
}

void signalHandler(int signal)
{
    pthread_mutex_lock(&terminate_mtx);
    terminate_flag = 1;
    pthread_mutex_unlock(&terminate_mtx);
    pthread_cond_broadcast(&cond_nonempty);

    for (int i = 0; i < buff.numWorkerthreads; i++)
    {
        if (pthread_join(*(workerThreads + i), NULL) != 0)
        {
            fprintf(stderr, "Failed to join thread %d\n", i);
            exit(1);
        }
    }
    // print_pollLog(buff.pollLog);

    write_pollLog(buff.pollLog);
    write_pollStats(buff.pollLog);
    free_memory();

    exit(signal);
}
