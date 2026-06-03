#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jogo.h"
#include "baralho.h"

int main() {
    srand(time(NULL));
    int deseja_continuar = 1;

    printf("=== BEM-VINDO AO SIMULADOR DE BLACKJACK ANALITICO VISUAL ===\n");
    printf("Integrantes: Luiz, Marques, Lucas, Raphael e Tagore\n\n");

    while (deseja_continuar == 1) {
        Jogo jogo;
        inicializar_jogo(&jogo);

        printf("Capital Inicial fixado em R$ 250.00 | Objetivo: R$ 500.00\n\n");
        printf("Selecione o Nivel de Dificuldade:\n");
        printf("1 - Modo Assistencia (Suporte Total)\n");
        printf("2 - Modo Facil (Sem calculo automatizado)\n");
        printf("3 - Modo Medio (Apenas 3 consultas para o JOGO TODO)\n");
        printf("4 - Modo Dificil (Contagem puramente mental)\n");
        printf("5 - Modo Simulacao (100 partidas para cada algoritmo)\n");
        printf("Escolha: ");
        scanf("%d", &jogo.nivel_dificuldade);

        if (jogo.nivel_dificuldade == 5) {
            executar_simulacao_e_gerar_relatorio();
        } else {
            while (jogo.saldo >= 50.0 && jogo.saldo < 500.0) {
                jogar_rodada(&jogo);
            }

            bool vitoria = (jogo.saldo >= 500.0);
            if (vitoria) {
                printf("\n==================================================\n");
                printf(" PARABENS! Voce atingiu o objetivo de R$ 500.00!\n");
                printf("==================================================\n");
            } else {
                printf("\n==================================================\n");
                printf(" GAME OVER! Saldo insuficiente para aposta minima.\n");
                printf("==================================================\n");
            }

            fechar_historico(&jogo, vitoria);
            destruir_baralho(&jogo.topo_baralho);
        }

        printf("\nDeseja iniciar um NOVO JOGO do zero? (1 - Sim / 0 - Sair): ");
        scanf("%d", &deseja_continuar);
        printf("\n\n");
    }

    printf("Programa encerrado. Verifique o log de historico gerado na pasta.\n");
    return 0;
}

void executar_simulacao_e_gerar_relatorio() {
    const int NUM_SIMULACOES = 100;
    const int ALGORITMOS[] = {5, 6, 7};
    const char* NOMES_ALGORITMOS[] = {"Tradicional", "Conservador", "Probabilistico"};
    const int NUM_ALGORITMOS = sizeof(ALGORITMOS) / sizeof(ALGORITMOS[0]);

    FILE* relatorio = fopen("relatorio_algoritmos.txt", "w");
    if (relatorio == NULL) {
        printf("[ERRO]: Não foi possível criar o arquivo de relatório.\n");
        return;
    }

    fprintf(relatorio, "=== RELATÓRIO DE SIMULAÇÃO DE BLACKJACK ===\n\n");
    fprintf(relatorio, "Configuração:\n");
    fprintf(relatorio, " - Capital inicial: R$ 250.00\n");
    fprintf(relatorio, " - Aposta fixa por rodada: R$ 50.00\n");
    fprintf(relatorio, " - Número de simulações por algoritmo: %d\n\n", NUM_SIMULACOES);

    for (int alg_idx = 0; alg_idx < NUM_ALGORITMOS; alg_idx++) {
        int algoritmo = ALGORITMOS[alg_idx];
        const char* nome_algoritmo = NOMES_ALGORITMOS[alg_idx];

        fprintf(relatorio, "Algoritmo %d: %s\n", algoritmo, nome_algoritmo);
        fprintf(relatorio, "----------------------------------------\n");

        int vitorias = 0;
        int derrotas = 0;
        int total_rodadas = 0;

        for (int sim = 0; sim < NUM_SIMULACOES; sim++) {
            Jogo jogo;
            inicializar_jogo(&jogo);
            jogo.nivel_dificuldade = algoritmo;
            jogo.arquivo_historico = NULL;

            while (jogo.saldo >= 50.0 && jogo.saldo < 500.0) {
                jogar_rodada(&jogo);
            }

            bool vitoria = (jogo.saldo >= 500.0);
            if (vitoria) {
                vitorias++;
            } else {
                derrotas++;
            }

            total_rodadas += jogo.rodadas_jogadas;

            if (jogo.arquivo_historico != NULL) {
                fechar_historico(&jogo, vitoria);
            }
            destruir_baralho(&jogo.topo_baralho);
        }

        float media_rodadas = (float)total_rodadas / NUM_SIMULACOES;
        float taxa_vitoria = ((float)vitorias / NUM_SIMULACOES) * 100.0f;

        fprintf(relatorio, "Resultados:\n");
        fprintf(relatorio, " - Vitórias: %d (%.2f%%)\n", vitorias, taxa_vitoria);
        fprintf(relatorio, " - Derrotas: %d (%.2f%%)\n", derrotas, 100.0f - taxa_vitoria);
        fprintf(relatorio, " - Média de rodadas por partida: %.2f\n", media_rodadas);
        fprintf(relatorio, "\n");
    }

    fprintf(relatorio, "=== FIM DO RELATÓRIO ===\n");
    fclose(relatorio);

    printf("\n[Sistema]: Simulação concluída. Relatório gerado em 'relatorio_algoritmos.txt'.\n");
}