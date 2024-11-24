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
#include "files.h"
#include "endpoints.h"

#define MAXPENDING 5

int createServer(char *hostName, char *port);
int AcceptClientRequest(int serverSocket);
void HandleRequest(int clientSocket, struct endpoints *endpoint);

struct Server *getServer(char *hostName, char *port)
{

    struct Server *server = malloc(sizeof(struct Server));
    server->hostName = malloc(strlen(hostName) + 1);
    strcpy(server->hostName, hostName);

    server->port = malloc(strlen(port) + 1);
    strcpy(server->port, port);

    server->endpointList = NULL;

    return server;
}

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

void HandleRequest(int clientSocket, struct endpoints *endpointList)
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
    // displayRequest(request);
    struct ResponseFormat *response = createResponse("HTTP/1.1");

    struct endpoints *endpoint = getEndpoint(endpointList, request->uri, request->methodType);
    if (endpoint != NULL)
    {
        fprintf(stdout, "%s\t%s\n", endpoint->uri, request->methodType);
        endpoint->handler(request, response);
    }
    else
    {

        char *fileContent;
        char *fileName;
        // fprintf(stdout, "%s\n", request->uri + 1);
        if (strcmp(request->uri, "/") == 0)
        {
            fileName = malloc(11);
            strcpy(fileName, "index.html");
            // fileContent = getFileContent("index.html");
        }
        else
        {
            fileName = malloc(strlen(request->uri) - 1);
            snprintf(fileName, strlen(request->uri), "%s", request->uri + 1);
            // fileContent = getFileContent(request->uri + 1);
        }

        // fprintf(stdout, "File Name: %s\n", request->uri);

        fileContent = getFileContent(fileName);

        if (fileContent == NULL)
        {
            free(fileContent);
            fileContent = getFileContent("notfound.html");
            addBody(response, fileContent);
            addContentType(response, getMimeType("notfound.html"));
            addStatusCode(response, SC_OK);
        }
        else
        {
            addBody(response, fileContent);
            addContentType(response, getMimeType(fileName));
            addStatusCode(response, SC_OK);
        }

        addHeader(response, "Nice", "done");
        free(fileName);
        free(fileContent);
    }
    char *responseStr = prepareResponse(response);
    size_t responseLen = strlen(responseStr);
    // fprintf(stdout, "%s\n", responseStr);

    ssize_t numBytesSent = send(clientSocket, responseStr, responseLen, 0);

    free(responseStr);
    freerequest(request);
    freeResponse(response);

    fclose(stream);
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
        HandleRequest(clientSocket, server->endpointList);

        close(clientSocket);
    }

    close(serverSocket);
    exit(0);
}