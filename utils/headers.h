
struct Header
{
    char *header_key;
    char *header_value;
    struct Header *header_next;
};

struct Header *create(char *key, char *value);
void insert(struct Header **head, char *key, char *value);
struct Header *search(struct Header *head, const char *key);
void displayHeader(struct Header *header);
void displayAllHeaders(struct Header *header);
void freelist(struct Header *header);
