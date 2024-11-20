// response format of the request
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers.h"
#include "response.h"

const char *getHttpResponseString(int statusCode)
{

    switch (statusCode)
    {
    case SC_OK:
        return "200 OK";
    case SC_BAD_REQUEST:
        return "400 Bad Request";
    case SC_UNAUTHORIZED:
        return "401 Unauthorized";
    case SC_FORBIDDEN:
        return "403 Forbidden";
    case SC_NOT_FOUND:
        return "404 Not Found";
    case SC_INTERNAL_SERVER_ERROR:
        return "500 Internal Server Error";
    default:
        return "Invalid Status Code";
    }
}

struct ResponseFormat *createResponse(char *httpVersion)
{
    struct ResponseFormat *httpResponse = (struct ResponseFormat *)malloc(sizeof(struct ResponseFormat));

    httpResponse->httpVersion = strdup(httpVersion);
    httpResponse->headers = NULL;
    httpResponse->responseBody = '\0';
    httpResponse->statusCode = SC_INTERNAL_SERVER_ERROR;
    httpResponse->contentType = "text/plain";
    httpResponse->bodyLength = 0;

    return httpResponse;
}

void addStatusCode(struct ResponseFormat *response, int statusCode)
{
    response->statusCode = statusCode;
}

void addContentType(struct ResponseFormat *response, char *contentType)
{
    response->contentType = strdup(contentType);
}

void addHeader(struct ResponseFormat *response, char *key, char *value)
{
    if (response->headers == NULL)
    {
        response->headers = create(key, value);
    }
    else
    {
        insert(&response->headers, key, value);
    }
}

void displayResponse(struct ResponseFormat *response)
{
    fprintf(stdout, "httpVersion: %s\nstatus code: %d\n", response->httpVersion, response->statusCode);
    displayAllHeaders(response->headers);
    fputc('\n', stdout);
    fprintf(stdout, "%s\n", response->responseBody);
}

void addBody(struct ResponseFormat *response, char *body)
{
    response->responseBody = strdup(body);
    response->bodyLength = strlen(body);
}

char *prepareResponse(struct ResponseFormat *response)
{
    // intial size of response, is changed later on
    int buffersize = 100;

    char *responseBuffer = (char *)malloc(buffersize);
    int length = snprintf(responseBuffer, buffersize, "%s %s\r\n", response->httpVersion, getHttpResponseString(response->statusCode));

    if (length >= buffersize)
    {
        buffersize = length + 1024;
        responseBuffer = (char *)realloc(responseBuffer, buffersize);
    }

    struct Header *current = response->headers;

    while (current != NULL)
    {
        if (buffersize - 1 <= length + strlen(current->header_key) + strlen(current->header_value) + 7)
        {
            buffersize = length + strlen(current->header_key) + strlen(current->header_value) + 7;
            responseBuffer = realloc(responseBuffer, buffersize);
        }
        length += snprintf(responseBuffer + length, buffersize - length, "%s: %s\r\n", current->header_key, current->header_value);
        current = current->header_next;
    }

    buffersize = length + strlen("Content-Length: \r\n") + sizeof(response->bodyLength) / 8 + 8;
    responseBuffer = realloc(responseBuffer, buffersize);
    length += snprintf(responseBuffer + length, buffersize - length, "Content-Length: %lu\r\n", response->bodyLength);

    buffersize = length + strlen("Content-Type: \r\n") + strlen(response->contentType) + 4;
    responseBuffer = realloc(responseBuffer, buffersize);
    length += snprintf(responseBuffer + length, buffersize - length, "Content-Type: %s\r\n", response->contentType);

    length += snprintf(responseBuffer + length, buffersize - length, "\r\n");
    if (buffersize - 1 <= length + response->bodyLength)
    {
        buffersize = length + response->bodyLength + 4;
        responseBuffer = realloc(responseBuffer, buffersize);
    }

    if (response->responseBody)
    {
        length += snprintf(responseBuffer + length, buffersize - length, "%s\r\n", response->responseBody);
    }

    fprintf(stdout, "%d\n%ld\n", buffersize, strlen(responseBuffer));
    return responseBuffer;
}

void freeResponse(struct ResponseFormat *response)
{
    free(response->httpVersion);
    free(response->contentType);
    free(response->responseBody);
    freelist(response->headers);
    free(response);
}