#include "filtragem.h"

#define ERRO_LOG -1
#define ACC_LOG 1

FILE* abrindo_arquivo(char* nome_arquivo){
  FILE *fp;
  fp = fopen(nome_arquivo, "r");
 if(fp == NULL){
  printf("Erro ao abrir o arquivo \n\n\n");
  exit(0);
 }
 return fp;
}

int checkLists(char* nome_arquivo,char * mensagem){
	
	char lista[SIZE_LISTA][MAX_STR];
	int i=0, saida = 0;
	char procurado[strlen(mensagem)];
 	// zerando a string
 	memset (procurado,'\0',strlen(mensagem));
	strcpy (procurado,mensagem);
	FILE * fp = abrindo_arquivo(nome_arquivo);
	int aux,cnt=0;


	while (i<SIZE_LISTA && fgets(lista[i],MAX_STR,fp)!=NULL)
	{
		aux = strlen(lista[i])-2;
		//printf(" %s - %d\n",lista[i],strlen(lista[i])-2);
			
		if (strncasecmp(procurado,lista[i],aux)==0)
		{
			saida = 1;
			
		}
		i++;
	} 
	// para ultimo pegar
	aux = strlen(lista[i-1]);
	if (strncasecmp(procurado,lista[i-1	],aux)==0)
		{
			saida = 1;
			
		}
	fclose(fp);
	return saida;
	}
//por enquanto retorna um int para indicar se ta ou nao ta proibido
int filtragem_url(char * url)
{
	// pre processamento do host ---- adicionando www.
	char host_www[500];
	char www[504]= "www.";
	if(strncmp("www.",url,4) != 0)
	{
		strcpy(host_www,url);
		strcat(www,host_www);
		strcpy(url,www);	
	}

	
	// primeiro verifica-se se esta na whiteList
	int aux_white,aux_black,aux_deny;
	aux_white = checkLists("whitelist.txt",url);
	aux_black= checkLists("blacklist.txt",url);
	// verificando se esta na url os termos proibidos
	aux_deny = denyterms_request(url);
	if(aux_white){
		//printf("\nWHITE LIST OK --- ENCAMINHAR MENSAGEM %s \n",url);
		mensagem_log(url,ACC_LOG);
		return 1;
	} // verificando se esta na lista negra
	else if(aux_black){
	   
		printf("\nESTA NA BLACK LIST  --- REJEITAR MENSAGEM %s \n",url);
		mensagem_log(url,ERRO_LOG);
		return 0;
	 }// se nao tiver em nenhuma da lista deve-se procurar por termos proibidos
	else if (aux_deny){
	 	// se for 1 entao tem termos proibidos na url 
		printf("\n\tdeny terms na URL  --- REJEITAR  MENSAGEM %s \n",url);
		mensagem_log(url,ERRO_LOG);
		return 0;	
	}else{
		//printf("\n\tNada %s \n",url);
		mensagem_log(url,ACC_LOG);
		return 1;
	}
	
	
}

int denyterms_request(char * request){
	char * aux_s = malloc(MAX_STR*sizeof(char));
	int * tamanho;
	tamanho = Length_denyterms();
	int i_aux=0,k=0,i=0,j=0,qnt;
	qnt	= tamanho[0];
	
	for(j=1;j<=qnt;j++)
	{
		for(i=0;i < strlen(request);i++)
		{
			memset (aux_s,'\0',MAX_STR);
			if((strlen(request)) - i < tamanho[j] ){
				break;							
			}
			for(k=0,i_aux=i;k<tamanho[j];k++ ,i_aux++)
			{
				if(request[i_aux] == '+' || request[i_aux] == '-'){
					aux_s[k] = ' ';	
				}else{
					aux_s[k] = request[i_aux];		
				}
			}		
		    
			//printf("%s - %d quita\n",aux_s,strlen(aux_s));
	       if (checkLists("denyterms.txt",aux_s))
		 	{
		 
		 	//printf("\nDENTRO IF \t\t DEBUG == %s /// %d\n",aux_s,tamanho[j]);
		 	free(aux_s);
		 	free(tamanho);
			 return 1;
			}
		
		}
		
	}
	free(aux_s);
	free(tamanho);
	return 0;
	
}

