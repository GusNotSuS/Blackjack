#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jogo.h"
#include "baralho.h"
#include "config.h"

void executar_simulacao_e_gerar_relatorio(void);

int main() {
    srand(time(NULL));
    int deseja_continuar = 1;

    printf("=== BEM-VINDO AO SIMULADOR DE BLACKJACK ANALITICO VISUAL ===\n");
    printf("Integrantes: Luiz, Marques, Lucas, Raphael e Tagore\n\n");

    while (deseja_continuar == 1) {
        Jogo jogo;
        inicializar_jogo(&jogo);

        printf("Capital Inicial fixado em R$ %.2f | Objetivo: R$ %.2f\n\n", SALDO_INICIAL, OBJETIVO_VITORIA);
        printf("Selecione o Nivel de Dificuldade:\n");
        printf("1 - Modo Assistencia (Suporte Total)\n");
        printf("2 - Modo Facil (Sem calculo automatizado)\n");
        printf("3 - Modo Medio (Apenas 3 consultas para o JOGO TODO)\n");
        printf("4 - Modo Dificil (Contagem puramente mental)\n");
        printf("5 - Modo Simulacao (100 partidas para cada algoritmo)\n");
        printf("Escolha: ");
        char input[10];
        fgets(input, sizeof(input), stdin);
        char *endptr;
        long valor = strtol(input, &endptr, 10);
        if (endptr == input || *endptr != '\n' && *endptr != '\0') {
            printf("Entrada invalida. Usando valor padrao 1.\n");
            jogo.nivel_dificuldade = 1;
        } else {
            jogo.nivel_dificuldade = (int)valor;
        }

        if (jogo.nivel_dificuldade == 5) {
            executar_simulacao_e_gerar_relatorio();
            deseja_continuar = 0;
        } else {
            while (jogo.saldo >= APOSTA_MINIMA && jogo.saldo < OBJETIVO_VITORIA) {
                jogar_rodada(&jogo);
            }

            bool vitoria = (jogo.saldo >= OBJETIVO_VITORIA);
            if (vitoria) {
                printf("\n==================================================\n");
                printf(" PARABENS! Voce atingiu o objetivo de R$ %.2f!\n", OBJETIVO_VITORIA);
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
        char input2[10];
        fgets(input2, sizeof(input2), stdin);
        char *endptr2;
        long valor2 = strtol(input2, &endptr2, 10);
        if (endptr2 == input2 || (*endptr2 != '\n' && *endptr2 != '\0')) {
            printf("Entrada invalida. Saindo do jogo.\n");
            deseja_continuar = 0;
        } else {
            deseja_continuar = (int)valor2;
        }
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

    fprintf(relatorio, "=== RELATÓRIO DETALHADO DE SIMULAÇÃO DE BLACKJACK ===\n\n");
    fprintf(relatorio, "Configuração:\n");
    fprintf(relatorio, " - Capital inicial: R$ %.2f\n", SALDO_INICIAL);
    fprintf(relatorio, " - Aposta fixa por rodada: R$ %.2f\n", APOSTA_MINIMA);
    fprintf(relatorio, " - Número de simulações por algoritmo: %d\n\n", NUM_SIMULACOES);

    for (int alg_idx = 0; alg_idx < NUM_ALGORITMOS; alg_idx++) {
        int algoritmo = ALGORITMOS[alg_idx];
        const char* nome_algoritmo = NOMES_ALGORITMOS[alg_idx];

        fprintf(relatorio, "Algoritmo %d: %s\n", algoritmo, nome_algoritmo);
        fprintf(relatorio, "----------------------------------------\n");
        fprintf(relatorio, "Jogo | Resultado | Rodadas\n");
        fprintf(relatorio, "------------------------\n");

        int vitorias = 0;
        int derrotas = 0;
        int total_rodadas = 0;

        for (int sim = 0; sim < NUM_SIMULACOES; sim++) {
            Jogo jogo;
            inicializar_jogo(&jogo);
            jogo.nivel_dificuldade = algoritmo;
            jogo.arquivo_historico = NULL;

            while (jogo.saldo >= APOSTA_MINIMA && jogo.saldo < OBJETIVO_VITORIA) {
                jogar_rodada(&jogo);
            }

            bool vitoria = (jogo.saldo >= OBJETIVO_VITORIA);
            if (vitoria) {
                vitorias++;
            } else {
                derrotas++;
            }

            total_rodadas += jogo.rodadas_jogadas;

            fprintf(relatorio, "%3d    | %7s   | %2d\n",
                    sim + 1,
                    vitoria ? "V" : "D",
                    jogo.rodadas_jogadas);

            if (jogo.arquivo_historico != NULL) {
                fechar_historico(&jogo, vitoria);
            }
            destruir_baralho(&jogo.topo_baralho);
        }

        float media_rodadas = (float)total_rodadas / (float)NUM_SIMULACOES;
        float taxa_vitoria = ((float)vitorias / (float)NUM_SIMULACOES) * 100.0f;

        fprintf(relatorio, "\n");
        fprintf(relatorio, "Estatísticas resumidas:\n");
        fprintf(relatorio, " - Vitórias: %d (%.2f%%)\n", vitorias, taxa_vitoria);
        fprintf(relatorio, " - Derrotas: %d (%.2f%%)\n", derrotas, 100.0f - taxa_vitoria);
        fprintf(relatorio, " - Média de rodadas por partida: %.2f\n", media_rodadas);
        fprintf(relatorio, "\n");
    }

    fprintf(relatorio, "=== FIM DO RELATÓRIO ===\n");
    fclose(relatorio);

    printf("\n[Sistema]: Simulação concluída. Relatório detalhado gerado em 'relatorio_algoritmos.txt'.\n");
}