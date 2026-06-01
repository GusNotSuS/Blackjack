#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jogo.h"
#include "mao.h"

#ifdef _WIN32
    #include <windows.h>
    void aguardar_ms(int milissegundos) {
        Sleep(milissegundos);
    }
#else
    #include <unistd.h>
    void aguardar_ms(int milissegundos) {
        usleep(milissegundos * 1000);
    }
#endif

void inicializar_jogo(Jogo* jogo) {
    jogo->saldo = 250.0;
    jogo->topo_baralho = NULL;
    jogo->cartas_restantes = 0;
    jogo->consultas_restantes_totais = 3;
    jogo->rodadas_jogadas = 0;
    
    jogo->arquivo_historico = fopen("historico_blackjack.txt", "w");
    if (jogo->arquivo_historico != NULL) {
        fprintf(jogo->arquivo_historico, "=== LOG DE PARTIDA - BLACKJACK VISUAL PREMIUM ===\n\n");
    }
    
    criar_baralho(jogo);
}

void criar_baralho(Jogo* jogo) {
    destruir_baralho(&jogo->topo_baralho);
    
    char naipes[4][10] = {"Copas", "Espadas", "Ouros", "Paus"};
    char idents[13][3] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    int pesos[13] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    Carta sapato_temporario[52];
    int k = 0;
    for (int n = 0; n < 4; n++) {
        for (int i = 0; i < 13; i++) {
            strcpy(sapato_temporario[k].naipe, naipes[n]);
            strcpy(sapato_temporario[k].identidade, idents[i]);
            sapato_temporario[k].peso = pesos[i];
            k++;
        }
    }

    for (int i = 51; i > 0; i--) {
        int j = rand() % (i + 1);
        Carta temp = sapato_temporario[i];
        sapato_temporario[i] = sapato_temporario[j];
        sapato_temporario[j] = temp;
    }

    for (int i = 0; i < 52; i++) {
        empilhar(&jogo->topo_baralho, sapato_temporario[i]);
    }
    jogo->cartas_restantes = 52;

    atualizar_frequencia_inicial(jogo);
}

void atualizar_frequencia_inicial(Jogo* jogo) {
    for (int i = 0; i < 13; i++) {
        jogo->vetor_frequencia[i] = 4; 
    }
}

void registrar_saida_carta(Jogo* jogo, Carta carta) {
    char idents[13][3] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    for (int i = 0; i < 13; i++) {
        if (strcmp(carta.identidade, idents[i]) == 0) {
            if (jogo->vetor_frequencia[i] > 0) {
                jogo->vetor_frequencia[i]--;
            }
            break;
        }
    }
}

float calcular_probabilidade_estouro(Jogo* jogo, int pontuacao_atual) {
    if (pontuacao_atual <= 11) return 0.0;
    
    int margem = 21 - pontuacao_atual;
    int cartas_que_estouram = 0;
    int pesos[13] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    for (int i = 0; i < 13; i++) {
        int peso_efetivo = pesos[i];
        if (i == 0) peso_efetivo = 1;

        if (peso_efetivo > margem) {
            cartas_que_estouram += jogo->vetor_frequencia[i];
        }
    }
    return ((float)cartas_que_estouram / jogo->cartas_restantes) * 100.0;
}

float calcular_probabilidade_banca_vencer(Jogo* jogo, int pontuacao_banca_visivel, int pontuacao_jogador) {
    int cartas_vencedoras = 0;
    int pesos[13] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    for (int i = 0; i < 13; i++) {
        int peso_carta = pesos[i];
        int hipotetica_pontuacao = pontuacao_banca_visivel + peso_carta;
        
        if (hipotetica_pontuacao > 21 && (pontuacao_banca_visivel == 11 || i == 0)) {
            hipotetica_pontuacao -= 10;
        }

        if (hipotetica_pontuacao >= pontuacao_jogador && hipotetica_pontuacao <= 21) {
            cartas_vencedoras += jogo->vetor_frequencia[i];
        }
    }
    return ((float)cartas_vencedoras / jogo->cartas_restantes) * 100.0;
}

void exibir_painel_transparencia(Jogo* jogo, bool forcar_exibicao) {
    if (jogo->nivel_dificuldade == 4 && !forcar_exibicao) {
        return;
    }
    
    printf("\n--- PAINEL DE TRANSPARENCIA ESTATISTICA (CONTAGEM REAL) ---\n");
    printf("Cartas REVELADAS fora de jogo: %d\n", 52 - jogo->cartas_restantes);
    
    char idents[13][3] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    printf("Cartas estatisticamente disponiveis (sem contar as ocultas):\n|");
    for (int i = 0; i < 13; i++) {
        printf(" %s:%d |", idents[i], jogo->vetor_frequencia[i]);
    }
    printf("\n-----------------------------------------------------------\n");
}

