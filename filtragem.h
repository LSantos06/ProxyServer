#ifndef filtragem_h 
#define filtragem_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SIZE_LISTA 30
#define MAX_STR 500

FILE* abrindo_arquivo(char* nome_arquivo);
int checkLists(char* nome_arquivo,char * mensagem);
// talvez seja melhor receber a mensagem toda para que depois possa analizar o deny terms
int  filtragem_url(char * url);
void remove_http(char* url_http);
int denyterms_request(char * request);
int * Length_denyterms(void);
#endif
