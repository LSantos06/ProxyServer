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

#define BACKLOG 20     // how many pending connections queue will hold

int main(int argc, char* argv[]){
// TODO: DEFINIR BACKLOG E TAMBEM SE TERA UMA PORTA PADRAO OU NAO
if(argc < 2)
{
	perror("Para executar ./proxy <numero_da_porta> \n Lembrando que eh porta utilizada pelo IP do LoopBack");
	exit(0);
}
struct sockaddr_in endereco,server_end,client_end;

printf("Começando a escutar a conexao\n");
/*
FLUXO DAS FUNCOES 
socket();
bind();
listen();
 accept() goes here 
 */ 

//botando os valores na struct da sock addr
// padrao
server_end.sin_family = AF_INET;
// pegando a porta passada como parametro e associando ao server
server_end.sin_port = htons(atoi(argv[1]));
// utilizando o loopback como endereco
server_end.sin_addr.s_addr =  inet_addr("127.0.0.1");
// o resto da struct nao interessa
memset(&(server_end.sin_zero), '\0', 8); // zero the rest of the struct
int num_socket;
//int socket(int domain, int type, int protocol); 
num_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
// retorna um valor para descrever a socket
if(num_socket < 0)
{
	perror("Socket nao valido, valor negativo retornado");
	exit(0);
}
// tendo a socket, devemos associar ela a uma porta
int auxERRO;
//int bind(int sockfd, struct sockaddr *my_addr, int addrlen); 
auxERRO = bind(num_socket,(struct sockaddr *)&server_end,sizeof(server_end));
if(auxERRO < 0)
	{
	perror("Èrro ao associar porta, valor negativo retornado");
	exit(0);
	}		

auxERRO = listen(num_socket,BACKLOG);
if( auxERRO < 0)
	{
	perror("Èrro ao escutar a porta\n");
	exit(0);
	}		
int sin_size = sizeof(client_end);
int new_accept_sock;
char buffer[6000];

// TODO :: CONTINUAR
// aceitando a socket 	
new_accept_sock = accept(num_socket,(struct sockaddr *)&server_end,&sin_size);
//new_accept_sock = connect(num_socket, (struct sockaddr *)&server_end, sizeof(struct sockaddr));

/* TODO : CONTINUAR 
 * RELATIVE : Executando uma vez, recebe o metodo GET ... Mas nao acessa a pagina	
 * NEW_SOCKET_NUM que esta mostrando o buffer
 * DEPOIS O PROGRAMA FECHA 

if( recv(num_socket, buffer, 6000 , 0) < 0)
    {
        printf("falhou socket nao aceita \n\n");
    }else{
    printf("Resposta recebida da socket nao aceita\n");
    puts(buffer);	
	}
if( recv(new_accept_sock, buffer, 6000 , 0) < 0)
    {
        printf("recv failed da socket aceita");
    }else{
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
    return 0;

}
