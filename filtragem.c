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
	int i=0, saida = 0;
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
			saida = 1;
			
		}
		i++;
	} 
	fclose(fp);
	return saida;
	}
//por enquanto retorna um int para indicar se ta ou nao ta proibido
int filtragem_url(char * url)
{
	/* NAO PRECISA MAIS ----------- CONSIDERANDO URL : www.<site>
	// pre processamento do host ---- adicionando www.
	char host_www[100];
	if(strnicmp("www.",url,4) != 0)
	{
	strcpy(host_www,url);
	}
	*/
	// primeiro verifica-se se esta na whiteList
	int aux_white,aux_black,aux_deny;
	aux_white = checkLists("whitelist.txt",url);
	aux_black= checkLists("blacklist.txt",url);
	// verificando se esta na url os termos proibidos
	aux_deny = denyterms_request(url);
	if(aux_white){
		printf("\n\tWHITE LIST OK --- ENCAMINHAR MENSAGEM %s \n",url);
		return 1;
	} // verificando se esta na lista negra
	else if(aux_black){
	   
		printf("\n\tESTA NA BLACK LIST  --- REJEITAR MENSAGEM %s \n",url);
		return 0;
	 }// se nao tiver em nenhuma da lista deve-se procurar por termos proibidos
	else if (aux_deny){
	 	// se for 1 entao tem termos proibidos na url 
		printf("\n\tdeny terms na URL  --- REJEITAR  MENSAGEM %s \n",url);
		return 0;	
	}else{
			printf("\n\tNao eh proibido nem esta na white - ENCAMINHAR MENSAGEM %s \n",url);
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

int denyterms_body(char * body){
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
	// op == 0 -> rejeitado 
	// op == 1 (!= 0) -> aceito
	char * mensagem =(char *) malloc(10*sizeof(char)); 
	char * arquivo =(char *) malloc(16*sizeof(char));
	if (!opcao){
		arquivo= "ErrorLog.txt";
		mensagem = "rejeitado";
	}else{
		arquivo = "AcceptLog.txt";
		mensagem = "aceito";
	}
	char data[9];
	char horas[9];
	_strdate(data);
	_strtime(horas);
	FILE *fp = abrindo_log(arquivo);
	fprintf(fp,"Data : %s  [%s] :: Request foi %s: %s \n",data,horas,mensagem,url);
	
	
	mensagem = NULL;
	arquivo = NULL;
	free(mensagem);
	free(arquivo);
	fclose(fp);
}