#ifndef CATALOGO_MUSICAS_H
#define CATALOGO_MUSICAS_H

#include <stdbool.h>

#define MAX_MUSICAS 5

/*
 * Criterios de ordenacao disponiveis para o catalogo.
 */
typedef enum {
    ORDEM_TITULO_AZ,
    ORDEM_DIFICULDADE_CRESCENTE,
    ORDEM_DIFICULDADE_DECRESCENTE
} CriterioOrdenacao;

/*
 * Estrutura que representa uma musica do catalogo.
 * Armazena as informacoes cadastrais, o nivel de dificuldade
 * e os caminhos dos arquivos necessarios para a partida.
 */
typedef struct {
    char titulo[64];
    char filme[64];
    int dificuldade;
    char caminho_audio[128];
    char caminho_notas[128];
    char caminho_fundo[128];
    char caminho_ranking[128];   /* arquivo com o top 10 desta musica */
} Musica;

/*
 * Inicializa o vetor de musicas com o catalogo padrao do jogo.
 * Recebe o vetor pre-alocado e um ponteiro para registrar a quantidade de musicas.
 */
void inicializar_catalogo(Musica catalogo[], int *quantidade);

/*
 * Ordena o vetor de musicas utilizando o algoritmo Merge Sort.
 * Recebe o catalogo, o total de elementos e o criterio desejado.
 */
void ordenar_catalogo(Musica catalogo[], int total_musicas, CriterioOrdenacao criterio);

/*
 * Busca o indice de uma musica no catalogo a partir do seu titulo.
 * Retorna o indice (0 a total-1) ou -1 se nao for encontrada.
 */
int buscar_indice_por_titulo(Musica catalogo[], int total_musicas, const char *titulo);

#endif /* CATALOGO_MUSICAS_H */
