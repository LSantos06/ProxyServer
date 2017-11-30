#ifndef inspecao_h 
#define inspecao_h

#include <stdio.h>
#include <stdlib.h>

/* Janela inpecao */
// exibe a janela de inspecao
void janela_inspecao();

/* Inspecao das requisicoes */
// intercepta a requisicao antes da mesma ser enviada pelo proxy
void intercepta_requisicao();
// exibicao da janela de requisicao
void janela_requisicao();
// edita um campo da requisicao
void edita_campo_requisicao();
// manda a requisicao para o proxy
void proxy_envia();

/* Inspecao das respostas */
// intercepta a resposta antes da mesma ser entregue ao browser
void intercepta_resposta();
// exibicao da janela de resposta
void janela_resposta();
// edita um campo da resposta
void edita_campo_resposta();
// manda a reposta para o browser
void entrega_browser();