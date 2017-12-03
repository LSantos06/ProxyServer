#ifndef filtragem_h 
#define filtragem_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define SIZE_LISTA 30
#define MAX_STR 500
#define MAX_DADO 6000
#define  ERRO_LOG -1
#define ACCEPT_LOG 1

FILE* abrindo_arquivo(char* nome_arquivo);
int checkLists(char* nome_arquivo,char * mensagem);
// talvez seja melhor receber a mensagem toda para que depois possa analizar o deny terms
int  filtragem_url(char * url);
int denyterms_request(char * request);
int denyterms_body(char * body, char * url);
FILE* abrindo_log(char* nome_arquivo);
int * Length_denyterms(void);
void mensagem_log(char * url, int opcao);
void mensagem_log_body(char * url, char * dado);


#endif