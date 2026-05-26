#ifndef BARALHO_H
#define BARALHO_H

#include <stdbool.h>

typedef struct {
    char naipe[10];
    char identidade[3];
    int peso;
} Carta;

typedef struct No {
    Carta carta;
    struct No* proximo;
} No;

void empilhar(No** topo, Carta carta);
Carta desempilhar(No** topo);
void destruir_baralho(No** topo);

#endif