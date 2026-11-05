#include "lista_notas.h"
#include <stdlib.h>
#include <math.h>

/*
 * Aloca dinamicamente um novo no de nota na memoria heap.
 * Verifica o retorno do malloc para garantir a robustez do programa.
 */
Nota *criar_nota(float tempo_alvo, int pista) {
    Nota *nova_nota = (Nota *)malloc(sizeof(Nota));
    if (nova_nota == NULL) {
        return NULL;
    }

    nova_nota->tempo_alvo = tempo_alvo;
    nova_nota->pista = pista;
    nova_nota->estado = NOTA_PENDENTE;
    nova_nota->proximo = NULL;

    return nova_nota;
}

/*
 * Insere um novo no no final da lista encadeada.
 * Se a lista estiver vazia (*cabeca == NULL), o novo no se torna a cabeca.
 */
void inserir_nota(Nota **cabeca, float tempo_alvo, int pista) {
    Nota *nova_nota = criar_nota(tempo_alvo, pista);
    if (nova_nota == NULL) {
        return;
    }

    if (*cabeca == NULL) {
        *cabeca = nova_nota;
    } else {
        Nota *atual = *cabeca;
        while (atual->proximo != NULL) {
            atual = atual->proximo;
        }
        atual->proximo = nova_nota;
    }
}

/*
 * Carrega uma sequencia inicial de notas sincronizadas com o tempo.
 * As notas sao geradas em sequencias ritmicas nas 5 pistas (0 a 4).
 */
void carregar_notas_iniciais(Nota **cabeca, int dificuldade) {
    /* Garante que a lista comece vazia antes de carregar */
    liberar_lista_notas(cabeca);

    /* Quantidade de notas proporcional ao nivel de dificuldade (1 a 5) */
    int total_notas = 20 + (dificuldade * 6);
    float tempo_inicio = 2.0f; /* Primeiras notas chegam apos 2 segundos */
    float intervalo = 1.0f - (dificuldade * 0.08f);
    if (intervalo < 0.45f) intervalo = 0.45f;

    /* Padrao de pistas ritmicas */
    int padrao_pistas[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 2, 4, 1, 3, 0, 4, 2 };
    int total_padrao = sizeof(padrao_pistas) / sizeof(padrao_pistas[0]);

    for (int i = 0; i < total_notas; i++) {
        float tempo = tempo_inicio + (i * intervalo);
        int pista = padrao_pistas[i % total_padrao];

        inserir_nota(cabeca, tempo, pista);

        /* Em dificuldades maiores (>= 3), insere algumas notas duplas ocasionais */
        if (dificuldade >= 3 && i % 5 == 0 && i > 0) {
            int pista_dupla = (pista + 2) % TOTAL_PISTAS;
            inserir_nota(cabeca, tempo, pista_dupla);
        }
    }
}

/*
 * Percorre a lista encadeada e verifica se ha uma nota pendente na pista informada
 * que esteja dentro da janela de acerto tolerada.
 */
bool verificar_acerto(Nota *cabeca, int pista, float tempo_atual, float janela_acerto) {
    Nota *atual = cabeca;

    while (atual != NULL) {
        if (atual->estado == NOTA_PENDENTE && atual->pista == pista) {
            float diferenca = (float)fabs(tempo_atual - atual->tempo_alvo);

            /* Se a nota esta dentro do intervalo de acerto permitido */
            if (diferenca <= janela_acerto) {
                atual->estado = NOTA_ACERTADA;
                return true;
            }
        }
        atual = atual->proximo;
    }

    return false;
}

/*
 * Atualiza o estado das notas conforme o tempo avanca.
 * Notas pendentes que passaram da linha de acerto alem da tolerancia sao marcadas como perdidas.
 */
void atualizar_notas(Nota *cabeca, float tempo_atual, float limite_perda, int *combo, int *erros) {
    Nota *atual = cabeca;

    while (atual != NULL) {
        if (atual->estado == NOTA_PENDENTE) {
            /* Se o tempo atual ultrapassou o alvo da nota alem do limite de tolerancia */
            if (tempo_atual > (atual->tempo_alvo + limite_perda)) {
                atual->estado = NOTA_PERDIDA;
                if (combo != NULL) *combo = 0;
                if (erros != NULL) (*erros)++;
            }
        }
        atual = atual->proximo;
    }
}

/*
 * Libera todos os nos da lista encadeada da memoria heap.
 * Armazena temporariamente o ponteiro 'proximo' antes de dar free no no atual.
 */
void liberar_lista_notas(Nota **cabeca) {
    if (cabeca == NULL || *cabeca == NULL) {
        return;
    }

    Nota *atual = *cabeca;
    while (atual != NULL) {
        Nota *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }

    *cabeca = NULL;
}

