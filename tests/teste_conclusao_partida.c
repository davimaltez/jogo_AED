#include "lista_notas.h"
#include "desempenho.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void testar_lista_vazia_e_acerto_final(void) {
    Nota *lista = NULL;
    assert(!todas_notas_resolvidas(lista));
    inserir_nota(&lista, 2.0f, 0);
    assert(lista != NULL && !todas_notas_resolvidas(lista));
    assert(!verificar_acerto(lista, 0, 1.7f, 0.20f));
    assert(!todas_notas_resolvidas(lista));
    assert(verificar_acerto(lista, 0, 1.875f, 0.20f));
    assert(todas_notas_resolvidas(lista));
    /* A consulta nao altera os dados, ligacoes ou estado do no. */
    assert(lista->tempo_alvo == 2.0f && lista->pista == 0);
    assert(lista->estado == NOTA_ACERTADA && lista->proximo == NULL);
    liberar_lista_notas(&lista);
    assert(!todas_notas_resolvidas(lista));
}

static void testar_limite_da_perda(void) {
    Nota *lista = criar_nota(2.0f, 4);
    assert(lista != NULL);
    int combo = 3, perdas = 0;
    float limite = lista->tempo_alvo + 0.22f;
    atualizar_notas(lista, limite, 0.22f, &combo, &perdas);
    assert(!todas_notas_resolvidas(lista) && perdas == 0 && combo == 3);
    atualizar_notas(lista, nextafterf(limite, INFINITY), 0.22f, &combo, &perdas);
    assert(todas_notas_resolvidas(lista) && perdas == 1 && combo == 0);
    atualizar_notas(lista, 10.0f, 0.22f, &combo, &perdas);
    assert(todas_notas_resolvidas(lista) && perdas == 1);
    liberar_lista_notas(&lista);
}

static void testar_notas_simultaneas(void) {
    Nota *lista = NULL;
    inserir_nota(&lista, 1.0f, 1);
    inserir_nota(&lista, 2.0f, 0);
    inserir_nota(&lista, 2.0f, 4);
    assert(lista != NULL && lista->proximo != NULL && lista->proximo->proximo != NULL);
    assert(verificar_acerto(lista, 1, 1.0f, 0.20f));
    assert(!todas_notas_resolvidas(lista));
    assert(verificar_acerto(lista, 0, 2.0f, 0.20f));
    assert(!todas_notas_resolvidas(lista));
    assert(verificar_acerto(lista, 4, 2.0f, 0.20f));
    assert(todas_notas_resolvidas(lista));
    liberar_lista_notas(&lista);
}

static void testar_todas_perdidas_sem_derrota(void) {
    Nota *lista = NULL;
    inserir_nota(&lista, 1.0f, 0);
    inserir_nota(&lista, 2.0f, 1);
    inserir_nota(&lista, 2.0f, 4);
    int perdas = 0, combo = 0;
    Desempenho desempenho;
    iniciar_desempenho(&desempenho);
    atualizar_notas(lista, 3.0f, 0.22f, &combo, &perdas);
    for (int i = 0; i < perdas; i++) registrar_desempenho(&desempenho, false);
    assert(perdas == 3 && todas_notas_resolvidas(lista));
    assert(desempenho.valor == 51 && !desempenho.game_over);
    liberar_lista_notas(&lista);
}

int main(void) {
    testar_lista_vazia_e_acerto_final();
    testar_limite_da_perda();
    testar_notas_simultaneas();
    testar_todas_perdidas_sem_derrota();
    puts("OK: conclusao, pendencias, acerto final, limite de perda e notas simultaneas.");
    return 0;
}
