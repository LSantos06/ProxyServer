
/**
* @file filtragem.h
* @brief Arquivo com as operacoes de filtragem.
*/
#ifndef filtragem_h
#define filtragem_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "estruturas.h"

#define SIZE_LISTA 30
#define MAX_STR 500
#define MAX_DADO 6000
#define NOT_FILTERED 4
#define DENY_ERRO 3
#define  ERRO_LOG  2
#define ACCEPT_LOG 1
/**
*   @fn FILE* abrindo_arquivo(char* nome_arquivo);
*   @brief Funcao para abrir um arquivo.txt .
*   @param nome_arquivo corresponde ao nome do arquivo que tera que ser lido.
*   @return Ponteiro para FILE* ja aberto.
*/
FILE* abrindo_arquivo(char* nome_arquivo);
/**
*   @fn int checkLists(char* nome_arquivo,char * mensagem);
*   @brief Funcao que checa se uma mensagem esta em um determinado arquivo.
*   @param nome_arquivo corresponde ao nome do arquivo sera lido.
*   @param mensagem que sera procurada no arquivo.
*   @return 1 se for encontrado, zero se nao for encontrado.
*/
int checkLists(char* nome_arquivo,char * mensagem);
// talvez seja melhor receber a mensagem toda para que depois possa analizar o deny terms
/**
*   @fn int  filtragem_url(char * url);
*   @brief Funcao que checa se uma  url se encontra na whitelist, na blacklist, denyterms ou se nao esta nem um destes arquivos.
*   @param url eh a url que sera filtrada.
*   @return 1 se for encontrado no denyterms.txt ou blacklist.txt, caso contrario 0.
*/
int  filtragem_url(char * url);
/**
*   @fn int denyterms_request(char * request);
*   @brief Funcao que checa se em uma url tem algum termo proibido.
*   @param request eh a url que sera verificada.
*   @return 1 se for encontrado no denyterms.txt, caso contrario 0.
*/
int denyterms_request(char * request);
/**
*   @fn int denyterms_body(char * body, char * url);
*   @brief Funcao que checa se em um corpo tem algum termo proibido.
*   @param body corpo que sera verificado.
*   @param url que eh passada para funcao de LOG caso tenha termos proibidos.
*   @return 1 se for encontrado no denyterms.txt, caso contrario 0.
*/
int denyterms_body(char * body, char * url);
/**
*   @fn FILE* abrindo_log(char* nome_arquivo);
*   @brief Funcao para abrir um arquivo<log>.txt .
*   @param nome_arquivo corresponde ao nome do arquivo que tera que ser lido.
*   @return Ponteiro para FILE* ja aberto.
*/
FILE* abrindo_log(char* nome_arquivo);
/**
*   @fn int * Length_denyterms(void);
*   @brief Funcao para guardar a quantidade de elementos e o tamanho de cada elemento do arquivo denyterms.txt.
*   @return Ponteiro para FILE* ja aberto.
*/
int * Length_denyterms(void);
/**
*   @fn void mensagem_log(char * url, int opcao);
*   @brief Funcao para escrever um log - com data, url e se foi aceita ou rejeitada.
*   @param url que sera colocada no log.
*   @param opcao indica se eh do modo aceito <whitelist ou nao proibido> ou rejeitado <blacklist, denyterms>.
*/
void mensagem_log(char * url, int opcao);

/**
*   @fn void mensagem_log_body(char * url, char * dado);
*   @brief Funcao para escrever um log - com data, url e se foi rejeitada no corpo.
*   @param url que sera colocada no log.
*   @param dado string que foi responsavel por ser rejeitada.
*/
void mensagem_log_body(char * url, char * dado);


#endif

