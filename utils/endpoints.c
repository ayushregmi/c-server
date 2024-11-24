
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "request.h"
#include "response.h"
#include "endpoints.h"

struct endpoints *createEndpoint(char *uri, char *type, void (*handler)(struct RequestFormat *, struct ResponseFormat *))
{

    struct endpoints *newEndpoint = malloc(sizeof(struct endpoints));
    newEndpoint->uri = malloc(strlen(uri));
    strcpy(newEndpoint->uri, uri);
    newEndpoint->type = malloc(strlen(type));
    strcpy(newEndpoint->type, type);
    newEndpoint->handler = handler;
    newEndpoint->next = NULL;

    return newEndpoint;
}

void addEndpoint(struct endpoints *endpointList, char *uri, char *type, void (*handler)(struct RequestFormat *, struct ResponseFormat *))
{

    struct endpoints *current = endpointList;

    while (current->next != NULL)
    {
        current = current->next;
    }

    struct endpoints *newEndpoint = createEndpoint(uri, type, handler);
    current->next = newEndpoint;
}

struct endpoints *getEndpoint(struct endpoints *endpoint, char *uri, char *type)
{

    struct endpoints *current = endpoint;

    while (current != NULL)
    {

        if (strcmp(uri, current->uri) == 0 && strcmp(type, current->type) == 0)
        {
            struct endpoints *foundEndpoint = createEndpoint(current->uri, current->type, current->handler);

            return foundEndpoint;
        }
        current = current->next;
    }

    return NULL;
}