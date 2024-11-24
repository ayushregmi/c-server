struct endpoints
{
    char *uri;
    char *type;
    void (*handler)(struct RequestFormat *, struct ResponseFormat *);

    struct endpoints *next;
};

struct endpoints *createEndpoint(char *uri, char *type, void (*handler)(struct RequestFormat *, struct ResponseFormat *));
void addEndpoint(struct endpoints *endpoint, char *uri, char *type, void (*handler)(struct RequestFormat *, struct ResponseFormat *));
struct endpoints *getEndpoint(struct endpoints *endpoint, char *uri, char *type);