#include "filtragem.h"
/*
int main()
{
	int i=0;
	
	
	char v[100] = "youtube.com.br/watch?v=9EHeSjoDTU8";
	printf("\n------------BLACK LIST -----------\n");
	i = checkLists("blacklist.txt","www.netfix.com.br");
	printf("\nSaida BLACK LIST = %d",i);
	printf("\n------------BLACK LIST -----------\n");
	i = checkLists("blacklist.txt","www.youtube.com.br");
	printf("\nSaida BLACK LIST = %d",i);
	printf("\n------------BLACK LIST -----------\n");
	i =  checkLists("blacklist.txt",v);
	printf("\nSaida BLACK LIST = %d",i);
	printf("\n------------DENY TERMS LIST -----------\n");
	i = checkLists("denyterms.txt","gatinhos");
	printf("\nSaida BLACK LIST = %d",i);
	i = checkLists("denyterms.txt","fofinhos");
	printf("\nSaida BLACK LIST = %d",i);
	i = checkLists("denyterms.txt","fofidsadanhos");
	printf("\nSaida BLACK LIST = %d",i);
	i = checkLists("denyterms.txt","FORA temer");
	printf("\nSaida BLACK LIST = %d",i);
	i = checkLists("denyterms.txt","fora temer");
	printf("\nSaida BLACK LIST = %d",i);
	i = filtragem_mensagem("aprender.unb.br");
	printf("\nSaida FILTRO = %d",i);
	i = filtragem_mensagem(v);
	printf("\nSaida FILTRO = %d",i);
	Length_denyterms();
	
		printf("\n====-==== COMECANDO ");
	char v2[] = "www.pt.org.br/tag/fora-temer";
	i = denyterms_request(v2);
	printf("\n====-==== %d",i);
	char v1[2000] = "https://www.google.com.br/search?ei=XCYKWvC0K4iHwgSs95jQAQ&q=fofInhos&oq=foFinhos&gs_l=psy-ab.3..0l10.5084.6085.0.8457.3.3.0.0.0.0.212.453.0j2j1.3.0....0...1.1.64.psy-ab..0.3.451...0i10k1j0i5i10i30k1j0i67k1.0.G0-j9hW_znQ";
	i = denyterms_request(v1);
	printf("\n====-==== %d",i);
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
 	printf("\n%s lista[%d] = %s\n",procurado,i,lista[i]);
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
int filtragem_mensagem(char * site_destino)
{
	// pre processamento do host ---- adicionando www.
	char host_www[100];
	strcpy(host_www,site_destino);
	remove_http(host_www);
	// primeiro verifica-se se esta na whiteList
	int aux,aux2;
	aux = checkLists("whitelist.txt",host_www);
	aux2= checkLists("blacklist.txt",host_www);
	if(aux){
		// aux = 1, site pertence a lista branca
		//saida = 1 indica que pode se encaminhar a mensagem ao destinatario
		// chama funcao que encaminha mensagem ....
		return 1;
	} // verificando se esta na lista negra
	else if(aux2){
		// aux 2 indica que esta na lista proibida e saida = 0 indica que deve-se descartar a requesicao
		return 0;
	 }// se nao tiver em nenhuma da lista deve-se procurar por termos proibidos
	else{
	 	//buscar pelos deny terms
	 	// tem que analisar o corpo do html
		// mais pra frente ....  	
		return 2000;
	}
	
}
void remove_http(char* site_destino){
	// recebe url com http://
	int contador = 6,i=0;

	char www[100];
	if(strnicmp(site_destino,"http://",7)==0)
	{
		for(i=0,contador = 6; contador < strlen(site_destino); contador++,i++)
			www[i] = site_destino[contador];
	}else{
		printf("ERRO nao eh http \n");
	}

	strcpy(site_destino,www);
	
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
			printf("\nDENTR == %d \n",tamanho[j]);
	       if (checkLists("denyterms.txt",aux_s))
		 	{
		 	printf("\nDENTRO IF \t\t DEBUG == %s /// %d\n",aux_s,tamanho[j]);
		 	return 1;
			}
		
		}
		}
	
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
	//printf("\n OLA %d ----- %d\n",vetor[0],(strlen(lista[i])-1));
	fclose(fp);
	return vetor;
	
}

