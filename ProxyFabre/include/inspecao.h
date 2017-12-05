
/**
* @file inspecao.h
* @brief Arquivo com funcoes para o modulo de inspecao.
*/
#ifndef inspecao_h 
#define inspecao_h

#include <stdio.h>
#include <stdlib.h>

//////////////////////* Janela inpecao */
// exibe a janela de inspecao

/**
*   @fn void janela_inspecao();
*   @brief Exibe a janela de inspecao.
*/
void janela_inspecao();

/* Inspecao das requisicoes */
// intercepta a requisicao antes da mesma ser enviada pelo proxy
/**
*   @fn void intercepta_requisicao(RequestORResponse *c_request);
*   @brief Intercepta a requisicao antes da mesma ser enviada pelo proxy.
*	@param c_request Ponteiro para estrutura que contem os campos das requisicoes HTTP.
*/
void intercepta_requisicao(RequestORResponse *c_request);
// edita um campo da requisicao
/**
*   @fn void edita_campo_requisicao();
*   @brief Edita um campo de requisicao.
*/
void edita_campo_requisicao();
// exibicao da janela de requisicao
/**
*   @fn void janela_requisicao();
*   @brief Exibicao da janela de requisicao.
*/
void janela_requisicao();
// manda a requisicao para o proxy
/**
*   @fn void proxy_envia();
*   @brief Manda a requisicao para o proxy.
*/
void proxy_envia();

////////////* Inspecao das respostas *//////////////
// intercepta a resposta antes da mesma ser entregue ao browser
/**
*   @fn void intercepta_resposta(RequestORResponse *c_request);
*   @brief Intercepta a resposta antes da mesma ser ao browser.
*	@param c_request Ponteiro para estrutura que contem os campos das requisicoes HTTP.
*/
void intercepta_resposta(RequestORResponse *s_response);
// edita um campo da resposta
/**
*   @fn void edita_campo_resposta();
*   @brief Edita um campo de resposta.
*/
void edita_campo_resposta();
// exibicao da janela de resposta
/**
*   @fn void janela_resposta();
*   @brief Exibicao da janela de resposta.
*/
void janela_resposta();
// manda a reposta para o browser
/**
*   @fn void entrega_browser();
*   @brief Manda a resposta para o browser.
*/
void entrega_browser();

#endif