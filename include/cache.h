
/**
* @file cache.h
* @brief Arquivo com as instrucoes da cache.
*/

#include "estruturas.h"
#define MAX_CACHE_SIZE (10<<20) /* 10MB */
#define MAX_OBJECT_SIZE (1<<20) /* 1MB */

struct list_cache {
    char * data; /**< Codigo HTML*/
	int size;  /**< Tamanho de dados*/
	long int lru_time_track; /* Usado recentemente */
	char * url;	/**< Identifica objeto da cache pela url*/
	struct list_cache * next;
};

typedef struct list_cache cache_object;

pthread_rwlock_t lock;

/**
*	@fn int add_to_cache(char * data, int site_size, char* url);
*   @brief Funcao para adicionar um objeto cache da cache.
*   @param data dado da pagina em si.
*   @param site_size, tamanho do site que sera armazenado.
*   @param url string que corresponde a url que sera armazena na cache.
*	@return Retorna 1 se deu certo, zero se deu errado.
*/
int add_to_cache(char * data, int site_size, char* url);

/**
*	@fn void remove_from_cache();
*   @brief Funcao para remover um objeto cache da cache.
*/
void remove_from_cache();
/**
*   @fn cache_object* cache_find(char * url);
*   @brief Funcao para procurar um objeto cache de acordo com a url.
*   @param url string que corresponde a url que sera procurada na lista de objetos de cache.
*   @return Objeto cache referente a URL ou NULL se nao foi encontrado.
*/
cache_object* cache_find(char * url);

/* Head of linked list cache_objects */
cache_object * head  ;/**<	Cabeca da lista de objetos cache*/
int cache_size; /**< Variavel global com o tamanho da cache*/
long int global_time;
