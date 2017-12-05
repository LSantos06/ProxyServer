#include "inspecao.h"

/* Janela inpecao */
// exibe a janela de inspecao
void janela_inspecao(){
    return;
}

/* Inspecao das requisicoes */
// intercepta a requisicao antes da mesma ser enviada pelo proxy
void intercepta_requisicao(RequestORResponse *c_request){
    char edit_answer[10];
    char field_answer[50];
    HeaderList *auxHeaderList = NULL;

    printf("Request intercepted:\n");
    printf("\n\nmethodORversion: %s, urlORstatusCode: %s, versionORphrase: %s",c_request->methodORversion,c_request->urlORstatusCode,c_request->versionORphrase);
    printHeaderList(c_request->headers);
    printf("\nbody: %s\n\n",c_request->body);   

    printf("Do you want to edit the request? (y/n)\n");
    scanf("%s", edit_answer);
    if(edit_answer == "y"){
        printf("What field do you want to edit?\n");
        scanf("%s", field_answer);
 
        for(auxHeaderList=c_request->headers;auxHeaderList!=NULL;auxHeaderList=auxHeaderList->next){
            if(!strcmp(auxHeaderList->headerFieldName,field_answer)){
                edita_campo_requisicao(auxHeaderList);
                break;
            }
            else{
                print("Invalid field %s", field_answer);
                break;
            }
        }
    }
    printf("End of inspection\n");

    return;
}
// edita um campo da requisicao
void edita_campo_requisicao(){
    return;
}
// manda a requisicao para o proxy
void proxy_envia(){
    return;
}

/* Inspecao das respostas */
// intercepta a resposta antes da mesma ser entregue ao browser
void intercepta_resposta(RequestORResponse *s_response){
    char edit_answer[10];
    char field_answer[50];
    HeaderList *auxHeaderList = NULL;

    printf("Response intercepted:\n");
    printf("\n\nmethodORversion: %s, urlORstatusCode: %s, versionORphrase: %s",s_response->methodORversion,s_response->urlORstatusCode,s_response->versionORphrase);
    printHeaderList(s_response->headers);
    printf("\nbody: %s\n\n",s_response->body);   

    printf("Do you want to edit the response? (y/n)\n");
    scanf("%s", edit_answer);
    if(edit_answer == "y"){
        printf("What field do you want to edit?\n");
        scanf("%s", field_answer);
 
        for(auxHeaderList=s_response->headers;auxHeaderList!=NULL;auxHeaderList=auxHeaderList->next){
            if(!strcmp(auxHeaderList->headerFieldName,field_answer)){
                edita_campo_requisicao(auxHeaderList);
                break;
            }
            else{
                print("Invalid field %s", field_answer);
                break;
            }
        }
    }
    printf("End of inspection\n");

    return;
}
// edita um campo da resposta
void edita_campo_resposta(){
    return;
}
// exibicao da janela de resposta
void janela_resposta(){
    return;
}
// manda a reposta para o browser
void entrega_browser(){
    return;
}

// Obtains fields from request/response message
RequestORResponse* getRequestORResponseFields(char *buffer)
{
    RequestORResponse *requestORresponse = (RequestORResponse *)malloc(sizeof(RequestORResponse));
    int aux = 0, contentLength = 0;
    char auxBuffer[BUFFER], *pch = NULL, *pch2 = NULL;

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

    // If body not empty, obtaining it
    if(contentLength > 0)
    {
        requestORresponse->body = (char *)malloc(contentLength*sizeof(char)+1);
        for(aux=0;aux<contentLength;aux++)
            requestORresponse->body[aux] = *((char*)(pch+2+aux));
        requestORresponse->body[aux] = '\0';
    }
    else
        requestORresponse->body = NULL;

    return requestORresponse;
}

// Obtains request/response message from fields
char* getRequestORResponseMessage(RequestORResponse *requestORresponse)
{
    char *buffer = (char*)malloc(BUFFER*sizeof(char)+1);
    HeaderList *auxHeaderList = NULL;

    strcpy(buffer, requestORresponse->methodORversion);
    strcat(buffer, " ");
    strcat(buffer, requestORresponse->urlORstatusCode);
    strcat(buffer, " ");
    strcat(buffer, requestORresponse->versionORphrase);
    strcat(buffer, "\n");
    for(auxHeaderList=requestORresponse->headers;auxHeaderList!=NULL;auxHeaderList=auxHeaderList->next)
    {
        strcat(buffer, auxHeaderList->headerFieldName);
        strcat(buffer, ": ");
        strcat(buffer, auxHeaderList->value);
        strcat(buffer, "\r\n");
    }
    strcat(buffer, "\r\n");
    if(requestORresponse->body != NULL)
        strcat(buffer, requestORresponse->body);

    return buffer;
}

// Deallocates request/response fields
void freeRequestORResponseFiedls(RequestORResponse *requestORresponse)
{
    free(requestORresponse->methodORversion);
    free(requestORresponse->urlORstatusCode);
    free(requestORresponse->versionORphrase);
    freeHeaderList(requestORresponse->headers);
    free(requestORresponse->body);
    free(requestORresponse);

    return;
}

// HeaderList functions

// Creates new list
HeaderList* createHeaderList()
{
    return NULL;
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

