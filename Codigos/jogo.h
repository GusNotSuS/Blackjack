#ifndef JOGO_H
#define JOGO_H

#include "baralho.h"

typedef struct {
    No* topo_baralho;
    int cartas_restantes;
    int vetor_frequencia[13];
    float saldo;
    int nivel_dificuldade;
    int consultas_restantes_rodada;
} Jogo;

void inicializar_jogo(Jogo* jogo);
void criar_baralho(Jogo* jogo);
void atualizar_frequencia_inicial(Jogo* jogo);
void registrar_saida_carta(Jogo* jogo, Carta carta);
float calcular_probabilidade_estouro(Jogo* jogo, int pontuacao_atual);
void exibir_painel_transparencia(Jogo* jogo, bool forçar_exibicao);
void jogar_rodada(Jogo* jogo);

#endif