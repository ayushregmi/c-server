

struct RequestFormat
{
    char *methodType;
    char *uri;
    char *httpVersion;

    struct Header *headers;

    char *body;
};

struct RequestFormat *RequestFormatFromFile(FILE *file);
// void createRequest(char *request);
void displayRequest(struct RequestFormat *request);
void freerequest(struct RequestFormat *request);