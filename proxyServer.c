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

/*
 Erros:
    -1 = numero da porta
    -2 = descritor socket negativo
    -3 = erro associacao
    -4 = erro escutar
    -5 = erro aceitacao
    -6 = erro dados
*/

// TODO: DEFINIR BACKLOG E TAMBEM SE TERA UMA PORTA PADRAO OU NAO

// numero de conexoes pendentes na fila
#define BACKLOG 20

/*
 Struct para armazenar os campos da linha de requisicao
*/
typedef struct campos_requisicao{
    char *comando;
    char *url;
    char *versao;

} Requisicao;

/*
 Struct para armazenar os campos do cabecalho
*/
typedef struct campos_header{
    Requisicao requisicao;
    char *host;
    char *user_agent;
    char *accept;
    char *accept_encoding;
    char *accept_language;        
    char *cookie;
    char *connection;

} Header;

int main(int argc, char* argv[]){

    if(argc < 2){
        perror("Para executar ./proxy <numero_da_porta> \n Lembrando que eh porta utilizada pelo IP do LoopBack");
        exit(-1);
    }

    printf("Comecando a escutar a conexao\n");

    struct sockaddr_in endereco, server_end, client_end;
    
    /* 
     Fluxo das funcoes: 
        socket();
        bind();
        listen();
        accept(); 
    */ 

    /* Associando os valores a struct sokadr_in server_send */
    // padrao
    server_end.sin_family = AF_INET;
    // pegando a porta passada como parametro e associando ao server
    server_end.sin_port = htons(atoi(argv[1]));
    // utilizando o loopback como endereco
    server_end.sin_addr.s_addr =  inet_addr("127.0.0.1");
    // zerando o resto da struct
    memset(&(server_end.sin_zero), '\0', 8); 
    
    /* Criacao da socket */
    // int socket(int domain, int type, int protocol)
    //  - retorna um valor para descrever a socket
    int num_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(num_socket < 0){
        perror("Socket nao valido, valor negativo retornado\n");
        exit(-2);
    }

    /* Associando uma porta a socket */
    // int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
    //  - retorna negativo se der erro 
    int auxERRO = bind(num_socket,(struct sockaddr *)&server_end,sizeof(server_end));
    if(auxERRO < 0){
        perror("Erro ao associar porta, valor negativo retornado\n");
        exit(-3);
    }		

    /* Preparaco para conexoes */
    // int bind(int sockfd, int backlog);
    //  - retorna negativo se der erro 
    auxERRO = listen(num_socket,BACKLOG);
    if(auxERRO < 0){
        perror("Erro ao escutar a porta, valor negativo retornado\n");
        exit(-4);
    }		

    // TODO :: CONTINUAR

    /* Aceitando a socket */	
    // accept(num_socket, (struct sockaddr *)&server_end, sizeof(struct sockaddr));
    //  - retorna negativo se der erro 
    int sin_size = sizeof(client_end);
    int new_accept_sock = accept(num_socket,(struct sockaddr *)&client_end,&sin_size);
    if(auxERRO < 0){
        perror("Erro ao aceitar a socket, valor negativo retornado\n");
        exit(-5);
    }	

    // TODO :: CONTINUAR

    /*  
     RELATIVE : Executando uma vez, recebe o metodo GET ... Mas nao acessa a pagina	
     NEW_SOCKET_NUM que esta mostrando o buffer
     DEPOIS O PROGRAMA FECHA 

    if(recv(num_socket, buffer, 6000 , 0) < 0){
        printf("falhou socket nao aceita \n\n");
    }
    else{
        printf("Resposta recebida da socket nao aceita\n");
        puts(buffer);	
    }
    if(recv(new_accept_sock, buffer, 6000 , 0) < 0){
        printf("recv failed da socket aceita");
    }
    else{
        printf("Resposta recebida da socket aceita\n");
        puts(buffer);
    }
        
    // tentando verificar um padrao 
    char GET[10],URL[120],HTTP[15],HOST[10];
    sscanf(buffer,"%s %s %s %s",GET,URL,HTTP,HOST);
    printf("GET == %s\n", GET);
    printf("URL == %s\n", URL);
    printf("HTTP == %s\n", HTTP);
    printf("HOST == %s\n",HOST);
    
    */ 

    /* Instanciacao dos campos do cabecalho, requisicao e dados */
    char buffer[6000];
    Requisicao linha_requisicao = {
        .comando = "0",
        .url = "0",
        .versao = "0",
    };
    Header cabecalho = {
        .requisicao = linha_requisicao,
        .host = "0",
        .user_agent = "0",
        .accept = "0",
        .accept_encoding = "0",
        .accept_language = "0",
        .cookie = "0",
        .connection = "0"
    };

    // USANDO pid para passar o cliente para o processo filho
    struct hostent * host;
    pid_t pid = fork();

    if(!pid){
        if(recv(new_accept_sock, buffer, 6000 , 0) < 0){
            printf("Erro ao receber dados da socket aceita, valor negativo retornado\n");
            exit(-6);
        }
        else{
            printf("Resposta recebida da socket aceita\n");
            puts(buffer);
        }
        
        /* Preenche as structs com os campos */

        // TODO: depois modularizar para alterar e pa 
        
        char GET[10],URL[120],HTTP[15],HOST_fix[10],HOST[100];
        sscanf(buffer,"%s %s %s %s %s",GET,URL,HTTP,HOST_fix,HOST);
        printf("GET == %s\n", GET);
        printf("URL == %s\n", URL);
        printf("HTTP == %s\n", HTTP);
        
        if((strcmp(GET,"GET")!=0)|| ((strcmp(HTTP,"HTTP/1.0"))!= 0 && (strcmp(HTTP,"HTTP/1.1")))){
            // SE FOR DIFERENTE DE GET = 1
            // SE NAO FOR HTTP1.0 e NEM HTTP1.1 = (PORTA E)1 
            printf("Cabecalho nao corresponde ao padrao ... \n");
            // TODO: fazer log de erro .... ou enviar mensagem de erro
            close(new_accept_sock);
        } 
        // esta eh a primeira linha sempre sera comecara com isso
        printf("HOST == %s\n",HOST_fix);
        printf("valor do HOST = %s\n",HOST);
        
    }    

    return 0;
}
