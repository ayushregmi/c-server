#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "server.h"
#include "request.h"
#include "response.h"

#define MAXPENDING 5

int createServer(char *hostName, char *port);
int AcceptClientRequest(int serverSocket);
void HandleRequest(int clientSocket);

int AcceptClientRequest(int serverSocket)
{
    struct sockaddr_storage clientAddress;
    socklen_t addrLen = sizeof(clientAddress);

    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &addrLen);

    if (clientSocket < 0)
    {
        ContinueWithSystemMessage("accept() failed");
    }

    return clientSocket;
}

void HandleRequest(int clientSocket)
{
    // Reading the data from the socket
    char buffer[BUFSIZ];

    size_t numBytesReceived = recv(clientSocket, &buffer, BUFSIZ - 1, 0);

    if (numBytesReceived < 0)
    {
        ContinueWithSystemMessage("recv() failed");
        return;
    }

    buffer[numBytesReceived] = '\0';

    // Formatting Request
    FILE *stream = fmemopen((void *)buffer, strlen(buffer), "r");

    struct RequestFormat *request = RequestFormatFromFile(stream);
    displayRequest(request);

    // fclose(stream);

    // while (numBytesReceived > 0)
    // {
    //     numBytesReceived = recv(clientSocket, &buffer + totalBytesReceived, BUFSIZ - 1 - totalBytesReceived, 0);
    // }
}

void shutdownSocket(int sig)
{
}

int createServer(char *hostName, char *port)
{
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_socktype = SOCK_STREAM;
    addrCriteria.ai_protocol = IPPROTO_TCP;

    struct addrinfo *serverAddr;

    int rtnVal = 0;

    if ((rtnVal = getaddrinfo(hostName, port, &addrCriteria, &serverAddr)) != 0)
    {
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));
    }

    int serverSocket = -1;

    for (struct addrinfo *addr = serverAddr; addr != NULL; addr = addr->ai_next)
    {
        serverSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if (serverSocket < 0)
        {
            continue;
        }

        if (bind(serverSocket, addr->ai_addr, addr->ai_addrlen) == 0 && listen(serverSocket, MAXPENDING) == 0)
        {
            struct sockaddr_storage localAddr;
            socklen_t addrLen = sizeof(localAddr);

            if (getsockname(serverSocket, (struct sockaddr *)&localAddr, &addrLen) < 0)
            {
                DieWithSystemMessage("getsockname() failed");
            }
            fputs("Starting server at ", stdout);
            PrintSocketAddress((struct sockaddr *)&localAddr, stdout);
            break;
        }
        close(serverSocket);
        serverSocket = -1;
    }

    freeaddrinfo(serverAddr);

    return serverSocket;
}

void startServer(struct Server *server)
{

    int serverSocket = createServer(server->hostName, server->port);

    if (serverSocket < 0)
    {
        DieWithSystemMessage("Unable to start server. socket() failed.");
    }

    for (;;)
    {
        int clientSocket = AcceptClientRequest(serverSocket);

        if (clientSocket < 0)
        {
            continue;
        }
        HandleRequest(clientSocket);

        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
    }

    close(serverSocket);
    exit(0);
}