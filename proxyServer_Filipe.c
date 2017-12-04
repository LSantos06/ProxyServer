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
#include "filtragem.h"


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

// HeaderList function headers
HeaderList* createHeaderList();
HeaderList* insertHeaderList(HeaderList*, char*, char*);
int emptyHeaderList(HeaderList*);
void freeHeaderList(HeaderList*);
void printHeaderList(HeaderList*);

// Main
int main(int argc, char *argv[])
{
    // c_: about client-proxy interface (proxy is server)
    int c_serverFD = 0, c_opt = 1;
    int c_newSocketFD = 0, *c_pNewSocketFD = NULL;
    struct sockaddr_in c_clientAddr, c_serverAddr;
    int c_addrLen = sizeof(struct sockaddr_in);
    
    // Argument check
    if(argc < 2)
    {
        puts("how to execute: ./caching <port_number>");
	    return 0;
    }
    if(argc > 2)
    {
        perror("too many arguments\n");
        exit(EXIT_FAILURE);
    }
    if(atoi(argv[1]) <= 1024 || atoi(argv[1]) >= 65535)
    {
        perror("invalid port argument\n");
        exit(EXIT_FAILURE);
    }

    // Creating proxy server socket file descriptor
    if((c_serverFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) // == -1
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    puts("socket succeded");

    // Optional: helps in reuse of address and port, prevents error such as “address already in use”
    if(setsockopt(c_serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &c_opt, sizeof(c_opt)) < 0) // == -1
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    puts("setsockport succeded");

    // Preparing proxy server sockaddr_in structure
    c_serverAddr.sin_family = AF_INET;
    c_serverAddr.sin_addr.s_addr = INADDR_ANY; // inet_addr("127.0.0.1");
    c_serverAddr.sin_port = htons(atoi(argv[1]));
    
    // Biding c_serverFD to c_serverAddr
    if(bind(c_serverFD, (struct sockaddr *)&c_serverAddr, c_addrLen) < 0) // == -1
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    puts("bind succeded");

    // Start listening for incoming conections requests
    if(listen(c_serverFD, BACKLOG) < 0) // == -1
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    puts("listen succeded");

    // Waiting or accepting incoming connections
    puts("waiting for incoming connections...");
    while((c_newSocketFD = accept(c_serverFD, (struct sockaddr *)&c_clientAddr, (socklen_t*)&c_addrLen)) >= 0) // != -1
    {
        puts("accept succeded");

        // Creating handler thread for accepted client-proxy connection
        pthread_t p_snifferThread;
        c_pNewSocketFD = (int*)malloc(sizeof(int));
        *c_pNewSocketFD = c_newSocketFD;
        if(pthread_create(&p_snifferThread, NULL, connectionHandler, (void*)c_pNewSocketFD) != 0)
        {
            printf("pthread_create failed | %d\n",c_newSocketFD);
            exit(EXIT_FAILURE);
        }
        printf("pthread_create succeded | %d\n",c_newSocketFD);

        // Now join the thread , so that we dont terminate before the thread
        //pthread_join(p_snifferThread, NULL);
    }

    // Accepting fail
    if(c_newSocketFD < 0) // == -1
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Closing proxy server socket
    close(c_serverFD);
    
    return 0;
}

// Handles each client connection
void *connectionHandler(void *c_pNewSocketFD)
{
    // c_: about client-proxy interface (proxy is server)
    int c_newSocketFD = *(int*)c_pNewSocketFD;
    RequestORResponse *c_request = NULL;
    // s_: about proxy-server interface (proxy is client)
    int s_clientFD = 0, s_opt = 1, s_yes = 1, s_idle = 1, s_interval = 1, s_maxpkt = 10;
    RequestORResponse *s_response = NULL;
    struct sockaddr_in s_serverAddr;
    // Common variables
    HeaderList *auxHeaderList = NULL;
    struct hostent *he;
    struct in_addr **addr_list;
    int auxInt1 = 0, auxInt2 = 0, auxInt3 = 0;
    char buffer[BUFFER+1] = {0};

    // Receiving request message from browser client
    if((auxInt1 = recv(c_newSocketFD, buffer, sizeof(buffer),0)) < 0) // read(c_newSocketFD, buffer, sizeof(buffer)) == -1
    {
        printf("recv failed 1 | %d\n",c_newSocketFD);
        exit(EXIT_FAILURE);
    }
    printf("recv succeded 1 | %d\n",c_newSocketFD);

    if(auxInt1 > 0) // != 0
    {
        // Obtaining fields from request message
        //puts(buffer);
        c_request = getRequestORResponseFields(buffer);
        printf("\n\nmethodORversion: %s, urlORstatusCode: %s, versionORphrase: %s",c_request->methodORversion,c_request->urlORstatusCode,c_request->versionORphrase);
        printHeaderList(c_request->headers);
        printf("\nbody: %s\n\n",c_request->body);

        // Creating proxy client socket file descriptor
        if ((s_clientFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) // == -1
        {
            printf("socket failed | %d\n",c_newSocketFD);
            exit(EXIT_FAILURE);
        }
        printf("socket succeded | %d\n",c_newSocketFD);

        // Optional: helps in reuse of address and port, prevents error such as “address already in use”
        if(setsockopt(s_clientFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &s_opt, sizeof(s_opt)) < 0) // == -1
        {
            printf("setsockopt failed | %d\n",c_newSocketFD);
            exit(EXIT_FAILURE);
        }
        printf("setsockport succeded | %d\n",c_newSocketFD);

        // Finding header containing server host name
        for(auxHeaderList = c_request->headers;auxHeaderList != NULL;auxHeaderList = auxHeaderList->next)
        {
            if(!strcmp(auxHeaderList->headerFieldName,"Host"))
                break;
        }

        // Requesting host name corresponding IP list
        if((he = gethostbyname(auxHeaderList->value)) == NULL)
        {
            printf("gethostbyname failed | %d\n",c_newSocketFD);
            exit(EXIT_FAILURE);
        }
        printf("gethostbyname succeded | %d\n",c_newSocketFD);

        // Preparing www server sockaddr_in structure
        s_serverAddr.sin_family = AF_INET;
        s_serverAddr.sin_port = htons(80);

        // Searching through IP list and requesting proxy-server connection
        addr_list = (struct in_addr **) he->h_addr_list;
        for(auxInt1 = 0; addr_list[auxInt1] != NULL; auxInt1++) 
        {
            s_serverAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*addr_list[auxInt1]));
            if(connect(s_clientFD, (struct sockaddr *)&s_serverAddr, sizeof(struct sockaddr_in)) >= 0) // != -1
            {
                printf("connect succeded | %d\n",c_newSocketFD);
                break;
            }
        }
        if(addr_list[auxInt1] == NULL)
        {
            printf("connect failed | %d\n",c_newSocketFD);
            exit(EXIT_FAILURE);
        }

        // Sending request message from proxy to www
        strcpy(buffer,getRequestORResponseMessage(c_request));
        if((auxInt1 = send(s_clientFD, buffer, strlen(buffer), 0)) < 0) // write(s_clientFD, s_message, strlen(s_message)) == -1
        {
            printf("send failed 1 | %d\n",c_newSocketFD);
            exit(EXIT_FAILURE);
        }
        printf("send succeded 1 | %d\n",c_newSocketFD);

        // Receiving response message from www server
        if((auxInt1 = recv(s_clientFD, buffer, sizeof(buffer), 0)) < 0) // read(s_clientFD, buffer, sizeof(s_buffer) == -1
        {
            printf("recv failed 2 | %d\n",c_newSocketFD);
            exit(EXIT_FAILURE);
        }
        printf("recv succeded 2 | %d\n",c_newSocketFD);

        // Obtaining fields from response message
        //puts(buffer);
        if(auxInt1 > 0) // != 0
        {
            for(auxInt2 = 1; auxInt2 <= auxInt1; auxInt2++)
            {
                if(buffer[auxInt2-1] == '\n' && buffer[auxInt2] == '\n')
                    auxInt3 = auxInt1 - auxInt2;
            }
            s_response = getRequestORResponseFields(buffer);
            printf("\n\nmethodORversion: %s, urlORstatusCode: %s, versionORphrase: %s",s_response->methodORversion,s_response->urlORstatusCode,s_response->versionORphrase);
            printHeaderList(s_response->headers);     
            printf("\nbody: %s\n\n",s_response->body);
            for(auxHeaderList = s_response->headers; auxHeaderList != NULL; auxHeaderList = auxHeaderList->next)
            {
                if(!strcmp(auxHeaderList->headerFieldName,"Content-Length"))
                {
                    //
                    while((auxInt1 = recv(s_clientFD, buffer, sizeof(buffer), 0)) > 0) // read(s_clientFD, buffer, sizeof(s_buffer)) != -1 && != 0
                    {
                        printf("recv succeded 3 | %d\n",c_newSocketFD);
                        for(auxInt2=0; auxInt2 < auxInt1; auxInt2++)
                            s_response->body[auxInt3 + auxInt2] = buffer[auxInt2];
                        auxInt3 += auxInt1;
                    }
                    if(auxInt1 < 0) // == -1
                    {
                        printf("recv failed 3 | %d\n",c_newSocketFD);
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
            }
        }

        

        // Sending response mesage from proxy to browser client
        strcpy(buffer,getRequestORResponseMessage(s_response));
        if(auxInt1 = send(c_newSocketFD, buffer, strlen(buffer), 0) < 0) // write(c_newSocket, buffer, strlen(buffer)) == -1
        {
            printf("send failed 2 | %d\n",c_newSocketFD);
            exit(EXIT_FAILURE);
        }
        printf("send succeded 2 | %d\n",c_newSocketFD);

        // Deallocating used memory
        //freeRequestORResponseFiedls(c_request);
        //freeRequestORResponseFiedls(s_response);
    }

    // Closing browser client socket
    close(c_newSocketFD);

    //Free the socket pointer
    free(c_pNewSocketFD);
    
    printf("end | %d\n",c_newSocketFD);

    return NULL;
}

// General functions

// Obtains fields from request/response message
RequestORResponse* getRequestORResponseFields(char *buffer)
{
    RequestORResponse *requestORresponse = (RequestORResponse *)malloc(sizeof(RequestORResponse));
    int aux = 0, contentLength = 0;
    char auxBuffer[BUFFER], *pch = NULL, *pch2 = NULL;

    strcpy(auxBuffer,buffer);
    // Obtaining method
    pch = strtok(auxBuffer," ");
    requestORresponse->methodORversion = (char *)malloc(strlen(pch)*sizeof(char)+1);
    strcpy(requestORresponse->methodORversion,pch);
    // Obtainig url
    pch = strtok(NULL," ");
    requestORresponse->urlORstatusCode = (char *)malloc(strlen(pch)*sizeof(char)+1);
    strcpy(requestORresponse->urlORstatusCode,pch);
    // Obtaining version
    pch = strtok(NULL,"\n");
    requestORresponse->versionORphrase = (char *)malloc(strlen(pch)*sizeof(char)+1);
    strcpy(requestORresponse->versionORphrase,pch);
    // Obtaining list of headers
    requestORresponse->headers = createHeaderList();
    while(pch != NULL)
    {
        pch = strtok(NULL," ");
        if(pch[1] == '\n')
        {
            pch[strlen(pch)] = ' ';
            break;
        }
        pch[strlen(pch)-1] = '\0';
        pch2 = pch;
        pch = strtok(NULL,"\n");
        pch[strlen(pch)-1] = '\0';
        if(pch != NULL)
            requestORresponse->headers = insertHeaderList(requestORresponse->headers,pch2,pch);
        // Saving body length if necessary
        if(!strcmp(pch2,"Content-Length"))
            contentLength = atoi(pch);
    }

    // If body not empty, obtaining it
    if(contentLength > 0)
    {
        requestORresponse->body = (char *)malloc(contentLength*sizeof(char)+1);
        for(aux=0;aux<contentLength;aux++)
            requestORresponse->body[aux] = *((char*)(pch+2+aux));
        requestORresponse->body[aux] = '\0';
    }
    else
        requestORresponse->body = NULL;

    return requestORresponse;
}

// Obtains request/response message from fields
char* getRequestORResponseMessage(RequestORResponse *requestORresponse)
{
    char *buffer = (char*)malloc(BUFFER*sizeof(char)+1);
    HeaderList *auxHeaderList = NULL;

    strcpy(buffer, requestORresponse->methodORversion);
    strcat(buffer, " ");
    strcat(buffer, requestORresponse->urlORstatusCode);
    strcat(buffer, " ");
    strcat(buffer, requestORresponse->versionORphrase);
    strcat(buffer, "\n");
    for(auxHeaderList=requestORresponse->headers;auxHeaderList!=NULL;auxHeaderList=auxHeaderList->next)
    {
        strcat(buffer, auxHeaderList->headerFieldName);
        strcat(buffer, ": ");
        strcat(buffer, auxHeaderList->value);
        strcat(buffer, "\r\n");
    }
    strcat(buffer, "\r\n");
    if(requestORresponse->body != NULL)
        strcat(buffer, requestORresponse->body);

    return buffer;
}

// Deallocates request/response fields
void freeRequestORResponseFiedls(RequestORResponse *requestORresponse)
{
    free(requestORresponse->methodORversion);
    free(requestORresponse->urlORstatusCode);
    free(requestORresponse->versionORphrase);
    freeHeaderList(requestORresponse->headers);
    free(requestORresponse->body);
    free(requestORresponse);

    return;
}

// HeaderList functions

// Creates new list
HeaderList* createHeaderList()
{
    return NULL;
}

// Inserts new element at the lists end
HeaderList* insertHeaderList(HeaderList *list, char *headerFieldName, char *value)
{
	HeaderList *aux = NULL;
	HeaderList *new = (HeaderList *)malloc(sizeof(HeaderList));

    new->headerFieldName = (char *)malloc(strlen(headerFieldName)*sizeof(char)+1);
    strcpy(new->headerFieldName,headerFieldName);
    new->value = (char *)malloc(strlen(value)*sizeof(char)+1);
    strcpy(new->value,value);
    new->next = NULL;
    if(list != NULL)
    {
        for(aux = list; aux->next != NULL; aux = aux->next);
        aux->next = new;
    }
    else
        list = new;

    return list;
}

// Verifies if lists empty
int emptyHeaderList(HeaderList *list)
{
    if(list == NULL)
        return 1;
    else
        return 0;
}

// Deallocates list
void freeHeaderList(HeaderList *list)
{
    if(!emptyHeaderList(list))
    {
        freeHeaderList(list->next);
        free(list);
    }
}

// Print list
void printHeaderList(HeaderList *list)
{
    if(!emptyHeaderList(list))
    {
        printf("\nheaderFieldName: %s | value: %s",list->headerFieldName,list->value);
        printHeaderList(list->next);
    }
}
