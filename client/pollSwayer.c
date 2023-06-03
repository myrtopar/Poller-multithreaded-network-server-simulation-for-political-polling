#include "header.h"

int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        printf("Invalid number of arguments. Expected 5 arguments.\n");
        exit(1);
    }

    // Filling the global struct server_info
    if ((poller.rem = gethostbyname(argv[1])) == NULL)
    {
        perror(" gethostbyname ");
        exit(-1);
    }

    poller.server_port = atoi(argv[2]);
    poller.server.sin_family = AF_INET;
    bcopy((char *)poller.rem->h_addr, (char *)&poller.server.sin_addr, poller.rem->h_length);
    poller.server.sin_port = htons(poller.server_port);

    poller.serverptr = (struct sockaddr *)&poller.server;
    poller.serverlen = sizeof(poller.server);

    line_count = file_line_count(argv[3]);

    // Opening and reading the input file, creating the threads
    read_file(argv[3]);

    return 0;
}