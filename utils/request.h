
struct RequestFormat
{
    char *methodType;
    char *uri;
    char *httpVersion;

    struct Header *headers;

    char *body;
};

struct RequestFormat *RequestFormatFromFile(FILE *file);
void displayRequest(struct RequestFormat *request);
void freerequest(struct RequestFormat *request);