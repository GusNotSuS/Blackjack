#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jogo.h"
#include "mao.h"

void inicializar_jogo(Jogo* jogo) {
    jogo->saldo = 200.0;
    jogo->topo_baralho = NULL;
    jogo->cartas_restantes = 0;
    criar_baralho(jogo);
}

void criar_baralho(Jogo* jogo) {
    destruir_baralho(&jogo->topo_baralho);
    
    char naipes[4][10] = {"Copas", "Espadas", "Ouros", "Paus"};
    char idents[13][3] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    int pesos[13] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    Carta sapato_temporario[104];
    int k = 0;
    for (int b = 0; b < 2; b++) {
        for (int n = 0; n < 4; n++) {
            for (int i = 0; i < 13; i++) {
                strcpy(sapato_temporario[k].naipe, naipes[n]);
                strcpy(sapato_temporario[k].identidade, idents[i]);
                sapato_temporario[k].peso = pesos[i];
                k++;
            }
        }
    }

    for (int i = 103; i > 0; i--) {
        int j = rand() % (i + 1);
        Carta temp = sapato_temporario[i];
        sapato_temporario[i] = sapato_temporario[j];
        sapato_temporario[j] = temp;
    }

    for (int i = 0; i < 104; i++) {
        empilhar(&jogo->topo_baralho, sapato_temporario[i]);
    }
    jogo->cartas_restantes = 104;

    atualizar_frequencia_inicial(jogo);
}

void atualizar_frequencia_inicial(Jogo* jogo) {
    for (int i = 0; i < 13; i++) {
        jogo->vetor_frequencia[i] = 8; 
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

void exibir_painel_transparencia(Jogo* jogo, bool forçar_exibicao) {
    if (jogo->nivel_dificuldade == 4 && !forçar_exibicao) {
        printf("\n[Painel Oculto - Modo Difícil ativo]\n");
        return;
    }
    
    printf("\n--- PAINEL DE TRANSPARÊNCIA ESTATÍSTICA ---\n");
    printf("Cartas restantes no sapato: %d\n", jogo->cartas_restantes);
    
    char idents[13][3] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    printf("Distribuição atual no Baralho:\n|");
    for (int i = 0; i < 13; i++) {
        printf(" %s:%d |", idents[i], jogo->vetor_frequencia[i]);
    }
    printf("\n-------------------------------------------\n");
}

void jogar_rodada(Jogo* jogo) {
    if (jogo->cartas_restantes < 20) {
        printf("\n[AVISO]: O sapato possui menos de 20 cartas. Reembaralhando...\n");
        criar_baralho(jogo);
    }

    printf("\n==================================================\n");
    printf("SALDO ATUAL: R$ %.2f | Aposta da Rodada: R$ 50.00\n", jogo->saldo);
    printf("==================================================\n");
    
    jogo->saldo -= 50.0;
    jogo->consultas_restantes_rodada = 1;

    Mao jogador, casa;
    inicializar_mao(&jogador);
    inicializar_mao(&casa);

    Carta c1 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, c1);
    Carta c2 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, c2);
    adicionar_carta_na_mao(&jogador, c1);
    adicionar_carta_na_mao(&jogador, c2);

    Carta cc1 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, cc1);
    Carta cc2 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; 
    adicionar_carta_na_mao(&casa, cc1);
    adicionar_carta_na_mao(&casa, cc2);

    int acao = 1;
    while (jogador.pontuacao < 21) {
        printf("\nSua Mão: ");
        No* aux = jogador.inicio;
        while(aux != NULL) {
            printf("[%s de %s] ", aux->carta.identidade, aux->carta.naipe);
            aux = aux->proximo;
        }
        printf(" -> Pontos: %d\n", jogador.pontuacao);
        printf("Carta aberta da Banca: [%s de %s]\n", cc1.identidade, cc1.naipe);

        if (jogo->nivel_dificuldade == 1) { 
            exibir_painel_transparencia(jogo, false);
            printf("Probabilidade de ESTOURO no próximo Hit: %.2f%%\n", calcular_probabilidade_estouro(jogo, jogador.pontuacao));
        } else if (jogo->nivel_dificuldade == 2) { 
            exibir_painel_transparencia(jogo, false);
        } else if (jogo->nivel_dificuldade == 3) { 
            printf("Consultas ao painel disponíveis nesta rodada: %d\n", jogo->consultas_restantes_rodada);
            printf("Deseja gastar sua consulta estratégica? (1-Sim / 0-Não): ");
            int gastar;
            scanf("%d", &gastar);
            if (gastar == 1 && jogo->consultas_restantes_rodada > 0) {
                exibir_painel_transparencia(jogo, true);
                jogo->consultas_restantes_rodada--;
            }
        }

        printf("\nEscolha sua ação: (1) Hit (Pedir Carta) ou (2) Stand (Manter)? ");
        scanf("%d", &acao);

        if (acao == 1) {
            Carta nova = desempilhar(&jogo->topo_baralho);
            jogo->cartas_restantes--;
            registrar_saida_carta(jogo, nova);
            printf("Você comprou: [%s de %s]\n", nova.identidade, nova.naipe);
            adicionar_carta_na_mao(&jogador, nova);
        } else {
            break;
        }
    }

    if (jogador.pontuacao > 21) {
        printf("\nSua pontuação final: %d. Você ESTOUROU (Bust)!\n", jogador.pontuacao);
        printf("A casa recolhe a aposta sem precisar jogar.\n");
        registrar_saida_carta(jogo, cc2); 
    } else {
        registrar_saida_carta(jogo, cc2); 
        printf("\n--- Turno da Banca ---\n");
        printf("Banca revela a carta oculta: [%s de %s]\n", cc2.identidade, cc2.naipe);

        while (casa.pontuacao < 17) {
            printf("Banca está com %d pontos e compra uma carta...\n", casa.pontuacao);
            Carta nova = desempilhar(&jogo->topo_baralho);
            jogo->cartas_restantes--;
            registrar_saida_carta(jogo, nova);
            adicionar_carta_na_mao(&casa, nova);
        }
        printf("Pontuação final da Banca: %d\n", casa.pontuacao);

        if (casa.pontuacao > 21) {
            printf("A Banca estourou! Você venceu a rodada.\n");
            jogo->saldo += 100.0; 
        } else if (jogador.pontuacao > casa.pontuacao) {
            printf("Você tem mais pontos que a Banca! Você venceu a rodada.\n");
            jogo->saldo += 100.0;
        } else if (jogador.pontuacao < casa.pontuacao) {
            printf("A Banca fez mais pontos. Você perdeu a rodada.\n");
        } else {
            printf("Empate (Push)! O saldo da aposta foi devolvido.\n");
            jogo->saldo += 50.0;
        }
    }

    liberar_mao(&jogador);
    liberar_mao(&casa);
}