#ifndef inst_h 
#define inst_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SIZE_LISTA 30
#define MAX_STR 200

FILE* abrindo_arquivo(char* nome_arquivo);
int checkLists(char* nome_arquivo,char * mensagem);
// talvez seja melhor receber a mensagem toda para que depois possa analizar o deny terms
int  filtragem_mensagem(char * site_dest);
#endif
