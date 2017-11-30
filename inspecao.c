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
 
        for(auxHeaderList=c_request->headers;auxHeaderList!=NULL;auxHeaderList=auxHeaderList->next)
        {
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
// exibicao da janela de requisicao
void janela_requisicao(){
    return;
}
// manda a requisicao para o proxy
void proxy_envia(){
    return;
}

/* Inspecao das respostas */
// intercepta a resposta antes da mesma ser entregue ao browser
void intercepta_resposta(RequestORResponse *s_response){
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