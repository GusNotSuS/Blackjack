#include "baralho.h"
#include <stdlib.h>
#include <stddef.h>

void empilhar(No **topo, Carta carta) {
  No *novo = (No *)malloc(sizeof(No));
  if (!novo) {
    fprintf(stderr, "Erro: memória insuficiente ao empilhar carta\n");
    exit(1);
  }
  novo->carta = carta;
  novo->proximo = *topo;
  *topo = novo;
}

Carta desempilhar(No **topo) {
  if (*topo == NULL) {
    Carta vazia = {"", "", 0};
    return vazia;
  }
  No *aux = *topo;
  Carta carta = aux->carta;
  *topo = aux->proximo;
  free(aux);
  return carta;
}

void destruir_baralho(No **topo) {
  while (*topo != NULL) {
    desempilhar(topo);
  }
}