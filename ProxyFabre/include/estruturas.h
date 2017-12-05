#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BACKLOG 20 // How many pending connections queue will hold
#define BUFFER 32768 // Buffer size, máx message size


// Header line list structure
typedef struct headerList
{
    char *headerFieldName;
    char *value;
    struct headerList *next;
} HeaderList;

// Request/Response message structure
typedef struct requestORresponse
{
    char *methodORversion; // request:method,response:vesion
    char *urlORstatusCode; // request:url,response:statusCode
    char *versionORphrase; // request:version,response:phrase
    HeaderList *headers;
    char *body;
} RequestORResponse;

// Thread that handles pairs request/reponse
void *connectionHandler(void *);

// General functions headers
RequestORResponse* getRequestORResponseFields(char*);
char* getRequestORResponseMessage(RequestORResponse*);
void freeRequestORResponseFiedls(RequestORResponse*);
char * search_host(RequestORResponse * c_request);
void handle_client(int c_newSocketFD);

// HeaderList function headers
HeaderList* createHeaderList();
HeaderList* insertHeaderList(HeaderList*, char*, char*);
int emptyHeaderList(HeaderList*);
void freeHeaderList(HeaderList*);
void printHeaderList(HeaderList*);  

void get_1_line(char * first_l, char * buffer);
void get_status(char * status, char * buffer);
int verify_status(char * status);

