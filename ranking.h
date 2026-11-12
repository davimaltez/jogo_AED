#ifndef RANKING_H
#define RANKING_H

#include <stdbool.h>

#define MAX_RANKING 10      /* quantas posicoes cada musica guarda */
#define TAMANHO_NOME 16     /* ate 15 caracteres + '\0' */

/*
 * Uma linha do ranking. O campo "recente" nao vai para o arquivo:
 * ele so marca a pontuacao que acabou de ser inserida, para descobrir
 * em que posicao ela ficou depois da ordenacao.
 */
typedef struct {
    char nome[TAMANHO_NOME];
    int pontuacao;
    bool recente;
} RegistroRanking;

/*
 * Vetor de registros de uma musica.
 * Tem uma posicao a mais que MAX_RANKING para caber a pontuacao nova
 * antes de ordenar; depois da ordenacao, a ultima e descartada.
 */
typedef struct {
    RegistroRanking registros[MAX_RANKING + 1];
    int quantidade;
} Ranking;

/* Deixa o ranking vazio. */
void iniciar_ranking(Ranking *ranking);

/*
 * Le o arquivo da musica (uma linha "pontuacao;nome" por registro).
 * Arquivo inexistente significa ranking vazio e nao e erro.
 * Linhas invalidas sao ignoradas. Ao final, ordena com Merge Sort.
 * Retorna false apenas se o arquivo existir e nao puder ser lido.
 */
bool carregar_ranking(Ranking *ranking, const char *caminho);

/* Grava o ranking no arquivo. Retorna false se nao conseguir gravar. */
bool salvar_ranking(const Ranking *ranking, const char *caminho);

/*
 * Merge Sort do vetor de registros:
 * maior pontuacao primeiro; empate decidido pelo nome em ordem A-Z.
 * E estavel: registros totalmente iguais mantem a ordem de chegada.
 */
void ordenar_ranking(Ranking *ranking);

/*
 * Insere uma pontuacao, ordena e mantem so os MAX_RANKING melhores.
 * Retorna a posicao (0 = primeiro lugar) ou -1 se ficou fora do top.
 */
int inserir_no_ranking(Ranking *ranking, const char *nome, int pontuacao);

/* Caracteres aceitos no nome: letras sem acento, digitos, espaco, '-' e '_'. */
bool caractere_valido_nome(int caractere);

#endif /* RANKING_H */
