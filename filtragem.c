#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "filtragem.h"


FILE* abrindo_arquivo(char* nome_arquivo){
  FILE *fp;
  fp = fopen(nome_arquivo, "r");
 if(fp == NULL){
  printf("Erro ao abrir o arquivo \n\n\n");
  exit(0);
 }
 return fp;
}

int vetor_do_txt(char* nome_arquivo,char * url){
	
	char info[SIZE_LISTA][MAX_STR];
	FILE * fp = abrindo_arquivo(nome_arquivo);
	int i =0, saida =0;
	while( (fgets(info[i], sizeof(info[i]), fp))!=NULL )
		{
			printf("%s \t",info[i]);
			
			if(!strcmp(strupr(info[i]),strupr(url))){
				saida = 1;
				break;
			}
			i++;
		}	
		
	fclose(fp);
	
	return saida;
}

