#include "ranking.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

void iniciar_ranking(Ranking *ranking) {
    ranking->quantidade = 0;
}

bool caractere_valido_nome(int caractere) {
    return (caractere >= 'A' && caractere <= 'Z') ||
           (caractere >= 'a' && caractere <= 'z') ||
           (caractere >= '0' && caractere <= '9') ||
           caractere == ' ' || caractere == '-' || caractere == '_';
}

static bool nome_valido(const char *nome) {
    if (nome[0] == '\0') {
        return false;
    }
    for (int i = 0; nome[i] != '\0'; i++) {
        if (!caractere_valido_nome((unsigned char)nome[i])) {
            return false;
        }
    }
    return true;
}

/* ---------------- Merge Sort do vetor de registros ---------------- */

/*
 * Retorna true se "a" deve ficar antes de "b" (ou empatar com ele).
 * O "<=" no empate do nome mantem a ordem original de registros iguais,
 * o que torna o Merge Sort estavel.
 */
static bool vem_antes(const RegistroRanking *a, const RegistroRanking *b) {
    if (a->pontuacao != b->pontuacao) {
        return a->pontuacao > b->pontuacao;   /* maior pontuacao primeiro */
    }
    return strcmp(a->nome, b->nome) <= 0;     /* empate: nome A-Z */
}

/* Intercala as metades ja ordenadas [inicio..meio] e [meio+1..fim]. */
static void intercalar_registros(RegistroRanking registros[], int inicio, int meio, int fim) {
    RegistroRanking auxiliar[MAX_RANKING + 1];
    int i = inicio;     /* percorre a metade esquerda */
    int j = meio + 1;   /* percorre a metade direita */
    int k = 0;          /* proxima posicao livre do auxiliar */

    while (i <= meio && j <= fim) {
        if (vem_antes(&registros[i], &registros[j])) {
            auxiliar[k++] = registros[i++];
        } else {
            auxiliar[k++] = registros[j++];
        }
    }
    while (i <= meio) {
        auxiliar[k++] = registros[i++];
    }
    while (j <= fim) {
        auxiliar[k++] = registros[j++];
    }
    for (int p = 0; p < k; p++) {
        registros[inicio + p] = auxiliar[p];
    }
}

static void merge_sort_registros(RegistroRanking registros[], int inicio, int fim) {
    if (inicio >= fim) {
        return;   /* 0 ou 1 elemento ja esta ordenado */
    }
    int meio = inicio + (fim - inicio) / 2;
    merge_sort_registros(registros, inicio, meio);
    merge_sort_registros(registros, meio + 1, fim);
    intercalar_registros(registros, inicio, meio, fim);
}

void ordenar_ranking(Ranking *ranking) {
    if (ranking->quantidade > 1) {
        merge_sort_registros(ranking->registros, 0, ranking->quantidade - 1);
    }
}

/* ---------------- Insercao ---------------- */

int inserir_no_ranking(Ranking *ranking, const char *nome, int pontuacao) {
    if (nome == NULL || !nome_valido(nome)) {
        nome = "JOGADOR";
    }
    for (int i = 0; i < ranking->quantidade; i++) {
        ranking->registros[i].recente = false;
    }

    /* A posicao extra do vetor recebe o novo registro antes da ordenacao. */
    RegistroRanking *novo = &ranking->registros[ranking->quantidade];
    strncpy(novo->nome, nome, TAMANHO_NOME - 1);
    novo->nome[TAMANHO_NOME - 1] = '\0';
    novo->pontuacao = pontuacao;
    novo->recente = true;
    ranking->quantidade++;

    ordenar_ranking(ranking);

    int posicao = -1;
    for (int i = 0; i < ranking->quantidade; i++) {
        if (ranking->registros[i].recente) {
            posicao = i;
        }
    }
    /* Mantem apenas os melhores; o que passou do limite e descartado. */
    if (ranking->quantidade > MAX_RANKING) {
        ranking->quantidade = MAX_RANKING;
    }
    if (posicao >= MAX_RANKING) {
        posicao = -1;
    }
    return posicao;
}

/* ---------------- Arquivo ---------------- */

bool carregar_ranking(Ranking *ranking, const char *caminho) {
    iniciar_ranking(ranking);

    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        
        return errno == ENOENT;
    }

    char linha[64];
    while (ranking->quantidade < MAX_RANKING && fgets(linha, sizeof(linha), arquivo) != NULL) {
        RegistroRanking registro;
        
        if (sscanf(linha, "%d;%15[^\r\n]", &registro.pontuacao, registro.nome) == 2 &&
            registro.pontuacao >= 0 && nome_valido(registro.nome)) {
            registro.recente = false;
            ranking->registros[ranking->quantidade] = registro;
            ranking->quantidade++;
        }
    }

    bool leitura_ok = !ferror(arquivo);
    if (fclose(arquivo) != 0) {
        leitura_ok = false;
    }

    ordenar_ranking(ranking);
    return leitura_ok;
}

bool salvar_ranking(const Ranking *ranking, const char *caminho) {
    FILE *arquivo = fopen(caminho, "w");
    if (arquivo == NULL) {
        return false;
    }
    bool gravou = true;
    for (int i = 0; i < ranking->quantidade; i++) {
        if (fprintf(arquivo, "%d;%s\n", ranking->registros[i].pontuacao,
                    ranking->registros[i].nome) < 0) {
            gravou = false;
        }
    }
    if (fclose(arquivo) != 0) {
        gravou = false;
    }
    return gravou;
}
