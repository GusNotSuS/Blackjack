#include <stdio.h>
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

void exibir_mao_grafica(Mao* mao, bool esconder_primeira) {
    if (mao->inicio == NULL) return;

    char linhas[9][1024];
    for (int i = 0; i < 9; i++) {
        linhas[i][0] = '\0';
    }

    No* current = mao->inicio;
    bool primeira = true;

    while (current != NULL) {
        char n_char = ' ';
        if (strcmp(current->carta.naipe, "Copas") == 0) n_char = 'v';      
        else if (strcmp(current->carta.naipe, "Espadas") == 0) n_char = '4'; 
        else if (strcmp(current->carta.naipe, "Ouros") == 0) n_char = 'o';   
        else if (strcmp(current->carta.naipe, "Paus") == 0) n_char = '+';    

        char temp[9][40];
        if (esconder_primeira && primeira) {
            sprintf(temp[0], " .__________. ");
            sprintf(temp[1], " |.:.:.:.:.:| ");
            sprintf(temp[2], " |:.:.:.:.:.| ");
            sprintf(temp[3], " |.:.:.:.:.:| ");
            sprintf(temp[4], " |:.:.:.:.:.| ");
            sprintf(temp[5], " |.:.:.:.:.:| ");
            sprintf(temp[6], " |:.:.:.:.:.| ");
            sprintf(temp[7], " |.:.:.:.:.:| ");
            sprintf(temp[8], " |__________| ");
        } else {
            sprintf(temp[0], " .__________. ");
            sprintf(temp[1], " |%-2s        | ", current->carta.identidade);
            sprintf(temp[2], " | .      . | ");
            sprintf(temp[3], " |   .  .   | ");
            sprintf(temp[4], " |    %c     | ", n_char);
            sprintf(temp[5], " |   .  .   | ");
            sprintf(temp[6], " | .      . | ");
            sprintf(temp[7], " |        %2s| ", current->carta.identidade);
            sprintf(temp[8], " |__________| ");
        }

        for (int i = 0; i < 9; i++) {
            strcat(linhas[i], temp[i]);
        }

        primeira = false;
        current = current->proximo;
    }

    for (int i = 0; i < 9; i++) {
        printf("%s\n", linhas[i]);
    }
}