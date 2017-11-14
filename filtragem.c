#include "filtragem.h"
/*
int main()
{
	int i=0;
	
	
	char v[100] = "http://www.youtube.com.br/watch?v=9EHeSjoDTU8";
	printf("\n====-==== COMECANDO ");
	char v2[] = "http://www.pt.org.br/tag/fora-temer";
	i=filtragem_url(v2);
	printf("\n saida = %d ",i);
	char v1[] = "http://www.google.com.br/search?ei=XCYKWvC0K4iHwgSs95jQAQ&q=fofInhos&oq=foFinhos&gs_";
	i=filtragem_url(v1);
	printf("\n saida = %d ",i);
	i=filtragem_url(v);
	printf("\n saida = %d ",i);	
	char w[]= "http://www.unb.br";
	i=filtragem_url(w);
	printf("\n saida = %d ",i);	
	char g[] = "http://www.globo.br";
	i=filtragem_url(g);
	printf("\n saida = %d ",i);	
	printf("\n FIM");
	return i;
}
*/


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
	int i=0;
	
	char procurado[strlen(mensagem)];
 	// zerando a string
 	memset (procurado,'\0',strlen(mensagem));
	strcpy (procurado,mensagem);
	FILE * fp = abrindo_arquivo(nome_arquivo);

	while (i<SIZE_LISTA && fgets(lista[i],MAX_STR,fp)!=NULL)
	{
		//printf("\n%s lista[%d] = %s\n",procurado,i,lista[i]);
		
		
		if (strnicmp(procurado,lista[i],(strlen(lista[i])-1))==0)
		{
			return 1;
			
		}
		i++;
	} 
	fclose(fp);
	return 0;
	}
//por enquanto retorna um int para indicar se ta ou nao ta proibido
int filtragem_url(char * url)
{
	// pre processamento do host ---- adicionando www.
	char host_www[100];
	strcpy(host_www,url);
	remove_http(host_www);
	// primeiro verifica-se se esta na whiteList
			
	int aux_white,aux_black,aux_deny;
	aux_white = checkLists("whitelist.txt",host_www);
	aux_black= checkLists("blacklist.txt",host_www);
	aux_deny = denyterms_request(host_www);
	if(aux_white){
		/* aux = 1, site pertence a lista branca
		*saida = 1 indica que pode se encaminhar a mensagem ao destinatario
		* chama funcao que encaminha mensagem ....
		* neste caso nao se retornaria nada 
		* deveria chamar a funcao para que enviar e escrever log
		*/
		printf("\n\tWHITE LIST OK --- ENCAMINHAR MENSAGEM %s \n",host_www);
		
		return 1;
	} // verificando se esta na lista negra
	else if(aux_black){
	   /* aux 2 indica que esta na lista proibida e saida = 0 indica que deve-se descartar a requesicao
		* deveria chamar a funcao para que seja reijado e escrever log
		*/
		printf("\n\tESTA NA BLACK LIST  --- REJEITAR MENSAGEM %s \n",host_www);
		return 0;
	 }// se nao tiver em nenhuma da lista deve-se procurar por termos proibidos
	else if (aux_deny){
	 	// se for 1 entao tem termos proibidos na url 
		printf("\n\tdeny terms na URL  --- REJEITAR  MENSAGEM %s \n",host_www);
		return 0;	
	}else{
			printf("\n\tNao eh proibido nem esta na white - ENCAMINHAR MENSAGEM %s \n",host_www);
			return 1;
	}
	
	
}
void remove_http(char* url_http){
	// recebe url com http://
	int contador = 6,i=0;

	char url_www[100];
	if(strnicmp(url_http,"http://",7)==0)
	{	
		
		for(i=0,contador = 7; contador < strlen(url_http); contador++,i++)
			{
			url_www[i] = url_http[contador];
			}
			strcpy(url_http,url_www);
	}else if(strnicmp(url_http,"https://",8)==0){
		printf("\n ERRO nao eh  aceito https \n");
		exit(0);
	}else{
		printf("Nao reconhecido o cabecalho \n");
		exit(0);
	}

	
}

int denyterms_request(char * request){
	char * aux_s = malloc(MAX_STR*sizeof(char));
	int * tamanho = malloc(SIZE_LISTA*sizeof(int));
	tamanho = Length_denyterms();
	int i_aux=0,k=0,i=0,j=0,qnt = tamanho[0];
	
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
		vetor[j] = strlen(lista[i])-1;
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
