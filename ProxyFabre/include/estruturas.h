
/**
* @file estruturas.h
* @brief Arquivo principal com a funcao main com servico de proxy.
*/
#ifndef estruturas_h 
#define estruturas_h


#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>


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

/**
*   @fn int main(int argc, char *argv[]);
*   @brief Funcao principal do programa.
*   @param argc Quantidade de argumentos passados pela linha de comando.
*   @param argv[] Argumentos passados pela linha de comando.
*   @return Status de execucao.
*/
int main(int argc, char *argv[]);

// Thread that handles pairs request/reponse

/**
*   @fn void *connectionHandler(void *);
*   @brief Atraves de thread maneja pares requesicoes e respostas.
* 	@param c_pNewSocketFD descritor da socket do servidor. 
*/
void *connectionHandler(void * c_pNewSocketFD);

// General functions headers
/**
*   @fn RequestORResponse* getRequestORResponseFields(char *buffer)
*   @brief Parse da string buffer guarda os campos de HTTP em uma struct.
* 	@param buffer -> string que tem os dados enviados pelo browser. 
*	@return RequestORResponse estrutura que armazena as strings do cabecalho e formato HTTP.
*/
RequestORResponse* getRequestORResponseFields(char *buffer);

/**
*   @fn char* getRequestORResponseMessage(RequestORResponse *requestORresponse)
*   @brief Pega estrutura que tem os campos separados e monta uma string de requisicao.  
* 	@param requestORresponse estrutura que sera usada para montar a requisicao ou mensagem de resposta. 
*	@return String no formato correto para envio de requisicao.
*/
char* getRequestORResponseMessage(RequestORResponse *requestORresponse);

/**
*   @fn void freeRequestORResponseFiedls(RequestORResponse *requestORresponse);
*   @brief Libera a memoria da estrutura que foi passada com parametro.  
* 	@param requestORresponse estrutura que sera liberada.
*/
void freeRequestORResponseFiedls(RequestORResponse *requestORresponse);

/**
*   @fn char * search_host(RequestORResponse * c_request);
*   @brief Ṕrocura o valor de host dado a estrutura.  
* 	@param c_request estrutura que sera usada para procurar o valor do host. 
*	@return String com o valor do host ou NULL.
*/
char * search_host(RequestORResponse * c_request);

/**
*   @fn void handle_client(int c_newSocketFD);
*   @brief Maneja a conexao especifica de um cliente.
* 	@param c_newSocketFD descritor da socket do cliente. 
*/
void handle_client(int c_newSocketFD);

// HeaderList function headers

/**
*   @fn HeaderList* createHeaderList();
*   @brief Aloca um ponteiro de estutura HeaderList.
*	@return Ponteiro para HeaderList ja inicializada.
*/
HeaderList* createHeaderList();

/**
*   @fn HeaderList* insertHeaderList(HeaderList *list, char *headerFieldName, char *value)
*   @brief Insere na lista de cabecalho de acordo com o headerFieldName e a string value.
* 	@param list ponteiro para estrutura que contem os cabecalhos e seus valores.
* 	@param headerFieldName String com o nome do campo do cabecalho.
* 	@param Value valor do campo.
*	@return Ponteiro para HeaderList que contem o novo elemento inserido.
*/
HeaderList* insertHeaderList(HeaderList *list, char *headerFieldName, char *value);

/**
*   @fn int emptyHeaderList(HeaderList *list);
*   @brief Verificia se a lista de cabecalho esta vazia.
* 	@param list ponteiro para estrutura que contem os cabecalhos e seus valores.
*	@return 1 se estiver vazio, 0 se estiver com elementos.
*/
int emptyHeaderList(HeaderList *list);

/**
*   @fn void freeHeaderList(HeaderList *list);
*   @brief Libera memoria da estrutura.
* 	@param list ponteiro para estrutura que contem os cabecalhos e seus valores.
*/
void freeHeaderList(HeaderList *list);

/**
*   @fn void printHeaderList(HeaderList *list);  
*   @brief Printa tudo que esta na lista de cabecalho.
* 	@param list ponteiro para estrutura que contem os cabecalhos e seus valores.
*/
void printHeaderList(HeaderList *list);  

/**
*   @fn void get_1_line(char * first_l, char * buffer);
*   @brief Pega a primeira linha de uma requisicao.
* 	@param first_l String que ter apenas o valor da primeira linha.
* 	@param buffer String que tem todo conteudo do buffer.
*/
void get_1_line(char * first_l, char * buffer);

/**
*   @fn void get_status(char * status, char * buffer);
*   @brief Pega o status da primeira linha de uma requisicao.
* 	@param first_l String que ter apenas o valor do codigo de status.
* 	@param buffer String que tem todo conteudo do buffer.
*/
void get_status(char * status, char * buffer);

/**
*   @fn int verify_status(char * status);
*   @brief Verifica o status esta OK (2xx).
* 	@param status String que tem o codigo de status.
*	@return 1 se comeca com 2 e 0 caso contrario.
*/
int verify_status(char * status);

#endif
