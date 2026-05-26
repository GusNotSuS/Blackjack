#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jogo.h"
#include "baralho.h"

int main() {
    srand(time(NULL));
    Jogo jogo;
    inicializar_jogo(&jogo);

    printf("=== BEM-VINDO AO SIMULADOR DE BLACKJACK ANALÍTICO ===\n");
    printf("Integrantes: Luiz, Marques, Lucas, Raphael e Tágore\n\n");
    
    printf("Selecione o Nível de Dificuldade:\n");
    printf("1 - Modo Assistência (Suporte Total)\n");
    printf("2 - Modo Fácil (Sem cálculo automatizado)\n");
    printf("3 - Modo Médio (Apenas 1 consulta por rodada)\n");
    printf("4 - Modo Difícil (Contagem puramente mental)\n");
    printf("Escolha: ");
    scanf("%d", &jogo.nivel_dificuldade);

    while (jogo.saldo >= 50.0 && jogo.saldo < 1000.0) {
        jogar_rodada(&jogo);
    }

    if (jogo.saldo >= 1000.0) {
        printf("\n==================================================\n");
        printf(" PARABÉNS! Você atingiu o objetivo de R$ 1000.00!\n");
        printf(" Vitória alcançada mitigando a vantagem da casa.\n");
        printf("==================================================\n");
    } else {
        printf("\n==================================================\n");
        printf(" GAME OVER! Saldo insuficiente para aposta mínima.\n");
        printf(" Falência decretada. A casa venceu no longo prazo.\n");
        printf("==================================================\n");
    }

    destruir_baralho(&jogo.topo_baralho);
    return 0;
}