#ifndef MAO_H
#define MAO_H

#include "baralho.h"

typedef struct {
    No* inicio;
    int total_cartas;
    int pontuacao;
    bool possui_as_flex;
} Mao;

void inicializar_mao(Mao* mao);
void adicionar_carta_na_mao(Mao* mao, Carta carta);
void liberar_mao(Mao* mao);

#endif