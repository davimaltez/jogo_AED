#include "desempenho.h"
#include "lista_notas.h"
#include "catalogo_musicas.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    Desempenho desempenho;
    iniciar_desempenho(&desempenho);
    assert(desempenho.valor == 75 && !desempenho.game_over);
    assert(desempenho.valor >= LIMITE_VERDE);

    registrar_desempenho(&desempenho, false);
    assert(desempenho.valor == 67);
    registrar_desempenho(&desempenho, true);
    assert(desempenho.valor == 70);
    for (int i = 0; i < 50; i++) registrar_desempenho(&desempenho, true);
    assert(desempenho.valor == 100 && !desempenho.game_over);

    iniciar_desempenho(&desempenho);
    for (int i = 0; i < 9; i++) {
        registrar_desempenho(&desempenho, false);
        assert(!desempenho.game_over);
    }
    assert(desempenho.valor == 3);
    registrar_desempenho(&desempenho, false);
    assert(desempenho.valor == 0 && desempenho.game_over);
    registrar_desempenho(&desempenho, true);
    assert(desempenho.valor == 0 && desempenho.game_over);
    iniciar_desempenho(&desempenho);
    assert(desempenho.valor == 75 && !desempenho.game_over);

    /* Recuperacao de uma situacao vermelha sem reiniciar a partida. */
    for (int i = 0; i < 6; i++) registrar_desempenho(&desempenho, false);
    assert(desempenho.valor == 27 && desempenho.valor < LIMITE_VERMELHO);
    for (int i = 0; i < 11; i++) registrar_desempenho(&desempenho, true);
    assert(desempenho.valor == LIMITE_VERDE && !desempenho.game_over);

    Nota *notas = NULL;
    inserir_nota(&notas, 2.0f, 0);
    inserir_nota(&notas, 3.0f, 1);
    inserir_nota(&notas, 3.0f, 4);
    iniciar_desempenho(&desempenho);
    assert(verificar_acerto(notas, 0, 2.0f, 0.20f));
    registrar_desempenho(&desempenho, true);
    assert(!verificar_acerto(notas, 0, 2.0f, 0.20f));
    assert(desempenho.valor == 78);

    int combo = 1;
    int perdas = 0;
    atualizar_notas(notas, 3.23f, 0.22f, &combo, &perdas);
    assert(perdas == 2 && combo == 0);
    for (int i = 0; i < perdas; i++) registrar_desempenho(&desempenho, false);
    assert(desempenho.valor == 62);
    atualizar_notas(notas, 4.0f, 0.22f, &combo, &perdas);
    assert(perdas == 2); /* Nao conta novamente as notas ja perdidas. */
    liberar_lista_notas(&notas);
    assert(notas == NULL);

    /* Regressao: o catalogo continua ordenando structs com seus recursos. */
    Musica catalogo[MAX_MUSICAS];
    int total;
    inicializar_catalogo(catalogo, &total);
    ordenar_catalogo(catalogo, total, ORDEM_TITULO_AZ);
    for (int i = 1; i < total; i++) assert(strcmp(catalogo[i-1].titulo, catalogo[i].titulo) <= 0);
    ordenar_catalogo(catalogo, total, ORDEM_DIFICULDADE_CRESCENTE);
    for (int i = 1; i < total; i++) assert(catalogo[i-1].dificuldade <= catalogo[i].dificuldade);
    ordenar_catalogo(catalogo, total, ORDEM_DIFICULDADE_DECRESCENTE);
    for (int i = 1; i < total; i++) assert(catalogo[i-1].dificuldade >= catalogo[i].dificuldade);

    puts("OK: limites, recuperacao, Game Over, reinicio, notas e catalogo.");
    return 0;
}
