#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void DieWithSystemMessage(const char *message)
{
    perror(message);
    exit(0);
}

void ContinueWithSystemMessage(const char *message)
{
    perror(message);
}

void DieWithUserMessage(const char *msg, const char *detail)
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputs(": ", stderr);
    fputc('\n', stderr);
    exit(0);
}

void ContinueWithUserMessage(const char *msg, const char *detail)
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputs(": ", stderr);
    fputc('\n', stderr);
}

void PrintSocketAddress(struct sockaddr *addr, FILE *stream)
{

    if (addr == NULL || stream == NULL)
        return;

    void *numericAddress;
    char addrBuffer[INET6_ADDRSTRLEN];

    in_port_t port;

    switch (addr->sa_family)
    {
    case AF_INET:
        numericAddress = &((struct sockaddr_in *)addr)->sin_addr;
        port = ntohs(((struct sockaddr_in *)addr)->sin_port);
        break;
    case AF_INET6:
        numericAddress = &((struct sockaddr_in6 *)addr)->sin6_addr;
        port = ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
        break;
    default:
        fputs("[unknown type]", stream);
        return;
        break;
    }

    if (inet_ntop(addr->sa_family, numericAddress, addrBuffer, sizeof(addrBuffer)) == NULL)
    {
        fputs("[invalid address]", stream);
        return;
    }
    else
    {
        char hostName[NI_MAXHOST];
        char serviceName[NI_MAXSERV];
        socklen_t addrLen = sizeof(addr);

        int rtnVal = getnameinfo(addr, sizeof(struct sockaddr_storage), hostName, NI_MAXHOST, serviceName, NI_MAXSERV, 0);

        if (rtnVal != 0)
        {
            DieWithUserMessage("getnameinfo() failed", gai_strerror(rtnVal));
        }

        fprintf(stream, "http://%s:%d\n", hostName, port);
    }
}