#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "filtragem.h"

/*
int main()
{
	int i=0;
	
	
	char v[100] = "www.youtube.com.br/watch?v=9EHeSjoDTU8";
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
	i = filtragem("www.aprender.unb.br");
	printf("\nSaida FILTRO = %d",i);
	i = filtragem(v);
	printf("\nSaida FILTRO = %d",i);
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
	int i=0,saida=0;
	
	char procurado[strlen(mensagem)];
 	// zerando a string
	memset (procurado,'\0',strlen(mensagem));
	strcpy (procurado,mensagem);
	FILE * fp = abrindo_arquivo(nome_arquivo);

	while (i<SIZE_LISTA && fgets(lista[i],MAX_STR,fp)!=NULL)
	{
		printf("\n%s lista[%d] = %s\n",procurado,i,lista[i]);
		
		
		if (strnicmp(procurado,lista[i],strlen(lista[i])-1)==0)
		{
			saida = 1;
			break;
		}
		i++;
	} 
	fclose(fp);
	return saida;
	}
//por enquanto retorna um int para indicar se ta ou nao ta proibido
int filtragem(char * site_destino)
{
	// primeiro verifica-se se esta na whiteList
	int aux,aux2;
	aux = checkLists("whitelist.txt",site_destino);
	aux2= checkLists("blacklist.txt",site_destino);
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

