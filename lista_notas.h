#ifndef LISTA_NOTAS_H
#define LISTA_NOTAS_H

#include <stdbool.h>

/*
 * Situacao de cada nota no ciclo de jogo.
 */
typedef enum {
    NOTA_PENDENTE,   /* Nota ainda nao alcancou a linha ou aguarda acerto */
    NOTA_ACERTADA,   /* Jogador pressionou a tecla correta na janela */
    NOTA_PERDIDA     /* Passou da linha sem acerto */
} EstadoNota;

/*
 * No da lista simplesmente encadeada representando uma nota musical.
 */
typedef struct Nota {
    float tempo_alvo;       /* Instante em que a nota deve ser tocada (em segundos) */
    int pista;              /* Pista correspondente: 0 = D, 1 = F, 2 = J, 3 = K */
    EstadoNota estado;      /* Pendente, acertada ou perdida */
    struct Nota *proximo;   /* Ponteiro para o proximo no da lista */
} Nota;

/*
 * Aloca dinamicamente e inicializa um novo no de nota.
 */
Nota *criar_nota(float tempo_alvo, int pista);

/*
 * Insere uma nova nota no final da lista encadeada.
 */
void inserir_nota(Nota **cabeca, float tempo_alvo, int pista);

/*
 * Gera uma sequencia de notas ritmicas iniciais com base na dificuldade da musica.
 */
void carregar_notas_iniciais(Nota **cabeca, int dificuldade);

/*
 * Verifica se a tecla pressionada pelo jogador corresponde a uma nota pendente na pista.
 * Retorna true em caso de acerto dentro da janela de tolerancia.
 */
bool verificar_acerto(Nota *cabeca, int pista, float tempo_atual, float janela_acerto);

/*
 * Percorre a lista e marca como perdidas as notas que ultrapassaram a janela de acerto.
 * Atualiza os contadores de combo e erros.
 */
void atualizar_notas(Nota *cabeca, float tempo_atual, float limite_perda, int *combo, int *erros);

/*
 * Libera toda a memoria alocada para os nos da lista encadeada.
 */
void liberar_lista_notas(Nota **cabeca);

#endif /* LISTA_NOTAS_H */

