#include <stdlib.h>
#include <string.h>
#include "mao.h"

void inicializar_mao(Mao* mao) {
    mao->inicio = NULL;
    mao->total_cartas = 0;
    mao->pontuacao = 0;
    mao->possui_as_flex = false;
}

void adicionar_carta_na_mao(Mao* mao, Carta carta) {
    No* novo = (No*)malloc(sizeof(No));
    novo->carta = carta;
    novo->proximo = NULL;

    if (mao->inicio == NULL) {
        mao->inicio = novo;
    } else {
        No* atual = mao->inicio;
        while (atual->proximo != NULL) {
            atual = atual->proximo;
        }
        atual->proximo = novo;
    }
    
    mao->total_cartas++;
    mao->pontuacao += carta.peso;

    if (strcmp(carta.identidade, "A") == 0) {
        mao->possui_as_flex = true;
    }

    if (mao->pontuacao > 21 && mao->possui_as_flex) {
        No* atual = mao->inicio;
        while (atual != NULL) {
            if (strcmp(atual->carta.identidade, "A") == 0 && atual->carta.peso == 11) {
                atual->carta.peso = 1;
                mao->pontuacao -= 10;
                if (mao->pontuacao <= 21) break;
            }
            atual = atual->proximo;
        }
        
        bool ainda_tem = false;
        atual = mao->inicio;
        while (atual != NULL) {
            if (strcmp(atual->carta.identidade, "A") == 0 && atual->carta.peso == 11) {
                ainda_tem = true;
            }
            atual = atual->proximo;
        }
        mao->possui_as_flex = ainda_tem;
    }
}

void liberar_mao(Mao* mao) {
    No* atual = mao->inicio;
    while (atual != NULL) {
        No* prox = atual->proximo;
        free(atual);
        atual = prox;
    }
    mao->inicio = NULL;
}