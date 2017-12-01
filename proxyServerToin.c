#include "proxyServerToin.h"


int main(int argc, char* argv[])
{

    if(argc < 2){
        perror("Para executar ./proxy <numero_da_porta> \n Lembrando que eh porta utilizada pelo IP do LoopBack");
        exit(erro_porta);
    }

    printf("\n=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_\n");
    printf("Comecando a escutar a conexao\n");
     /* Instanciacao dos campos do cabecalho, requisicao e dados */
    char buffer[BUFFER_SIZE];
    int client_size,new_accept_sock,c_opt =1 ;
   
    struct sockaddr_in server_end,cliente;
    
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
        exit(erro_socket);
    }
    // Optional: helps in reuse of address and port, prevents error such as “address already in use”
    if(setsockopt(num_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &c_opt, sizeof(c_opt)) < 0) // == -1
    {
        perror("Set Sock Opt failhou");
        exit(erro_opt);
    }
   
    /* Associando uma porta a socket */
    // int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
    //  - retorna negativo se der erro 
    int auxERRO = bind(num_socket,(struct sockaddr *)&server_end,sizeof(server_end));
    if(auxERRO < 0){
        perror("Erro ao associar porta, valor negativo retornado");
        exit(erro_bind);
    }       
    puts("Bind foi um Sucesso");
    /* Preparaco para conexoes */
    // int bind(int sockfd, int backlog);
    //  - retorna negativo se der erro 
    auxERRO = listen(num_socket,BACKLOG);
    if(auxERRO < 0){
        perror("Erro ao escutar a porta, valor negativo retornado");
        exit(erro_list);
    }       
    //---------------------------------------------//
    puts("Pronto para começar a escutar conexoes \n");
    
    pthread_t thre[5];
    int i=0;
    while(i < 5){
    	auxERRO = pthread_create(&thre[i], NULL,&connectionHandler,&num_socket);
    	if(auxERRO < 0){
	       printf("Erro nas threads");
	        exit(erro_thr);
   	 	} 
   		 i++;
    }
    for (i = 0; i < 5; i++) {
    	pthread_join(thre[i], NULL);
  	}
      printf("\n [%d] Pronto para começar a escutar conexoes \n",i); 

return 0;
}



void *connectionHandler(void *c_pNewSocketFD)
{
   	while(1){
   	 struct sockaddr_in cliente;	
   	 int tamanho = 	sizeof(cliente);
   	 int newSocketFD;
   	 newSocketFD = accept((*(int*)c_pNewSocketFD), (struct sockaddr *)&cliente,&tamanho);
   	 if(newSocketFD < 0)
   	 {
   	 	perror("Erro ao aceitar \n\n");
   	 	
   	 }else{
   	 	printf("Conexao aceita e estabelecida \n");
   	 	handle_cliente(newSocketFD);
   	 }
   	 printf("\n\n Esperando outras conexoes \n");
   	}	
}

