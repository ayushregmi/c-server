// formats the input request
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "headers.h"
#include "request.h"

#define BUFFERSIZE 1024

// reads until a new line character is found
// fd -> file descriptor, line -> pointer to store the line, offset -> offset position, maxLineSize: max number of bytes in the line
int readLine(FILE *file, char *line, int offset, int maxLineSize)
{
    size_t bytesRead = 0;

    if (fseek(file, offset, SEEK_SET) < 0)
    {
        return -1;
    }

    while (bytesRead < maxLineSize - 1)
    {
        size_t n = fread(line + bytesRead, 1, 1, file);
        if (n < 0)
        {
            return -1;
        }

        if (line[bytesRead] == '\n')
        {
            line[bytesRead] = '\0';
            bytesRead++;
            return bytesRead;
        }
        bytesRead++;
    }
}

void displayRequest(struct RequestFormat *request)
{
    fprintf(stdout, "Method:%s\nURI:%s\nVersion:%s\n\n", request->methodType, request->uri, request->httpVersion);

    displayAllHeaders(request->headers);

    fputc('\n', stdout);
    fprintf(stdout, "%s\n", request->body);
}

// Generate HttpRequest from request
struct RequestFormat *RequestFormatFromFile(FILE *file)
{
    char buffer[BUFFERSIZE];
    int offset = 0;

    struct RequestFormat *httpRequest = malloc(sizeof(struct RequestFormat));
    memset(httpRequest, 0, sizeof(struct RequestFormat));

    // FILE *file = fdopen(fileDescriptor, "r");

    // Reading the request information
    offset = readLine(file, buffer, offset, BUFFERSIZE);

    char method[20], uri[100], httpVersion[10], key[128], value[128];

    int t = sscanf(buffer, "%s %s %s", method, uri, httpVersion);

    httpRequest->uri = uri;
    httpRequest->httpVersion = httpVersion;
    httpRequest->methodType = method;

    // Reading the headers and storing in a linked list
    int numBytes = 100;
    struct Header *headers = NULL;

    while (numBytes > 0)
    {
        numBytes = readLine(file, buffer, offset, BUFFERSIZE);
        t = sscanf(buffer, "%19[^:]: %[^\n]", key, value);

        offset += numBytes;

        if (buffer[0] == '\r')
        {
            break;
        }

        if (headers == NULL)
        {
            headers = create(key, value);
        }
        else
        {
            insert(&headers, key, value);
        }
    }

    httpRequest->headers = headers;

    // Reading the body of the request
    char *bodyBuffer;

    if (strcmp("GET", method) == 0)
    {
        httpRequest->body = '\0';
    }
    else
    {
        // getting the size of the file
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        // moving to the offset position
        fseek(file, offset, SEEK_SET);

        long bytesToRead = atoi(search(httpRequest->headers, "Content-Length")->header_value);
        fprintf(stdout, "%ld\n", bytesToRead);

        if (bytesToRead == 0)
        {
            httpRequest->body = '\0';
        }
        else
        {
            bodyBuffer = (char *)malloc(bytesToRead + 1);
            memset(bodyBuffer, 0, bytesToRead + 1);

            if (bodyBuffer == NULL)
            {
                perror("malloc() failed");
            }

            size_t bytesRead = fread(bodyBuffer, 1, bytesToRead, file);

            if (bytesRead != bytesToRead)
            {
                perror("fread() failed");
            }
            else
            {
                bodyBuffer[bytesRead] = '\0';
                httpRequest->body = (char *)malloc(bytesRead + 1);

                strncpy(httpRequest->body, bodyBuffer, bytesToRead);
            }

            free(bodyBuffer);
        }
    }

    return httpRequest;
}

// void createRequest(char *request)
// {

//     struct RequestFormat *httpRequest = (struct RequestFormat *)malloc(sizeof(struct RequestFormat));

//     size_t bufferSize = 1024;
//     char requestBuffer[bufferSize];
//     char *start = request;
//     char *end;
//     size_t requestSize = strlen(request);
//     size_t offset = 0;

//     // Reading Request line
//     char method[10], uri[50], version[20];
//     end = strstr(request, "\r\n");
//     offset = end - start;
//     strncpy(requestBuffer, start, offset);
//     sscanf(requestBuffer, "%s %s %s", method, uri, version);

//     httpRequest->methodType = method;
//     httpRequest->uri = uri;
//     httpRequest->httpVersion = version;

//     fprintf(stdout, "%s\n%s\n%s\n", method, uri, version);
// }

void freerequest(struct RequestFormat *request)
{
    if (request == NULL)
        return;

    free(request->body);
    freelist(request->headers);
    free(request);
}