#include "header.h"

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);

    if (argc != 6)
    {
        printf("Invalid number of arguments. Expected 5 arguments.\n");
        exit(1);
    }

    // Filling the global struct buffer_info
    buff.port = atoi(argv[1]);
    buff.numWorkerthreads = atoi(argv[2]);
    buff.bufferSize = atoi(argv[3]);

    buff.buffer = malloc(buff.bufferSize * sizeof(int));
    buff.connection_count = 0;
    buff.pollLog[0] = NULL; // NULL will indicate where the pollLog array ends
    buff.voted = 0;
    strcpy(buff.pollLog_filename, argv[4]);
    strcpy(buff.pollStats_filename, argv[5]);
    terminate_flag = 0;
    num_of_parties = 0;
    pollStats[0] = NULL; // NULL will indicate where the pollStats array ends

    // Mutex and condition variables initialization
    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);
    pthread_mutex_init(&terminate_mtx, 0);

    // Initialize the buffer before any socket description is placed
    for (int i = 0; i < buff.bufferSize; i++)
    {
        buff.buffer[i] = -1;
    }

    printf("Setting up the server...\n\n");
    pthread_t master_thread;
    if (pthread_create(&master_thread, NULL, masterThread, NULL))
    {
        perror("Failed to create master thread\n");
        exit(1);
    }
    sleep(1);

    workerThreads = (pthread_t *)malloc(buff.numWorkerthreads * sizeof(pthread_t));

    for (int i = 0; i < buff.numWorkerthreads; i++)
    {
        pthread_create(workerThreads + i, NULL, workerThread, NULL);
    }

    // if (pthread_join(master_thread, NULL)) // cannot go below this line while the server is running
    // {
    //     perror("pthread join/n");
    //     exit(1);
    // }
    while (1)
    {
    }

    printf("the end\n");
    return 0;
}