void jogar_rodada(Jogo* jogo) {
    jogo->rodadas_jogadas++;
    
    if (jogo->cartas_restantes < 10) {
        printf("\n[AVISO]: O sapato possui poucas cartas conhecidas. Reembaralhando...\n");
        aguardar_ms(500);
        criar_baralho(jogo);
    }

    printf("\n==================================================\n");
    printf("RODADA NUMERO: %d | SALDO ATUAL: R$ %.2f\n", jogo->rodadas_jogadas, jogo->saldo);
    printf("==================================================\n");
    
    float aposta = 0.0;
    bool aposta_valida = false;
    
    while (!aposta_valida) {
        printf("Quanto deseja apostar nesta rodada? (Minimo: R$ 50.00 / Maximo: R$ %.2f): R$ ", jogo->saldo);
        scanf("%f", &aposta);
        
        if (aposta < 50.0) {
            printf("[Fallback]: A aposta minima permitida e de R$ 50.00.\n\n");
        } else if (aposta > jogo->saldo) {
            printf("[Fallback]: Saldo insuficiente.\n\n");
        } else {
            aposta_valida = true;
        }
    }

    jogo->saldo -= aposta;
    aguardar_ms(500);

    Mao jogador, casa;
    inicializar_mao(&jogador);
    inicializar_mao(&casa);

    Carta c1 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, c1);
    adicionar_carta_na_mao(&jogador, c1);

    Carta cc1 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, cc1);
    adicionar_carta_na_mao(&casa, cc1);

    Carta c2 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, c2);
    adicionar_carta_na_mao(&jogador, c2);

    Carta cc2 = desempilhar(&jogo->topo_baralho); 
    adicionar_carta_na_mao(&casa, cc2);

    int acao = 1;
    char resultado_rodada[50] = "Derrota";

    if (jogador.pontuacao == 21) {
        printf("\n==== FOR THE DEALER ====\n");
        exibir_mao_grafica(&casa, true);
        printf("\n==== FOR THE PLAYER ==== (Pontos: %d)\n", jogador.pontuacao);
        exibir_mao_grafica(&jogador, false);
        printf("\n[AVISO]: Voce atingiu a pontuacao maxima de 21 pontos com as iniciais!\n");
        aguardar_ms(1000);
    } else {
        while (jogador.pontuacao < 21) {
            printf("\n=================== MESA DE JOGO ===================\n");
            printf("==== FOR THE DEALER ====\n");
            exibir_mao_grafica(&casa, true);
            
            printf("\n==== FOR THE PLAYER ==== (Pontos: %d)\n", jogador.pontuacao);
            exibir_mao_grafica(&jogador, false);
            printf("====================================================\n");

            if (jogo->nivel_dificuldade == 1) { 
                exibir_painel_transparencia(jogo, false);
                printf("Sua probabilidade de ESTOURO se pedir carta (Hit): %.2f%%\n", calcular_probabilidade_estouro(jogo, jogador.pontuacao));
                printf("Chance de a Banca ganhar/empatar com voce na carta oculta: %.2f%%\n", calcular_probabilidade_banca_vencer(jogo, cc1.peso, jogador.pontuacao));
            } else if (jogo->nivel_dificuldade == 2) { 
                exibir_painel_transparencia(jogo, false);
            } else if (jogo->nivel_dificuldade == 3) { 
                printf("Consultas ao painel disponiveis: %d\n", jogo->consultas_restantes_totais);
                if (jogo->consultas_restantes_totais > 0) {
                    printf("Deseja gastar uma consulta estrategica? (1-Sim / 0-Nao): ");
                    int gastar;
                    scanf("%d", &gastar);
                    if (gastar == 1) {
                        exibir_painel_transparencia(jogo, true);
                        jogo->consultas_restantes_totais--;
                        printf("Analise: Chance de a Banca ganhar na carta oculta: %.2f%%\n", calcular_probabilidade_banca_vencer(jogo, cc1.peso, jogador.pontuacao));
                    }
                }
            }

            printf("\nAcao: (1) Hit (Pedir Carta) ou (2) Stand (Manter)? ");
            scanf("%d", &acao);

            if (acao == 1) {
                Carta nova = desempilhar(&jogo->topo_baralho);
                jogo->cartas_restantes--;
                registrar_saida_carta(jogo, nova);
                adicionar_carta_na_mao(&jogador, nova);
                printf("\nVoce comprou uma carta...\n");
                aguardar_ms(500);
            } else {
                break;
            }
        }
    }

    if (jogador.pontuacao > 21) {
        printf("\n==== FOR THE PLAYER ==== FINAL (Pontos: %d)\n", jogador.pontuacao);
        exibir_mao_grafica(&jogador, false);
        printf("\nSua pontuacao final: %d. Voce ESTOUROU (Bust)!\n", jogador.pontuacao);
        jogo->cartas_restantes--; 
        registrar_saida_carta(jogo, cc2); 
        strcpy(resultado_rodada, "Derrota (Estouro)");
        aguardar_ms(1000);
    } else {
        jogo->cartas_restantes--; 
        registrar_saida_carta(jogo, cc2); 
        printf("\n--- Turno da Banca ---\n");
        printf("==== FOR THE DEALER ==== REVELA A CARTA OCULTA:\n");
        exibir_mao_grafica(&casa, false);
        printf("Pontuacao imediata da Banca: %d\n", casa.pontuacao);
        aguardar_ms(1000);

        while (casa.pontuacao < 17) {
            printf("Banca esta com %d pontos e compra uma carta...\n", casa.pontuacao);
            aguardar_ms(800);
            Carta nova = desempilhar(&jogo->topo_baralho);
            jogo->cartas_restantes--;
            registrar_saida_carta(jogo, nova);
            adicionar_carta_na_mao(&casa, nova);
            exibir_mao_grafica(&casa, false);
        }
        
        printf("\n================ RESULTADO FINAL ================\n");
        printf("[SUA MAO] Pontos: %d | [BANCA] Pontos: %d\n", jogador.pontuacao, casa.pontuacao);

        if (casa.pontuacao > 21) {
            printf("A Banca estourou! Voce venceu a rodada.\n");
            jogo->saldo += (aposta * 2.0); 
            strcpy(resultado_rodada, "Vitoria (Banca Estourou)");
        } else if (jogador.pontuacao > casa.pontuacao) {
            printf("Voce fez mais pontos que a Banca! Vitoria.\n");
            jogo->saldo += (aposta * 2.0);
            strcpy(resultado_rodada, "Vitoria por Pontos");
        } else if (jogador.pontuacao < casa.pontuacao) {
            printf("A Banca fez mais pontos. Derrota.\n");
            strcpy(resultado_rodada, "Derrota por Pontos");
        } else {
            printf("Empate (Push)! O saldo foi devolvido.\n");
            jogo->saldo += aposta;
            strcpy(resultado_rodada, "Empate (Push)");
        }
        printf("=================================================\n");
        aguardar_ms(1500);
    }

    if (jogo->arquivo_historico != NULL) {
        fprintf(jogo->arquivo_historico, "Rodada %d:\n", jogo->rodadas_jogadas);
        fprintf(jogo->arquivo_historico, "  - Aposta: R$ %.2f\n", aposta);
        fprintf(jogo->arquivo_historico, "  - Pontos Jogador: %d\n", jogador.pontuacao);
        fprintf(jogo->arquivo_historico, "  - Pontos Banca: %d\n", casa.pontuacao);
        fprintf(jogo->arquivo_historico, "  - Resultado: %s\n", resultado_rodada);
        fprintf(jogo->arquivo_historico, "  - Saldo Apos Rodada: R$ %.2f\n\n", jogo->saldo);
    }

    liberar_mao(&jogador);
    liberar_mao(&casa);
}

void fechar_historico(Jogo* jogo, bool objetivo_atingido) {
    if (jogo->arquivo_historico != NULL) {
        fprintf(jogo->arquivo_historico, "=========================================\n");
        fprintf(jogo->arquivo_historico, "SUMMARY DO JOGO:\n");
        fprintf(jogo->arquivo_historico, "  - Total de Rodadas Concluidas: %d\n", jogo->rodadas_jogadas);
        fprintf(jogo->arquivo_historico, "  - Status Final: %s\n", objetivo_atingido ? "VITORIA TOTAL" : "DERROTA (FALENCIA)");
        fprintf(jogo->arquivo_historico, "  - Capital Final Encerramento: R$ %.2f\n", jogo->saldo);
        fprintf(jogo->arquivo_historico, "=========================================\n");
        fclose(jogo->arquivo_historico);
        printf("\n[Sistema]: Historico de jogo persistido com sucesso em 'historico_blackjack.txt'.\n");
    }
}