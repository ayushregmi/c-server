#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "utils/server.h"
#include "utils/utils.h"
#include "utils/request.h"
#include "utils/response.h"
#include "utils/endpoints.h"

void user(struct RequestFormat *request, struct ResponseFormat *response)
{
    char *body = "{'user': 'user1', 'password':'123'}";
    addBody(response, body);
    addStatusCode(response, SC_OK);
    addContentType(response, "application/json");
}

void product(struct RequestFormat *request, struct ResponseFormat *response)
{

    char *body = "{'p_id': 100, 'p_name':'p_1'}";
    addBody(response, body);
    addStatusCode(response, SC_OK);
    addContentType(response, "application/json");
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        DieWithUserMessage("Parameter(s)", "<server address> <port>");
    }

    char *hostName = argv[1];
    char *port = argv[2];

    // struct Server *server = (struct Server *)malloc(sizeof(struct Server));
    // server->hostName = malloc(strlen(hostName));
    // server->port = malloc(strlen(port));

    // strcpy(server->hostName, hostName);
    // strcpy(server->port, port);

    struct Server *server = getServer(hostName, port);
    struct endpoints *endpoint = createEndpoint("/user", "GET", user);
    server->endpointList = endpoint;
    addEndpoint(server->endpointList, "/product", "GET", product);

    startServer(server);

    return 0;
}