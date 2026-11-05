#include "desempenho.h"

void iniciar_desempenho(Desempenho *desempenho) {
    desempenho->valor = DESEMPENHO_INICIAL;
    desempenho->game_over = false;
}

void registrar_desempenho(Desempenho *desempenho, bool acertou) {
    /* Uma partida encerrada so pode ser recuperada pelo reinicio. */
    if (desempenho->game_over) return;

    desempenho->valor += acertou ? GANHO_ACERTO : -PERDA_ERRO;
    if (desempenho->valor > DESEMPENHO_MAXIMO) {
        desempenho->valor = DESEMPENHO_MAXIMO;
    }
    if (desempenho->valor <= 0) {
        desempenho->valor = 0;
        desempenho->game_over = true;
    }
}
