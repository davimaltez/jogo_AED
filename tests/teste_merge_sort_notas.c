#include "lista_notas.h"
#include <assert.h>
#include <stdio.h>

static void testar_casos_basicos(void) {
    Nota *lista = NULL;
    merge_sort_notas(NULL);
    merge_sort_notas(&lista);
    assert(lista == NULL);
    inserir_nota(&lista, 2.0f, 4);
    assert(lista != NULL);
    Nota *original = lista;
    lista->estado = NOTA_ACERTADA;
    merge_sort_notas(&lista);
    assert(lista == original && lista->proximo == NULL);
    assert(lista->tempo_alvo == 2.0f && lista->pista == 4 && lista->estado == NOTA_ACERTADA);
    liberar_lista_notas(&lista);
    liberar_lista_notas(&lista);
    assert(lista == NULL);
}

static void testar_integridade(int quantidade, bool invertida) {
    Nota *lista = NULL;
    Nota *ultima = NULL;
    Nota *originais[2048];
    int posicoes[2048];
    assert(quantidade <= 2048);
    for (int i = 0; i < quantidade; i++) {
        /* O padrao fixo repete tempos em partes diferentes da divisao. */
        int tempo = invertida ? quantidade - i : (i * 37) % 31;
        originais[i] = criar_nota((float)tempo, i % TOTAL_PISTAS);
        assert(originais[i] != NULL);
        originais[i]->estado = (EstadoNota)(i % 3);
        if (ultima == NULL) lista = originais[i];
        else ultima->proximo = originais[i];
        ultima = originais[i];
        posicoes[i] = -1;
    }
    merge_sort_notas(&lista);

    Nota *anterior = NULL;
    Nota *atual = lista;
    for (int posicao = 0; posicao < quantidade; posicao++) {
        assert(atual != NULL);
        if (anterior != NULL) assert(anterior->tempo_alvo <= atual->tempo_alvo);
        int indice = 0;
        while (indice < quantidade && originais[indice] != atual) indice++;
        assert(indice < quantidade);
        assert(posicoes[indice] == -1); /* Um endereco repetido denunciaria um ciclo. */
        posicoes[indice] = posicao;
        float tempo_esperado = invertida ? (float)(quantidade - indice) : (float)((indice * 37) % 31);
        assert(atual->tempo_alvo == tempo_esperado);
        assert(atual->pista == indice % TOTAL_PISTAS);
        assert(atual->estado == (EstadoNota)(indice % 3));
        anterior = atual;
        atual = atual->proximo;
    }
    assert(atual == NULL); /* Exatamente a quantidade original, sem nos extras. */
    for (int i = 0; i < quantidade; i++) {
        assert(posicoes[i] >= 0);
        for (int j = i + 1; j < quantidade; j++) {
            if (originais[i]->tempo_alvo == originais[j]->tempo_alvo) {
                assert(posicoes[i] < posicoes[j]);
            }
        }
    }
    /* Ordenar novamente tambem deve preservar todos os nos e os empates. */
    merge_sort_notas(&lista);
    atual = lista;
    for (int posicao = 0; posicao < quantidade; posicao++) {
        assert(atual != NULL);
        int indice = 0;
        while (indice < quantidade && originais[indice] != atual) indice++;
        assert(indice < quantidade && posicoes[indice] == posicao);
        atual = atual->proximo;
    }
    assert(atual == NULL);
    liberar_lista_notas(&lista);
    assert(lista == NULL);
}

int main(void) {
    testar_casos_basicos();
    testar_integridade(2, true);
    testar_integridade(7, false);
    testar_integridade(2048, true);
    testar_integridade(2048, false);
    puts("OK: Merge Sort, estabilidade, estados, enderecos, quantidade e ausencia de ciclos.");
    return 0;
}