void handle_cliente(int client_socket_fd) {

	char buffer[BUFFER_SIZE];
	char buffer_copy[BUFFER_SIZE];
	memset(buffer,0,BUFFER_SIZE);
	memset(buffer_copy,0,BUFFER_SIZE);
	int dado;
	RequestORResponse *c_request = NULL;
   
	dado = recv(client_socket_fd,buffer,BUFFER_SIZE,0);
	if(dado < 0){
		perror("Erro ao receber dados \n");
		close(client_socket_fd);
		return;
	}if(dado == 0){
    perror("Cliente encerrou a conexao, sem dados \n");
  }

	printf("Received request:\n");
  puts(buffer);
	char * aux = strdup(buffer);
	c_request = getRequestORResponseFields(aux);
    printf("\n\n------------------------------------");
    printf("\n\nmethodORversion: %s, urlORstatusCode: %s, versionORphrase: %s",c_request->methodORversion,c_request->urlORstatusCode,c_request->versionORphrase);
    printHeaderList(c_request->headers);
    printf("\nbody: %s\n\n",c_request->body);
    char * http_t = strdup(c_request->versionORphrase);
    if((strncmp("GET",c_request->methodORversion,3)!= 0) ||
     ((strncmp("HTTP/1.0",c_request->versionORphrase,8) != 0) && (strncmp("HTTP/1.1",c_request->versionORphrase,8) != 0)))
 	{   
 		memset(buffer,0,BUFFER_SIZE);
 		printf("Method %s\n",c_request->methodORversion);
 		printf("Versao %s\n",c_request->versionORphrase);
       	sprintf(buffer,"405 Metodo errado");
     	printf("Metodo errado\n");
     	send(client_socket_fd, buffer, strlen(buffer), 0);
  	 	close(client_socket_fd);
      return;
  	 	
 	}
 	// pegando o valor da url em si 
 	char URL[BUFFER_SIZE],requestHTTP[BUFFER_SIZE],host_request[BUFFER_SIZE];
 	char host_url[256];
 	char * strptr;

 	strcpy(URL,c_request->urlORstatusCode);
	strptr = strtok(URL, ":");
  	strptr = strtok(NULL, ":");
  	strcpy(host_url, strptr);
 	strcpy(host_request, strptr);
 	
 	printf("Host: %s\n", host_url);
    printf("HOSTRequest: %s\n", host_request);
  
	strptr = strtok(host_url, "//");
 	strcpy(host_url,strptr);  
 	
 	printf("HostRequest: %s\n", host_request);
    printf("Request: %s\n", requestHTTP);
  
  	strcpy(requestHTTP, host_request + 2 + strlen(host_url)); 
  	
  	printf("Host: %s\n", host_url);
    printf("Request: %s\n", requestHTTP);
  
  	
  	strcpy(buffer_copy, buffer);
  	memset(buffer, 0, BUFFER_SIZE); 
    if((strncmp("HTTP/1.0",http_t,8)==0)){
    	sprintf(buffer, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n", requestHTTP, host_url);
	}else{
		sprintf(buffer, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", requestHTTP, host_url);
	}
    // ao fim obteve-se
    printf("Host: %s\n", host_url);
    printf("Request: %s\n", requestHTTP);
    printf("Buffer:%s\n",buffer);
  
    do_proxy(host_url,buffer,client_socket_fd);
}

RequestORResponse* getRequestORResponseFields(char *buffer)
{

    RequestORResponse *requestORresponse = (RequestORResponse *)malloc(sizeof(RequestORResponse));
    int aux = 0, contentLength = 0;
    char auxBuffer[BUFFER_SIZE], *pch = NULL, *pch2 = NULL;

    strcpy(auxBuffer,buffer);
    // Obtaining method
    pch = strtok(auxBuffer," ");
    requestORresponse->methodORversion = (char *)malloc(strlen(pch)*sizeof(char)+1);
    strcpy(requestORresponse->methodORversion,pch);
    // Obtainig url
    pch = strtok(NULL," ");
    requestORresponse->urlORstatusCode = (char *)malloc(strlen(pch)*sizeof(char)+1);
    strcpy(requestORresponse->urlORstatusCode,pch);
    // Obtaining version
    pch = strtok(NULL,"\n");
    requestORresponse->versionORphrase = (char *)malloc(strlen(pch)*sizeof(char)+1);
    strcpy(requestORresponse->versionORphrase,pch);
    // Obtaining list of headers
    requestORresponse->headers = createHeaderList();
    while(pch != NULL)
    {
      pch = strtok(NULL," ");
      if(pch != NULL){
            
         
          if(pch[1] == '\n')
          {
              pch[strlen(pch)] = ' ';
              break;
          }
          pch[strlen(pch)-1] = '\0';
          pch2 = pch;
          pch = strtok(NULL,"\n");
          pch[strlen(pch)-1] = '\0';
          if(pch != NULL)
              requestORresponse->headers = insertHeaderList(requestORresponse->headers,pch2,pch);
          // Saving body length if necessary
          if(!strcmp(pch2,"Content-Length"))
              contentLength = atoi(pch);
      }
    }

    // If body not empty, obtaining it
    if(contentLength > 0)
    {
        requestORresponse->body = (char *)malloc(contentLength*sizeof(char));
        for(aux=0;aux<contentLength;aux++){
            requestORresponse->body[aux] = *((char*)(pch+2+aux));
            printf("%c", requestORresponse->body[aux]);
        }
        requestORresponse->body[aux] = '\0';
    }
    else
        requestORresponse->body = NULL;

    printf("Printando - %s\n",requestORresponse->body);
    return requestORresponse;
}


// Inserts new element at the lists end
HeaderList* insertHeaderList(HeaderList *list, char *headerFieldName, char *value)
{
    HeaderList *aux = NULL;
    HeaderList *new = (HeaderList *)malloc(sizeof(HeaderList));

    new->headerFieldName = (char *)malloc(strlen(headerFieldName)*sizeof(char)+1);
    strcpy(new->headerFieldName,headerFieldName);
    new->value = (char *)malloc(strlen(value)*sizeof(char)+1);
    strcpy(new->value,value);
    new->next = NULL;
    if(list != NULL)
    {
        for(aux = list; aux->next != NULL; aux = aux->next);
        aux->next = new;
    }
    else
        list = new;

    return list;
}

// Verifies if lists empty
int emptyHeaderList(HeaderList *list)
{
    if(list == NULL)
        return 1;
    else
        return 0;
}

// Deallocates list
void freeHeaderList(HeaderList *list)
{
    if(!emptyHeaderList(list))
    {
        freeHeaderList(list->next);
        free(list);
    }
}

// Print list
void printHeaderList(HeaderList *list)
{
    if(!emptyHeaderList(list))
    {
        printf("\nheaderFieldName: %s | value: %s",list->headerFieldName,list->value);
        printHeaderList(list->next);
    }
}
HeaderList* createHeaderList()
{
    return NULL;
}

char * printHeaderList_filtro(HeaderList *list, char * filtro)
{	HeaderList *auxHeaderList = NULL;
    for(auxHeaderList=list;auxHeaderList!=NULL;auxHeaderList=auxHeaderList->next)
    {
        if(!strcmp(auxHeaderList->headerFieldName,"Host"))
           return auxHeaderList->value;
           	break;
    }

    return NULL;
}

void do_proxy(char* host, char buffer[], int client_socket_fd){

	int host_fd;
	host_fd = socket(AF_INET,SOCK_STREAM,0);
	if(host_fd < 0){
	   perror("Socket nao valido, valor negativo retornado");
       memset(buffer,0,BUFFER_SIZE);
       sprintf(buffer,"Errorrr 500");
       send(client_socket_fd, buffer, strlen(buffer), 0);
  	   close(client_socket_fd);
       return;
    }
    struct sockaddr_in host_end;
    host_end.sin_family = AF_INET;
    host_end.sin_port = htons(80);

    struct hostent * host_name;
    host_name = gethostbyname(host);

    if(host_name == NULL){
	    printf("Falha ao encontrar host.\n"); 
	    memset(buffer, 0, BUFFER_SIZE);
   		sprintf(buffer, "404 Not Found.\n");
        send(client_socket_fd, buffer, strlen(buffer), 0);
  	    close(client_socket_fd);
        return;
    }
    bcopy(host_name->h_addr,&host_end.sin_addr.s_addr,host_name->h_length);

    if(connect(host_fd,(const struct sockaddr *) &host_end, sizeof(struct sockaddr)) < 0)
    {
       perror("connect nao valido, valor negativo retornado");
       memset(buffer,0,BUFFER_SIZE);
       sprintf(buffer,"Errorrr 502");
       send(client_socket_fd, buffer, strlen(buffer), 0);
  	   close(client_socket_fd);
       return;	
    }
	printf("Finalmente conectado ao host \n");
	if(send(host_fd,buffer,strlen(buffer),0) < 0)
    { 
       perror("Envio erro, valor negativo retornado");
       memset(buffer,0,BUFFER_SIZE);
       sprintf(buffer,"Errorrr 500");
       send(client_socket_fd, buffer, strlen(buffer), 0);
  	   close(client_socket_fd);
       return;	
    }
	printf("Request enviado  \n");
	int bytes_rcv;
	int total_dados =0;
	int primeira_vez=1;
	do{
		printf("ENTROUUUUUUUUUU\n");
		puts(buffer);
       memset(buffer,0,BUFFER_SIZE);
       bytes_rcv = recv (host_fd,buffer,BUFFER_SIZE,0); 
       if((bytes_rcv < 0) && (primeira_vez))
       {
       		perror("Envio erro, valor negativo retornado");
      		memset(buffer,0,BUFFER_SIZE);
       		sprintf(buffer,"Error 500");
       		send(client_socket_fd, buffer, strlen(buffer), 0);
  	   		close(client_socket_fd);
       		return;
       }
       if(primeira_vez){
	       	char dec_1_linha[256];
	       	char dec_status[256];
	       	decodifica_1_linha(buffer,dec_1_linha);
	       	decodifica_status(buffer,dec_status);
	       	primeira_vez = 0;
	       	int ver_ax = verifica_status(dec_status);
	        if(ver_ax == 0){
	         		memset(buffer,0,BUFFER_SIZE);
       		 	  sprintf(buffer, "%s\n", dec_1_linha);
       		   	printf("Taqui\n");
              puts(dec_1_linha);
       			  puts(buffer);
       			  send(client_socket_fd, buffer, strlen(buffer), 0);
  	   			  close(client_socket_fd);
  	   	  return;
	       }
       	}
       	 if(send(client_socket_fd,buffer,strlen(buffer),0) < 0){
       		perror("Envio erro, valor negativo retornado");
       		puts(buffer);
       		sprintf(buffer,"Error 503");
       		send(client_socket_fd, buffer, strlen(buffer), 0);
  	   		close(client_socket_fd);
       		return;
       	}
       	if(!bytes_rcv){
       		printf("Nao tem mais dados acabou conexao");
       		memset(buffer,0,BUFFER_SIZE);
          return;
       	}


       total_dados += bytes_rcv;
	}while(bytes_rcv > 0);


	close(host_fd);
	close(client_socket_fd);
}	


void decodifica_1_linha(char * buffer,char * p_linha)
{
	char * resp = strdup(buffer);
	char * aux = strtok(resp, "\n");
	strcpy(p_linha,aux);
	printf("Alo %s\n",p_linha);
}

void decodifica_status(char * buffer,char * p_linha)
{
	char * resp = strdup(buffer);
	char * aux = strtok(resp, " ");
	aux = strtok(NULL," ");
	strcpy(p_linha,aux);
	printf("Status %s\n",p_linha);

}

int verifica_status(char * p_code){
	if(p_code[0] == '2'){
		//printf("FGHJKJHGFDSDFGBNM,MNBVCX\n");
	 //	printf("%c\n",p_code[0]);
		return 1;
	}else{
		return 0;
	}
	
}
