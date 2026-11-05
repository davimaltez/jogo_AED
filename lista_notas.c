#include "lista_notas.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

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

/* O ponteiro rapido encontra o fim enquanto o lento encontra o meio. */
static Nota *dividir_lista(Nota *cabeca) {
    Nota *lento = cabeca;
    Nota *rapido = cabeca->proximo;
    while (rapido != NULL && rapido->proximo != NULL) {
        lento = lento->proximo;
        rapido = rapido->proximo->proximo;
    }
    Nota *segunda_metade = lento->proximo;
    lento->proximo = NULL;
    return segunda_metade;
}

static Nota *intercalar_listas(Nota *esquerda, Nota *direita) {
    Nota *resultado = NULL;
    Nota **proxima_ligacao = &resultado;
    while (esquerda != NULL && direita != NULL) {
        /* Escolher a esquerda no empate torna a ordenacao estavel. */
        if (esquerda->tempo_alvo <= direita->tempo_alvo) {
            *proxima_ligacao = esquerda;
            esquerda = esquerda->proximo;
        } else {
            *proxima_ligacao = direita;
            direita = direita->proximo;
        }
        proxima_ligacao = &(*proxima_ligacao)->proximo;
    }
    *proxima_ligacao = esquerda != NULL ? esquerda : direita;
    return resultado;
}

void merge_sort_notas(Nota **cabeca) {
    if (cabeca == NULL || *cabeca == NULL || (*cabeca)->proximo == NULL) {
        return;
    }
    Nota *segunda_metade = dividir_lista(*cabeca);
    merge_sort_notas(cabeca);
    merge_sort_notas(&segunda_metade);
    *cabeca = intercalar_listas(*cabeca, segunda_metade);
}

static void informar_erro(char *mensagem, size_t tamanho,
                         const char *causa, size_t numero_linha) {
    if (mensagem == NULL || tamanho == 0) return;
    if (numero_linha == 0) {
        snprintf(mensagem, tamanho, "%s", causa);
    } else {
        snprintf(mensagem, tamanho, "Linha %zu: %s", numero_linha, causa);
    }
}

/* Retorna 1 para linha completa, 0 para fim e valores negativos para erros. */
static int ler_linha_notas(FILE *arquivo, char linha[257]) {
    size_t tamanho = 0;
    int caractere;
    while ((caractere = fgetc(arquivo)) != EOF && caractere != '\n') {
        if (caractere == '\0') return -3;
        if (tamanho == 256) return -2;
        linha[tamanho++] = (char)caractere;
    }
    if (ferror(arquivo)) return -1;
    if (caractere == EOF && tamanho == 0) return 0;
    if (tamanho > 0 && linha[tamanho - 1] == '\r') tamanho--;
    if (tamanho > 255) return -2;
    linha[tamanho] = '\0';
    return 1;
}

static bool interpretar_evento(char *linha, float *tempo, int *pista,
                              const char **causa) {
    char *fim;
    errno = 0;
    *tempo = strtof(linha, &fim);
    if (fim == linha || errno == ERANGE || !isfinite(*tempo) || *tempo < 0.0f) {
        *causa = "Tempo invalido.";
        return false;
    }
    /* O formato aceita numeros decimais, incluindo notacao cientifica. */
    for (char *atual = linha; atual < fim; atual++) {
        if (!isdigit((unsigned char)*atual) && *atual != '+' && *atual != '-' &&
            *atual != '.' && *atual != 'e' && *atual != 'E') {
            *causa = "Tempo invalido.";
            return false;
        }
    }
    if (!isspace((unsigned char)*fim)) {
        *causa = "Use tempo e pista separados por espaco.";
        return false;
    }
    char *inicio_pista = fim;
    while (isspace((unsigned char)*inicio_pista)) inicio_pista++;
    errno = 0;
    long pista_lida = strtol(inicio_pista, &fim, 10);
    if (fim == inicio_pista || errno == ERANGE || pista_lida < 0 || pista_lida >= TOTAL_PISTAS) {
        *causa = "Pista invalida (use 0 a 4).";
        return false;
    }
    while (isspace((unsigned char)*fim)) fim++;
    if (*fim != '\0') {
        *causa = "Registro malformado ou campos extras.";
        return false;
    }
    *pista = (int)pista_lida;
    return true;
}

bool carregar_notas_arquivo(Nota **cabeca, const char *caminho,
                           char *mensagem_erro, size_t tamanho_mensagem) {
    informar_erro(mensagem_erro, tamanho_mensagem, "", 0);
    if (cabeca == NULL || caminho == NULL || *caminho == '\0') {
        informar_erro(mensagem_erro, tamanho_mensagem, "Destino ou caminho invalido.", 0);
        return false;
    }
    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        informar_erro(mensagem_erro, tamanho_mensagem, "Nao foi possivel abrir o arquivo.", 0);
        return false;
    }

    Nota *nova_lista = NULL;
    Nota *ultima_nota = NULL;
    char linha[257];
    size_t numero_linha = 0;
    const char *causa = NULL;
    int leitura;
    while ((leitura = ler_linha_notas(arquivo, linha)) > 0) {
        numero_linha++;
        char *inicio = linha;
        while (isspace((unsigned char)*inicio)) inicio++;
        if (*inicio == '\0' || *inicio == '#') continue;

        float tempo;
        int pista;
        if (!interpretar_evento(inicio, &tempo, &pista, &causa)) break;
        Nota *nova_nota = criar_nota(tempo, pista);
        if (nova_nota == NULL) {
            causa = "Memoria insuficiente para carregar as notas.";
            break;
        }
        /* Anexa em O(1), mantendo a ordem de leitura antes do Merge Sort. */
        if (ultima_nota == NULL) nova_lista = nova_nota;
        else ultima_nota->proximo = nova_nota;
        ultima_nota = nova_nota;
    }
    if (leitura < 0) {
        numero_linha++;
        causa = leitura == -1 ? "Falha de leitura do arquivo." :
                leitura == -2 ? "Linha excede 255 caracteres." : "Caractere nulo no arquivo.";
    }
    if (fclose(arquivo) != 0 && causa == NULL) {
        causa = "Falha ao fechar o arquivo.";
    }
    if (causa == NULL && nova_lista == NULL) {
        causa = "Arquivo sem notas.";
        numero_linha = 0;
    }
    if (causa != NULL) {
        liberar_lista_notas(&nova_lista);
        informar_erro(mensagem_erro, tamanho_mensagem, causa, numero_linha);
        return false;
    }
    merge_sort_notas(&nova_lista);
    /* A lista anterior so e descartada quando a substituta esta pronta. */
    liberar_lista_notas(cabeca);
    *cabeca = nova_lista;
    return true;
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

bool todas_notas_resolvidas(const Nota *cabeca) {
    if (cabeca == NULL) return false;
    const Nota *atual = cabeca;
    while (atual != NULL) {
        if (atual->estado != NOTA_ACERTADA && atual->estado != NOTA_PERDIDA) {
            return false;
        }
        atual = atual->proximo;
    }
    return true;
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

