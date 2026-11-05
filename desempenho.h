#ifndef DESEMPENHO_H
#define DESEMPENHO_H

#include <stdbool.h>

#define DESEMPENHO_INICIAL 75
#define DESEMPENHO_MAXIMO 100
#define GANHO_ACERTO 3
#define PERDA_ERRO 8
#define LIMITE_VERMELHO 30
#define LIMITE_VERDE 60

typedef struct {
    int valor;
    bool game_over;
} Desempenho;

void iniciar_desempenho(Desempenho *desempenho);
void registrar_desempenho(Desempenho *desempenho, bool acertou);

#endif
