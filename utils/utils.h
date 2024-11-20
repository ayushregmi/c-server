#include <sys/socket.h>

void DieWithSystemMessage(const char *message);
void ContinueWithSystemMessage(const char *message);
void DieWithUserMessage(const char *message, const char *detail);
void ContinueWithUserMessage(const char *message, const char *detail);

void PrintSocketAddress(struct sockaddr *address, FILE *stream);