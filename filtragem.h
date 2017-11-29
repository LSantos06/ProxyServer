#ifndef filtragem_h 
#define filtragem_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SIZE_LISTA 30
#define MAX_STR 500
#define MAX_DADO 6000

FILE* abrindo_arquivo(char* nome_arquivo);
int checkLists(char* nome_arquivo,char * mensagem);
int  filtragem_url(char * url);
int denyterms_request(char * request);
int denyterms_body(char * request);
FILE* abrindo_log(char* nome_arquivo);
int * Length_denyterms(void);
void mensagem_log(char * url, int opcao);