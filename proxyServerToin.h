#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>
#include <pthread.h>	
/*
 Erros:
    -1 = numero da porta
    -2 = descritor socket negativo
    -3 = erro associacao
    -4 = erro escutar
    -5 = erro aceitacao
    -6 = erro dados
*/

#define erro_porta -1
#define erro_socket -2
#define erro_opt -3
#define erro_bind -3
#define erro_list -4
#define erro_acc -5
#define erro_dados -6
#define erro_thr -7

//TODO: DEFINIR BACKLOG E TAMBEM SE TERA UMA PORTA PADRAO OU NAO

// numero de conexoes pendentes na fila
#define BACKLOG 20

//TODO: Alocacao dinamica
// tamanho do buffer
#define BUFFER_SIZE 65480

typedef struct headerList
{
    char *headerFieldName;
    char *value;
    struct headerList *next;
} HeaderList;

// Request/Response message structure
typedef struct requestORresponse
{
    char *methodORversion; // request:method,response:version
    char *urlORstatusCode; // request:url,response:statusCode
    char *versionORphrase; // request:version,response:phrase
    HeaderList *headers;
    char *body;
} RequestORResponse;



void *connectionHandler(void *c_pNewSocketFD);
void handle_cliente(int client_socket_fd);
RequestORResponse* getRequestORResponseFields(char*);
char* getRequestORResponseMessage(RequestORResponse*);
void freeRequestORResponseFiedls(RequestORResponse*);
HeaderList* createHeaderList();
HeaderList* insertHeaderList(HeaderList*, char*, char*);
int emptyHeaderList(HeaderList*);
void freeHeaderList(HeaderList*);
void printHeaderList(HeaderList*);
char * printHeaderList_filtro(HeaderList *list, char * filtro);
void decodifica_1_linha(char * buffer,char * p_linha);
void decodifica_status(char * buffer,char * p_linha);
int verifica_status(char * p_code);
void do_proxy(char* host, char buffer[], int client_socket_fd);
