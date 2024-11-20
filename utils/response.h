#include <stdio.h>

#define SC_OK 200
#define SC_BAD_REQUEST 400
#define SC_UNAUTHORIZED 401
#define SC_FORBIDDEN 403
#define SC_NOT_FOUND 404
#define SC_INTERNAL_SERVER_ERROR 500

struct ResponseFormat
{
    char *httpVersion;
    int statusCode;

    struct Header *headers;

    char *responseBody;
    char *contentType;
    size_t bodyLength;
};

struct ResponseFormat *createResponse(char *httpVersion);
void addHeader(struct ResponseFormat *response, char *key, char *value);
void addBody(struct ResponseFormat *response, char *body);
void addStatusCode(struct ResponseFormat *response, int statusCode);
char *prepareResponse(struct ResponseFormat *response);
void addContentType(struct ResponseFormat *response, char *contentType);

void freeResponse(struct ResponseFormat *response);

void displayResponse(struct ResponseFormat *response);
