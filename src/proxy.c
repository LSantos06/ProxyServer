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
#include "../include/filtragem.h"


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
// Main
int main(int argc, char *argv[])
{
    // c_: about client-proxy interface (proxy is server)
    int c_serverFD = 0, c_opt = 1;
    int c_newSocketFD = 0, *c_pNewSocketFD = NULL;
    struct sockaddr_in c_serverAddr;
    int c_addrLen = sizeof(struct sockaddr_in);
    pthread_t v_thr[4];
    int aux_erro,i=0;

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
    
    while(i < 4){
        aux_erro = pthread_create(&v_thr[i],NULL,&connectionHandler,(void *) &c_serverFD);
        if(aux_erro < 0){
            puts("Failed do create thread");
        }    
    i++;
    }
    for(i =0;i<4;i++){
        pthread_join(v_thr[i],NULL);
    }
    puts("waiting for incoming connections...");


    return 0;
}

// Handles each client connection
void *connectionHandler(void *c_pNewSocketFD)
{
    // c_: about client-proxy interface (proxy is server)
    int c_newSocketFD = *(int*)c_pNewSocketFD;
 
    int c_addrLen = sizeof(struct sockaddr_in);
    struct sockaddr_in c_clientAddr;
    while(1){
        c_newSocketFD = accept(*(int *) c_pNewSocketFD, (struct sockaddr *) &c_clientAddr, &c_addrLen);
        if(c_newSocketFD < 0){
            puts("Failed to accept");
        }else{
            puts("Conection accept and established");
                handle_client(c_newSocketFD);
        }

        puts("Waiting for another connections");                         
    }
    

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

char * search_host(RequestORResponse * c_request){
    HeaderList *auxHeaderList = NULL;

    for(auxHeaderList = c_request->headers;auxHeaderList != NULL;auxHeaderList = auxHeaderList->next)
     {
            if(!strcmp(auxHeaderList->headerFieldName,"Host"))
               return auxHeaderList->value;
     }
     return NULL;
}        


void get_1_line(char * first_l, char * buffer){
    char * aux_b = strdup(buffer);
    char * str_aux;
    str_aux = strtok(aux_b, "\n");
    strcpy(first_l,str_aux);
}

void get_status(char * status, char * buffer){
    char * aux_b = strdup(buffer);
    char * str_aux;
    str_aux = strtok(aux_b, " ");
    str_aux = strtok(NULL, " ");
    strcpy(status,str_aux);
}

int verify_status(char * status){

    if(status[0] == '2')
        return 1;
    else
        return 0;        
}

void handle_client(int c_newSocketFD){

    RequestORResponse *c_request = NULL;
    // s_: about proxy-server interface (proxy is client)
    int s_clientFD = 0, s_opt = 1, s_yes = 1, s_idle = 1, s_interval = 1, s_maxpkt = 10;
    RequestORResponse *s_response = NULL;
    struct sockaddr_in s_serverAddr;
    // Common variables
    HeaderList *auxHeaderList = NULL;
    struct hostent *he;
    struct in_addr **addr_list;
    int aux = 0, contentLength = 0;
    char buffer[BUFFER+1] = {0};
    int qnt=0;
    // Receiving request message from browser client
                if((aux = recv(c_newSocketFD, buffer, sizeof(buffer),0)) < 0) // read(c_newSocketFD, buffer, sizeof(buffer)) == -1
                {
                    printf("recv failed 1 | %d\n",c_newSocketFD);
                    return;
                }
                printf("recv succeded 1 | %d\n",c_newSocketFD);

                if(aux > 0) // != 0
                {
                    // Obtaining fields from request message
                    //puts(buffer);
                    c_request = getRequestORResponseFields(buffer);
                    printf("\n\nmethodORversion: %s, urlORstatusCode: %s, versionORphrase: %s",c_request->methodORversion,c_request->urlORstatusCode,c_request->versionORphrase);
                    printHeaderList(c_request->headers);
                    printf("\nbody: %s\n\n",c_request->body);
                    
                    char * aux_host = search_host(c_request);
                    printf("aux host -- %s\n",aux_host );
                    // filtragem_url --> 0 ta na black list ou tem deny terms
                    // se 1 -> esta white list    
                    printf("filtragem %d ",filtragem_url(aux_host));
                    printf("Fazendo a filtragem ......\n");
                    if(filtragem_url(aux_host) == 0){
                           // EM TESE EH PROIBIDO  
                           printf("Host esta na black list ou contem termos proibidos na URL\n"); 
                           printf("Fechando conexao\n"); 
                           memset(buffer,0,BUFFER);
                           sprintf(buffer, "<html><body>Blocked website!<br><br></body></html>");
                           send(c_newSocketFD, buffer, strlen(buffer), 0);
                           close(c_newSocketFD);  
                         return; 
                     }    
                    // Creating proxy client socket file descriptor
                    if((strncmp("GET",c_request->methodORversion,3)!=0)
                     ||(
                        (strncmp("HTTP/1.0",c_request->versionORphrase,8)!=0)
                            &&
                            (strncmp("HTTP/1.1",c_request->versionORphrase,8)!=0)))
                    {
                           memset(buffer,0,BUFFER);
                           sprintf(buffer, "405 Forbidden %s %s.\n",c_request->methodORversion,c_request->versionORphrase);
                           send(c_newSocketFD, buffer, strlen(buffer), 0);
                           close(c_newSocketFD);     
                           return;
                    }
                    memset(buffer,0,BUFFER);
                    strcpy(buffer,getRequestORResponseMessage(c_request));
    
                    if ((s_clientFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) // == -1
                    {
                        printf("socket failed | %d\n",c_newSocketFD);
                        memset(buffer,0,BUFFER);
                        sprintf(buffer, "Erro interno.\n");
                        send(c_newSocketFD, buffer, strlen(buffer), 0);
                        close(c_newSocketFD);
                        return;
                       
                    }
                    printf("socket succeded | %d\n",c_newSocketFD);

                    // Optional: helps in reuse of address and port, prevents error such as “address already in use”
                    if(setsockopt(s_clientFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &s_opt, sizeof(s_opt)) < 0) // == -1
                    {
                        printf("setsockopt failed | %d\n",c_newSocketFD);
                        memset(buffer,0,BUFFER);
                        sprintf(buffer, "Erro interno.\n");
                        send(c_newSocketFD, buffer, strlen(buffer), 0);
                        close(c_newSocketFD);
                       return;
                        
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
                        memset(buffer,0,BUFFER);
                        sprintf(buffer, "404 not found.\n");
                        send(c_newSocketFD, buffer, strlen(buffer), 0);
                        close(c_newSocketFD);
                        return;

                       
                    }
                    printf("gethostbyname succeded | %d\n",c_newSocketFD);

                    // Preparing www server sockaddr_in structure
                    s_serverAddr.sin_family = AF_INET;
                    s_serverAddr.sin_port = htons(80);

                    // Searching through IP list and requesting proxy-server connection
                    addr_list = (struct in_addr **) he->h_addr_list;
                    for(aux = 0; addr_list[aux] != NULL; aux++) 
                    {
                        s_serverAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*addr_list[aux]));
                        if(connect(s_clientFD, (struct sockaddr *)&s_serverAddr, sizeof(struct sockaddr_in)) >= 0) // != -1
                        {
                            printf("connect succeded | %d\n",c_newSocketFD);
                            break;
                        }
                    }
                    if(addr_list[aux] == NULL)
                    {
                        printf("connect failed | %d\n",c_newSocketFD);
                        memset(buffer,0,BUFFER);
                        sprintf(buffer, "Erro ao conectar.\n");
                        send(c_newSocketFD, buffer, strlen(buffer), 0);
                        close(c_newSocketFD);
                        return;
                    }

                    // Sending request message from proxy to www
                    if((aux = send(s_clientFD, buffer, strlen(buffer), 0)) < 0) // write(s_clientFD, s_message, strlen(s_message)) == -1
                    {
                        printf("send failed 1 | %d\n",c_newSocketFD);
                        memset(buffer,0,BUFFER);
                        sprintf(buffer, "Erro interno ao enviar.\n");
                        send(c_newSocketFD, buffer, strlen(buffer), 0);
                        close(c_newSocketFD);
                        return;
                     }
                    printf("send succeded 1 | %d\n",c_newSocketFD);

                    // Receiving response message from www server
                    int num_bytes=0,first_line=1;
                    do{
                       memset(buffer,0,BUFFER); 
                    num_bytes = recv(s_clientFD, buffer, sizeof(buffer), 0);
                        if((num_bytes < 0) && first_line) // read(s_clientFD, buffer, sizeof(s_buffer) == -1
                        {
                            printf("recv failed 2 | %d\n",c_newSocketFD);
                            memset(buffer,0,BUFFER);
                            sprintf(buffer, "Erro interno ao enviar.\n");
                            send(c_newSocketFD, buffer, strlen(buffer), 0);
                            close(c_newSocketFD);
                            return;
                        }
                    // pegando primeira linha 
                        if(first_line){
                            first_line = 0;
                            char first_l[256], status[256];
                            get_1_line(first_l,buffer);
                            get_status(status,first_l);
                        
                            puts(first_l);

                            if(verify_status(status) == 0){
                               // se nao eh 2xx -- errado 
                                memset(buffer,0,BUFFER);
                                sprintf(buffer, "%s\n",first_l);
                                send(c_newSocketFD, buffer, strlen(buffer), 0);
                                close(c_newSocketFD);
                                return;
                            }
                        }
                             // resposta do host agr   
                             if(send(c_newSocketFD, buffer, num_bytes, 0) ==-1)
                             {
                                    printf("Falha ao receber dados \n");
                                    sprintf(buffer, "erro servico sem dados ");
                                    send(c_newSocketFD, buffer, strlen(buffer), 0);
                                    close(c_newSocketFD);
                                    return;
                             }
                             printf("ENVIANDO DADOS \n");
                             // agora ja recebeu os dados 
                             if(num_bytes == 0){
                                // cabou a conexao
                                printf("Fim conexao, sem dados\n");
                                memset(buffer,0,BUFFER);
                                break;
                             }

                        qnt+=num_bytes;
                   
                    }while(num_bytes > 0);
                    printf("Quantidade de bytes recebida %d \n",qnt);       
 
                           
                close(s_clientFD);
                // Closing browser client socket
                close(c_newSocketFD);
            }
                               
printf("end | %d\n",c_newSocketFD);
        

}
