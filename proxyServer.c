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
struct sockaddr_in endereco,client_end;

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
if(validade_socket < 0)
{
	perror("Socket nao valido, valor negativo retornado");
	exit(0);
}
// tendo a socket, devemos associar ela a uma porta
int aux1;
//int bind(int sockfd, struct sockaddr *my_addr, int addrlen); 
aux1 = bind(num_socket,(struct sockaddr *)&server_end,sizeof(server_end));
if(aux1 < 0)
	{
	perror("Èrro ao associar porta, valor negativo retornado");
	exit(0);
	}		

listen(num_socket,BACKLOG);
// TODO :: CONTINUAR

}
