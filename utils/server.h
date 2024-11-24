struct Server
{
    char *hostName;
    char *port;
    struct endpoints *endpointList;
};

struct Server *getServer(char *hostName, char *port);
void startServer(struct Server *server);