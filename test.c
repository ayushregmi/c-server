#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "utils/server.h"
#include "utils/utils.h"

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        DieWithUserMessage("Parameter(s)", "<server address> <port>");
    }

    char *hostName = argv[1];
    char *port = argv[2];

    struct Server *server = (struct Server *)malloc(sizeof(struct Server));

    server->hostName = hostName;
    server->port = port;

    startServer(server);

    return 0;
}