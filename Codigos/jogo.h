#ifndef JOGO_H
#define JOGO_H

#include <stdio.h>
#include "baralho.h"

typedef struct {
    No* topo_baralho;
    int cartas_restantes;
    int vetor_frequencia[13];
    float saldo;
    int nivel_dificuldade;
    int consultas_restantes_totais;
    int rodadas_jogadas;
    FILE* arquivo_historico;
} Jogo;

void aguardar_ms(int milissegundos);
void inicializar_jogo(Jogo* jogo);
void criar_baralho(Jogo* jogo);
void atualizar_frequencia_inicial(Jogo* jogo);
void registrar_saida_carta(Jogo* jogo, Carta carta);
float calcular_probabilidade_estouro(Jogo* jogo, int pontuacao_atual);
float calcular_probabilidade_banca_vencer(Jogo* jogo, int pontuacao_banca_visivel, int pontuacao_jogador);
void exibir_painel_transparencia(Jogo* jogo, bool forcar_exibicao);
void jogar_rodada(Jogo* jogo);
void fechar_historico(Jogo* jogo, bool objetivo_atingido);

#endif