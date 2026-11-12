#define _POSIX_C_SOURCE 200809L
#include "lista_notas.h"
#include "catalogo_musicas.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* As falhas sao injetadas somente neste executavel, pelo linker GNU. */
static int alocacoes_restantes = -1;
static int caracteres_restantes = -1;
static bool leitura_falhou = false;
static bool falhar_fechamento = false;

void *__real_malloc(size_t tamanho);
int __real_fgetc(FILE *arquivo);
int __real_ferror(FILE *arquivo);
int __real_fclose(FILE *arquivo);

void *__wrap_malloc(size_t tamanho) {
    if (alocacoes_restantes == 0) return NULL;
    if (alocacoes_restantes > 0) alocacoes_restantes--;
    return __real_malloc(tamanho);
}

int __wrap_fgetc(FILE *arquivo) {
    if (caracteres_restantes == 0) {
        leitura_falhou = true;
        return EOF;
    }
    if (caracteres_restantes > 0) caracteres_restantes--;
    return __real_fgetc(arquivo);
}

int __wrap_ferror(FILE *arquivo) {
    return leitura_falhou || __real_ferror(arquivo);
}

int __wrap_fclose(FILE *arquivo) {
    int resultado = __real_fclose(arquivo);
    return falhar_fechamento ? EOF : resultado;
}

static char caminho[] = "/tmp/final_act_notas_XXXXXX";

static void escrever_arquivo(const char *conteudo, size_t tamanho) {
    FILE *arquivo = fopen(caminho, "wb");
    assert(arquivo != NULL);
    assert(fwrite(conteudo, 1, tamanho, arquivo) == tamanho);
    assert(fclose(arquivo) == 0);
}

static void testar_rejeicao(const char *conteudo, size_t tamanho, const char *causa) {
    escrever_arquivo(conteudo, tamanho);
    Nota *lista = criar_nota(8.0f, 3);
    assert(lista != NULL);
    lista->estado = NOTA_ACERTADA;
    Nota *original = lista;
    char erro[160];
    assert(!carregar_notas_arquivo(&lista, caminho, erro, sizeof(erro)));
    assert(strstr(erro, causa) != NULL);
    assert(lista == original && lista->proximo == NULL);
    assert(lista->tempo_alvo == 8.0f && lista->pista == 3 && lista->estado == NOTA_ACERTADA);
    liberar_lista_notas(&lista);
}

static void testar_formatos_invalidos(void) {
    const char *invalidos[] = {
        "2 0\nabc 1\n", "2 0\n3\n", "2 0\n3 1 extra\n", "2 0\n3 1 # comentario\n",
        "2 0\n3 1.5\n", "2 0\n3 -1\n", "2 0\n3 5\n", "2 0\n3 999999999999999999999999\n",
        "2 0\n-1 1\n", "2 0\nnan 1\n", "2 0\ninf 1\n", "2 0\n-inf 1\n",
        "2 0\n1e100 1\n", "2 0\n1e-100 1\n", "2 0\n2,5 1\n", "2 0\n0x1p2 1\n",
        "2 0\n3+1\n", "2 0\n3 1x\n", "2 0\n3 0x1\n", "2 0\n3 1 2\n"
    };
    for (size_t i = 0; i < sizeof(invalidos) / sizeof(invalidos[0]); i++) {
        testar_rejeicao(invalidos[i], strlen(invalidos[i]), "Linha 2:");
    }
    testar_rejeicao("", 0, "Arquivo sem notas");
    const char *comentarios = "  # comentario\r\n \t\n";
    testar_rejeicao(comentarios, strlen(comentarios), "Arquivo sem notas");
    const char nulo[] = "2 0\n3 1\0extra\n";
    testar_rejeicao(nulo, sizeof(nulo) - 1, "Caractere nulo");
    char longa[260];
    memset(longa, ' ', sizeof(longa));
    memcpy(longa, "2 0\n", 4);
    testar_rejeicao(longa, sizeof(longa), "Linha excede 255");
}

