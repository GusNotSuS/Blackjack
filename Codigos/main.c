#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jogo.h"
#include "baralho.h"

int main() {
    srand(time(NULL));
    Jogo jogo;
    inicializar_jogo(&jogo);

    printf("=== BEM-VINDO AO SIMULADOR DE BLACKJACK ANALITICO ===\n");
    printf("Integrantes: Luiz, Marques, Lucas, Raphael e Tagore\n\n");
    printf("Capital Inicial fixado em R$ 250.00 | Objetivo: R$ 500.00\n\n");

    printf("Selecione o Nivel de Dificuldade:\n");
    printf("1 - Modo Assistencia (Suporte Total)\n");
    printf("2 - Modo Facil (Sem calculo automatizado)\n");
    printf("3 - Modo Medio (Apenas 3 consultas para o JOGO TODO)\n");
    printf("4 - Modo Dificil (Contagem puramente mental)\n");
    printf("Escolha: ");
    scanf("%d", &jogo.nivel_dificuldade);

    while (jogo.saldo >= 50.0 && jogo.saldo < 500.0) {
        jogar_rodada(&jogo);
    }

    if (jogo.saldo >= 500.0) {
        printf("\n==================================================\n");
        printf(" PARABENS! Voce atingiu o objetivo de R$ 500.00!\n");
        printf("==================================================\n");
    } else {
        printf("\n==================================================\n");
        printf(" GAME OVER! Saldo insuficiente para aposta minima.\n");
        printf("==================================================\n");
    }

    destruir_baralho(&jogo.topo_baralho);
    return 0;
}