int * Length_denyterms(){
	
	// PADRAO INDICE ZERO - CONTEM A QUANTIDADE DE ELEMENTOS
	// O RESTO SAO OS VALORES 
	FILE * fp = abrindo_arquivo("denyterms.txt");
	char lista[SIZE_LISTA][MAX_STR];
	int * vetor = malloc(SIZE_LISTA*sizeof(int));
	
	int i=0;
	int j=1;
	while (i<SIZE_LISTA && fgets(lista[i],MAX_STR,fp)!=NULL)
	{
		vetor[j] = strlen(lista[i])-2;
		i++;j++;
	} 
	vetor[0] = i;  
	// para o ultimo termo do denyterms deve-se contar o ultimo elemento buga porcausa do EOF
	vetor[i]++;
	/*printf("\n OLA %d ----- %d\n",vetor[0],(strlen(lista[i])-1));
	 for(j = 0;j <= i; j++){
			printf("\n\t\tVetor[%d] = %d",j,vetor[j]);

		}
	*/
	fclose(fp);
	return vetor;
	
}

int denyterms_body(char * body,char *url){
	char * aux_s = malloc(MAX_DADO*sizeof(char));
	int * tamanho;
	tamanho = Length_denyterms();
	int i_aux=0,k=0,i=0,j=0,qnt;
	qnt	= tamanho[0];
	
	for(j=1;j<=qnt;j++)
	{
		for(i=0;i < strlen(body);i++)
		{
			memset (aux_s,'\0',MAX_STR);
			if((strlen(body)) - i < tamanho[j] ){
				break;							
			}
			for(k=0,i_aux=i;k<tamanho[j];k++ ,i_aux++)
			{
				aux_s[k] = body[i_aux];		
			}		
			
	       if (checkLists("denyterms.txt",aux_s))
		 	{
		 	//printf("\nDENTRO IF \t\t DEBUG == %s /// %d\n",aux_s,tamanho[j]);
		 	mensagem_log_body(url,aux_s);
		 	free(aux_s);
		 	free(tamanho);
			 return 1;
			}
		
		}
		
	}
	free(aux_s);
	free(tamanho);
	return 0;
	
}

FILE* abrindo_log(char* nome_arquivo){
  FILE *fp;
  fp = fopen(nome_arquivo, "a+");
 if(fp == NULL){
  printf("Erro ao abrir/criar o log \n\n\n");
  exit(0);
 }
 return fp;
}

void mensagem_log(char * url, int opcao){
	// op == ERRO_LOG -> rejeitado 
	// op == ACEPT_LOG (!= 0) -> aceito
	
	char * mensagem =(char *) malloc(10*sizeof(char)); 
	char * arquivo =(char *) malloc(16*sizeof(char));
	if (opcao == ERRO_LOG){
		arquivo= "ErrorLog.txt";
		mensagem = "rejeitado";
	}else{
		arquivo = "AcceptLog.txt";
		mensagem = "aceito";
	}
	FILE *fp = abrindo_log(arquivo);
	
	fprintf(fp,"Data : %s [%s] :: Request foi %s: %s \n",__DATE__,__TIME__ ,mensagem,url);
	
	mensagem = NULL;
	arquivo = NULL;
	free(mensagem);
	free(arquivo);
	fclose(fp);
}

void mensagem_log_body(char * url, char * dado){
	// op == ERRO_LOG -> rejeitado 
	// op == ACEPT_LOG (!= 0) -> aceito
	
	char * mensagem =(char *) malloc(10*sizeof(char)); 
	char * arquivo =(char *) malloc(16*sizeof(char));
	
	arquivo= "ErrorLog.txt";
	mensagem = "rejeitado por palavra proibida";
	
	FILE *fp = abrindo_log(arquivo);
	
	fprintf(fp,"Data : %s [%s] :: Request foi %s: %s -> %s\n",__DATE__,__TIME__ ,mensagem,url,dado);
	
	mensagem = NULL;
	arquivo = NULL;
	free(mensagem);
	free(arquivo);
	fclose(fp);
}