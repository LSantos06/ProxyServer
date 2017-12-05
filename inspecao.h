#ifndef inspecao_h 
#define inspecao_h

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

/* Janela inpecao */
// exibe a janela de inspecao
void janela_inspecao();

/* Inspecao das requisicoes */
// intercepta a requisicao antes da mesma ser enviada pelo proxy
void intercepta_requisicao(RequestORResponse *c_request);
// edita um campo da requisicao
void edita_campo_requisicao();
// exibicao da janela de requisicao
void janela_requisicao();
// manda a requisicao para o proxy
void proxy_envia();

/* Inspecao das respostas */
// intercepta a resposta antes da mesma ser entregue ao browser
void intercepta_resposta(RequestORResponse *s_response);
// edita um campo da resposta
void edita_campo_resposta();
// exibicao da janela de resposta
void janela_resposta();
// manda a reposta para o browser
void entrega_browser();

RequestORResponse* getRequestORResponseFields(char *buffer);
char* getRequestORResponseMessage(RequestORResponse *requestORresponse);
void freeRequestORResponseFiedls(RequestORResponse *requestORresponse);
HeaderList* createHeaderList();
HeaderList* insertHeaderList(HeaderList *list, char *headerFieldName, char *value);
int emptyHeaderList(HeaderList *list);
void freeHeaderList(HeaderList *list);
void printHeaderList(HeaderList *list);

#endif