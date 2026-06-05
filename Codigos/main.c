#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jogo.h"
#include "baralho.h"

int main() {
    srand(time(NULL));
    int deseja_continuar = 1;

    print_lento("=== BEM-VINDO AO SIMULADOR DE BLACKJACK ANALITICO VISUAL ===\n", 5);
    print_lento("Integrantes: Luiz, Marques, Lucas, Raphael e Tagore\n\n",5);

    while (deseja_continuar == 1) {
        Jogo jogo;
        inicializar_jogo(&jogo);

        print_lento("Capital Inicial fixado em R$ 250.00 | Objetivo: R$ 500.00\n\n",1);
        print_lento("Selecione o Nivel de Dificuldade:\n",1);
        printf("1 - Modo Assistencia (Suporte Total)\n");
        printf("2 - Modo Facil (Sem calculo automatizado)\n");
        printf("3 - Modo Medio (Apenas 3 consultas para o JOGO TODO)\n");
        printf("4 - Modo Dificil (Contagem puramente mental)\n");
        print_lento("Escolha: ",1);
        scanf("%d", &jogo.nivel_dificuldade);

        while (jogo.saldo >= 50.0 && jogo.saldo < 500.0) {
            jogar_rodada(&jogo);
        }

        bool vitoria = (jogo.saldo >= 500.0);
        if (vitoria) {
            printf("\n==================================================\n");
            print_lento(" PARABENS! Voce atingiu o objetivo de R$ 500.00!\n",5);
            printf("==================================================\n");
        } else {
            printf("\n==================================================\n");
            print_lento(" GAME OVER! Saldo insuficiente para aposta minima.\n",5);
            printf("==================================================\n");
        }

        fechar_historico(&jogo, vitoria);
        destruir_baralho(&jogo.topo_baralho);

        print_lento("\nDeseja iniciar um NOVO JOGO do zero? (1 - Sim / 0 - Sair): ",5);
        scanf("%d", &deseja_continuar);
        printf("\n\n");
    }

    printf("Programa encerrado. Verifique o log de historico gerado na pasta.\n");
    return 0;
}