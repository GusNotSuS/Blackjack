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
        printf("\n[Painel Oculto - Modo Dificil ativo]\n");
        return;
    }
    
    printf("\n--- PAINEL DE TRANSPARENCIA ESTATISTICA (CONTAGEM REAL) ---\n");
    printf("Cartas revaladas fora de jogo: %d\n", 52 - jogo->cartas_restantes);
    
    char idents[13][3] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    printf("Cartas disponiveis:\n|");
    for (int i = 0; i < 13; i++) {
        printf(" %s:%d |", idents[i], jogo->vetor_frequencia[i]);
    }
    printf("\n-----------------------------------------------------------\n");
}

void jogar_rodada(Jogo* jogo) {
    if (jogo->cartas_restantes < 10) {
        printf("\n[AVISO]: O sapato possui poucas cartas. Reembaralhando...\n");
        aguardar_ms(500);
        criar_baralho(jogo);
    }

    printf("\n==================================================\n");
    printf("SALDO ATUAL: R$ %.2f\n", jogo->saldo);
    printf("==================================================\n");
    
    float aposta = 0.0;
    bool aposta_valida = false;
    
    while (!aposta_valida) {
        printf("Quanto deseja apostar nesta rodada? (Minimo: R$ 50.00 / Maximo: R$ %.2f): R$ ", jogo->saldo);
        scanf("%f", &aposta);
        
        if (aposta < 50.0) {
            printf("[Fallback]: A aposta minima permitida e de R$ 50.00.\n\n");
        } else if (aposta > jogo->saldo) {
            printf("[Fallback]: Você nao pode apostar R$ %.2f pois seu saldo atual e de R$ %.2f.\n\n", aposta, jogo->saldo);
        } else {
            aposta_valida = true;
        }
    }

    jogo->saldo -= aposta;
    printf("\nAposta aceita! R$ %.2f removidos do saldo.\n", aposta);
    aguardar_ms(500);

    Mao jogador, casa;
    inicializar_mao(&jogador);
    inicializar_mao(&casa);

    Carta c1 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, c1);
    adicionar_carta_na_mao(&jogador, c1);
    aguardar_ms(500);

    Carta cc1 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, cc1);
    adicionar_carta_na_mao(&casa, cc1);
    printf("[Banca]: Recebeu uma carta aberta.\n");
    aguardar_ms(500);

    Carta c2 = desempilhar(&jogo->topo_baralho); jogo->cartas_restantes--; registrar_saida_carta(jogo, c2);
    adicionar_carta_na_mao(&jogador, c2);
    aguardar_ms(500);

    Carta cc2 = desempilhar(&jogo->topo_baralho); 
    adicionar_carta_na_mao(&casa, cc2);
    printf("[Banca]: Recebeu uma carta oculta.\n");
    aguardar_ms(500);

    printf("\nSua Mao Inicial: ");
    No* aux = jogador.inicio;
    while(aux != NULL) {
        printf("[%s de %s] ", aux->carta.identidade, aux->carta.naipe);
        aux = aux->proximo;
    }
    printf(" -> Seus Pontos: %d\n", jogador.pontuacao);
    printf("Carta aberta da Banca: [%s de %s]\n", cc1.identidade, cc1.naipe);
    aguardar_ms(500);

    int acao = 1;
    if (jogador.pontuacao == 21) {
        printf("\nVocê atingiu a pontuação maxima de 21 pontos!\n");
        printf("Turno encerrado. Avancando para o turno da banca...\n");
        aguardar_ms(1000);
    } else {
        while (jogador.pontuacao < 21) {
            if (jogo->nivel_dificuldade == 1) { 
                exibir_painel_transparencia(jogo, false);
                printf("Sua probabilidade de ESTOURO se der Hit: %.2f%%\n", calcular_probabilidade_estouro(jogo, jogador.pontuacao));
                printf("Probabilidade de a Banca bater ou empatar com voce na carta oculta (2 carta): %.2f%%\n", calcular_probabilidade_banca_vencer(jogo, cc1.peso, jogador.pontuacao));
            } else if (jogo->nivel_dificuldade == 2) { 
                exibir_painel_transparencia(jogo, false);
            } else if (jogo->nivel_dificuldade == 3) { 
                printf("Consultas ao painel disponíveis para ESTE JOGO: %d\n", jogo->consultas_restantes_totais);
                if (jogo->consultas_restantes_totais > 0) {
                    printf("Deseja gastar uma consulta estratégica? (1-Sim / 0-Nao): ");
                    int gastar;
                    scanf("%d", &gastar);
                    if (gastar == 1) {
                        exibir_painel_transparencia(jogo, true);
                        jogo->consultas_restantes_totais--;
                        printf("Consulta realizada! Restam apenas %d consultas.\n", jogo->consultas_restantes_totais);
                        printf("Analise preditiva de Mesa: Probabilidade de a Banca igualar/vencer na carta oculta: %.2f%%\n", calcular_probabilidade_banca_vencer(jogo, cc1.peso, jogador.pontuacao));
                    }
                } else {
                    printf("[Aviso]: Suas consultas estrategicas acabaram\n");
                }
            }

            printf("\nxmlEscolha sua acao: (1) Pedir Carta) ou (2) Manter? ");
            scanf("%d", &acao);

            if (acao == 1) {
                printf("\nComprando carta...\n");
                aguardar_ms(500);
                Carta nova = desempilhar(&jogo->topo_baralho);
                jogo->cartas_restantes--;
                registrar_saida_carta(jogo, nova);
                printf("Voce comprou: [%s de %s]\n", nova.identidade, nova.naipe);
                adicionar_carta_na_mao(&jogador, nova);
                
                printf("\nSua Mao Atual: ");
                aux = jogador.inicio;
                while(aux != NULL) {
                    printf("[%s de %s] ", aux->carta.identidade, aux->carta.naipe);
                    aux = aux->proximo;
                }
                printf(" -> Seus Pontos: %d\n", jogador.pontuacao);
                aguardar_ms(500);
            } else {
                break;
            }
        }
    }

    if (jogador.pontuacao > 21) {
        printf("\nSua pontuacao final: %d. Você estourou!\n", jogador.pontuacao);
        printf("A casa recolhe a aposta sem precisar jogar.\n");
        jogo->cartas_restantes--; 
        registrar_saida_carta(jogo, cc2); 
        aguardar_ms(500);
    } else {
        jogo->cartas_restantes--; 
        registrar_saida_carta(jogo, cc2); 
        printf("\n--- Turno da Banca ---\n");
        printf("Banca revela a carta oculta: [%s de %s]\n", cc2.identidade, cc2.naipe);
        printf("Pontuacao imediata da Banca: %d\n", casa.pontuacao);
        aguardar_ms(500);

        if (casa.pontuacao < 17 && jogo->nivel_dificuldade == 1) {
            printf("Probabilidade de a Banca vencer/empatar com você na próxima compra (3ª carta): %.2f%%\n", calcular_probabilidade_banca_vencer(jogo, casa.pontuacao, jogador.pontuacao));
            aguardar_ms(500);
        }

        while (casa.pontuacao < 17) {
            printf("Banca está com %d pontos e compra uma carta...\n", casa.pontuacao);
            aguardar_ms(500);
            Carta nova = desempilhar(&jogo->topo_baralho);
            jogo->cartas_restantes--;
            registrar_saida_carta(jogo, nova);
            adicionar_carta_na_mao(&casa, nova);
            printf("Banca comprou: [%s de %s]\n", nova.identidade, nova.naipe);
            aguardar_ms(500);
        }
        printf("Pontuacao final da Banca: %d\n", casa.pontuacao);
        aguardar_ms(500);

        if (casa.pontuacao > 21) {
            printf("A Banca estourou! Voce venceu a rodada.\n");
            jogo->saldo += (aposta * 2.0); 
        } else if (jogador.pontuacao > casa.pontuacao) {
            printf("Voce tem mais pontos que a Banca! Você venceu a rodada.\n");
            jogo->saldo += (aposta * 2.0);
        } else if (jogador.pontuacao < casa.pontuacao) {
            printf("A Banca fez mais pontos. Voce perdeu a rodada.\n");
        } else {
            printf("Empate! O saldo da aposta foi devolvido.\n");
            jogo->saldo += aposta;
        }
        aguardar_ms(500);
    }

    liberar_mao(&jogador);
    liberar_mao(&casa);
}