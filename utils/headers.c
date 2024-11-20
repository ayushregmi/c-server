// Linked list to store the request and response headers

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "headers.h"

// Create a linked list for headers
struct Header *create(char *key, char *value)
{
    struct Header *header = malloc(sizeof(struct Header));

    header->header_key = strdup(key);
    header->header_next = NULL;
    header->header_value = strdup(value);

    return header;
}

// insert the header at the end of the list
void insert(struct Header **head, char *key, char *value)
{
    struct Header *temp = *head;

    while (temp->header_next != NULL)
    {
        temp = temp->header_next;
    }

    struct Header *newHeader = create(key, value);
    temp->header_next = newHeader;
}

// search a header
struct Header *search(struct Header *head, const char *key)
{
    struct Header *current = head;

    while (current != NULL)
    {
        if (strcmp(current->header_key, key) == 0)
            return current;
        current = current->header_next;
    }

    return NULL;
}

// display header
void displayHeader(struct Header *header)
{
    fputs(header->header_key, stdout);
    fputs(" : ", stdout);
    fputs(header->header_value, stdout);
    fputc('\n', stdout);
}

// display all header
void displayAllHeaders(struct Header *header)
{

    struct Header *current = header;

    while (current != NULL)
    {
        displayHeader(current);
        current = current->header_next;
    }
}

// Deallocate memory for linkedlist
void freelist(struct Header *head)
{
    struct Header *current = head;

    while (current != NULL)
    {
        struct Header *next = current->header_next;
        free(current->header_key);
        free(current->header_value);
        free(current);
        current = next;
    }
}