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

//TODO: DEFINIR BACKLOG E TAMBEM SE TERA UMA PORTA PADRAO OU NAO

// numero de conexoes pendentes na fila
#define BACKLOG 20

//TODO: Alocacao dinamica
// tamanho do buffer
#define BUFFER_SIZE 6000

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

    printf("\n=_=_=_=_=_=_=_=_\n");
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
        perror("Socket nao valido, valor negativo retornado");
        exit(-2);
    }

    /* Associando uma porta a socket */
    // int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
    //  - retorna negativo se der erro 
    int auxERRO = bind(num_socket,(struct sockaddr *)&server_end,sizeof(server_end));
    if(auxERRO < 0){
        perror("Erro ao associar porta, valor negativo retornado");
        exit(-3);
    }		

    /* Preparaco para conexoes */
    // int bind(int sockfd, int backlog);
    //  - retorna negativo se der erro 
    auxERRO = listen(num_socket,BACKLOG);
    if(auxERRO < 0){
        perror("Erro ao escutar a porta, valor negativo retornado");
        exit(-4);
    }		

    /* Aceitando a socket */	
    // accept(num_socket, (struct sockaddr *)&server_end, sizeof(struct sockaddr));
    //  - retorna negativo se der erro 
    int sin_size = sizeof(client_end);
    int new_accept_sock = accept(num_socket,(struct sockaddr *)&client_end,&sin_size);
    if(auxERRO < 0){
        perror("Erro ao aceitar a socket, valor negativo retornado");
        exit(-5);
    }	

    /* Instanciacao dos campos do cabecalho, requisicao e dados */
    char buffer[BUFFER_SIZE];
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
            perror("Erro ao receber dados da socket aceita, valor negativo retornado");
            exit(-6);
        }
        else{
            printf("\n=_=_=_=_=_=_=_=_\n");
            printf("Resposta recebida da socket aceita\n");
            printf("\n=_=_=_=_=_=_=_=_\n");
            puts(buffer);
        }

        /* Preenche as structs com os campos */
        printf("\n=_=_=_=_=_=_=_=_\n");
        // buffer para manipulacao
        char buffer_token[BUFFER_SIZE];  
        strcpy(buffer_token, buffer);
        // vetor de linhas
        int i=0;
        char *linhas[BUFFER_SIZE];
        // separacao do buffer em linhas
        char *token;
        token = strtok(buffer_token, "\n");
        // preenchimento da linha de requisicao
        linhas[i] = token; 
        //printf("%s\n", linhas[i]);
        // preenchimento do vetor de linhas de cabecalho
        while(token != NULL){
            i++;
            token = strtok(NULL, "\n");
            linhas[i] = token; 
            //printf("%s\n", linhas[i]);
        }
        i=0;

        /* Linha de requisicao */
        // COMANDO
        token = strtok(linhas[i], " ");
        cabecalho.requisicao.comando = token;
        printf("Comando: %s\n", cabecalho.requisicao.comando);
        // URL
        token = strtok(NULL, " ");
        cabecalho.requisicao.url = token;
        printf("URL: %s\n", cabecalho.requisicao.url);
        // VERSAO
        token = strtok(NULL, " ");
        cabecalho.requisicao.versao = token;
        printf("Versao: %s\n", cabecalho.requisicao.versao);  

        /* Linhas de cabecalho */
        while(linhas[i] != NULL){
            i++;
            token = strtok(linhas[i], ":");
            //printf("%s\n", token);

            if(!strcmp(token,"Host")){
                token = strtok(NULL, "\n");
                cabecalho.host = token;
                printf("Host: %s\n", cabecalho.host);
            }
            if(!strcmp(token,"User-Agent")){
                token = strtok(NULL, "\n");
                cabecalho.user_agent = token;
                printf("User-agent: %s\n", cabecalho.user_agent);
            }
            if(!strcmp(token,"Accept")){
                token = strtok(NULL, "\n");
                cabecalho.accept = token;
                printf("Accept: %s\n", cabecalho.accept);
            }
            if(!strcmp(token,"Accept-Encoding")){
                token = strtok(NULL, "\n");
                cabecalho.accept_encoding = token;
                printf("Accept-Encoding: %s\n", cabecalho.accept_encoding);
            }
            if(!strcmp(token,"Accept-Language")){
                token = strtok(NULL, "\n");
                cabecalho.accept_language = token;
                printf("Accept-Language: %s\n", cabecalho.accept_language); 
            }
            if(!strcmp(token,"Cookie")){
                token = strtok(NULL, "\n");
                cabecalho.cookie = token;
                printf("Cookie: %s\n", cabecalho.cookie); 
            }
            if(!strcmp(token,"Connection")){
                token = strtok(NULL, "\n");
                cabecalho.connection = token;
                printf("Connection: %s\n", cabecalho.connection);
            }

        }

        close(new_accept_sock);

        /*
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
        */
    }    

    return 0;
}
