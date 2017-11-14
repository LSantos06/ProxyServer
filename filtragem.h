#ifndef filtragem_h 
#define filtragem_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SIZE_LISTA 30
#define MAX_STR 100

FILE* abrindo_arquivo(char* nome_arquivo);
int checkLists(char* nome_arquivo,char * mensagem);
// talvez seja melhor receber a mensagem toda para que depois possa analizar o deny terms
int  filtragem_mensagem(char * site_destino);
void remove_http(char* site_host);
int denyterms_request(char * request);
int * Length_denyterms();
#endif