static void testar_arquivo_valido(void) {
    const char *conteudo = " # exemplo\r\n\t\r\n4 2\r\n 2.0\t0\r\n2e0 4\r\n3.25 1\r\n0 3";
    escrever_arquivo(conteudo, strlen(conteudo));
    Nota *lista = criar_nota(99.0f, 0);
    assert(lista != NULL);
    char erro[160] = "erro anterior";
    assert(carregar_notas_arquivo(&lista, caminho, erro, sizeof(erro)));
    assert(erro[0] == '\0');
    const float tempos[] = {0, 2, 2, 3.25f, 4};
    const int pistas[] = {3, 0, 4, 1, 2};
    Nota *atual = lista;
    for (int i = 0; i < 5; i++) {
        assert(atual != NULL);
        assert(atual->tempo_alvo == tempos[i] && atual->pista == pistas[i]);
        assert(atual->estado == NOTA_PENDENTE);
        atual = atual->proximo;
    }
    assert(atual == NULL);
    assert(verificar_acerto(lista, 0, 2.0f, 0.20f));
    assert(!verificar_acerto(lista, 0, 2.0f, 0.20f));
    assert(verificar_acerto(lista, 4, 2.0f, 0.20f));
    int combo = 2, perdas = 0;
    atualizar_notas(lista, 5.0f, 0.22f, &combo, &perdas);
    assert(combo == 0 && perdas == 3);
    atualizar_notas(lista, 6.0f, 0.22f, &combo, &perdas);
    assert(perdas == 3);
    /* Recarregar restaura os estados e substitui a lista anterior. */
    assert(carregar_notas_arquivo(&lista, caminho, NULL, 0));
    for (atual = lista; atual != NULL; atual = atual->proximo) assert(atual->estado == NOTA_PENDENTE);
    liberar_lista_notas(&lista);

    char limite[257];
    memset(limite, ' ', 255);
    memcpy(limite, "1 4", 3);
    limite[255] = '\r';
    limite[256] = '\n';
    escrever_arquivo(limite, sizeof(limite));
    assert(carregar_notas_arquivo(&lista, caminho, erro, sizeof(erro)));
    assert(lista->tempo_alvo == 1 && lista->pista == 4 && lista->proximo == NULL);
    liberar_lista_notas(&lista);
    escrever_arquivo(limite, 255);
    assert(carregar_notas_arquivo(&lista, caminho, erro, sizeof(erro)));
    liberar_lista_notas(&lista);
}

static void testar_falhas(void) {
    Nota *lista = criar_nota(9, 2);
    assert(lista != NULL);
    Nota *original = lista;
    char erro[160];
    assert(!carregar_notas_arquivo(&lista, NULL, erro, sizeof(erro)));
    assert(!carregar_notas_arquivo(&lista, "", erro, sizeof(erro)));
    assert(!carregar_notas_arquivo(NULL, caminho, erro, sizeof(erro)));
    assert(!carregar_notas_arquivo(&lista, "/dev/null/arquivo_inexistente", erro, sizeof(erro)));
    assert(strstr(erro, "abrir") != NULL);
    char minimo = 'x';
    assert(!carregar_notas_arquivo(&lista, "", &minimo, 1) && minimo == '\0');
    assert(!carregar_notas_arquivo(&lista, "", NULL, 10));

    escrever_arquivo("4 2\n2 0\n2 4\n", 12);
    for (int i = 0; i < 3; i++) {
        alocacoes_restantes = i;
        assert(!carregar_notas_arquivo(&lista, caminho, erro, sizeof(erro)));
        alocacoes_restantes = -1;
        assert(strstr(erro, "Memoria insuficiente") != NULL);
        assert(lista == original && lista->proximo == NULL);
    }
    /* Erro de leitura depois de ja ter alocado um no. */
    caracteres_restantes = 5;
    assert(!carregar_notas_arquivo(&lista, caminho, erro, sizeof(erro)));
    caracteres_restantes = -1;
    leitura_falhou = false;
    assert(strstr(erro, "Falha de leitura") != NULL && lista == original);
    falhar_fechamento = true;
    assert(!carregar_notas_arquivo(&lista, caminho, erro, sizeof(erro)));
    falhar_fechamento = false;
    assert(strstr(erro, "Falha ao fechar") != NULL && lista == original);
    assert(lista->tempo_alvo == 9 && lista->pista == 2 && lista->estado == NOTA_PENDENTE);
    liberar_lista_notas(&lista);
}

static void testar_arquivos_do_catalogo(void) {
    Musica catalogo[MAX_MUSICAS];
    int quantidade;
    inicializar_catalogo(catalogo, &quantidade);
    for (int i = 0; i < quantidade; i++) {
        /* Cada musica pode ter seu proprio mapa: valida o arquivo, nao o conteudo exato. */
        Nota *carregadas = NULL;
        char erro[160];
        /* Musicas ainda nao adicionadas pelo grupo sao avisadas, sem travar os testes. */
        FILE *existe = fopen(catalogo[i].caminho_notas, "r");
        if (existe == NULL) {
            printf("AVISO: %s ainda nao existe; teste pulado.\n", catalogo[i].caminho_notas);
            continue;
        }
        fclose(existe);
        assert(carregar_notas_arquivo(&carregadas, catalogo[i].caminho_notas, erro, sizeof(erro)));
        assert(carregadas != NULL);
        for (Nota *carregada = carregadas; carregada != NULL; carregada = carregada->proximo) {
            assert(carregada->pista >= 0 && carregada->pista <= 4);
            assert(carregada->tempo_alvo >= 0.0f && carregada->estado == NOTA_PENDENTE);
            if (carregada->proximo != NULL) {
                assert(carregada->tempo_alvo <= carregada->proximo->tempo_alvo);
            }
        }
        liberar_lista_notas(&carregadas);
    }
}

int main(void) {
    int descritor = mkstemp(caminho);
    assert(descritor >= 0 && close(descritor) == 0);
    testar_formatos_invalidos();
    testar_arquivo_valido();
    testar_falhas();
    testar_arquivos_do_catalogo();
    assert(unlink(caminho) == 0);
    puts("OK: formato, carregamento, falhas de memoria/leitura, substituicao e musicas do catalogo.");
    return 0;
}
