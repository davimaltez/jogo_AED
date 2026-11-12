#define _POSIX_C_SOURCE 200809L
/* Testa o ranking: Merge Sort, insercao, limite do top e arquivo. */
#include "ranking.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void adicionar(Ranking *ranking, const char *nome, int pontuacao) {
    RegistroRanking *registro = &ranking->registros[ranking->quantidade++];
    strcpy(registro->nome, nome);
    registro->pontuacao = pontuacao;
    registro->recente = false;
}

static void testar_ordenacao(void) {
    Ranking ranking;
    iniciar_ranking(&ranking);
    adicionar(&ranking, "Bia", 300);
    adicionar(&ranking, "Caio", 900);
    adicionar(&ranking, "Ana", 300);
    adicionar(&ranking, "Davi", 100);
    adicionar(&ranking, "Ana", 300);
    ranking.registros[4].recente = true;   /* marca o segundo "Ana 300" */
    ordenar_ranking(&ranking);

    /* Maior pontuacao primeiro; empate pelo nome A-Z. */
    assert(ranking.registros[0].pontuacao == 900);
    assert(strcmp(ranking.registros[1].nome, "Ana") == 0 && !ranking.registros[1].recente);
    /* Estavel: o "Ana 300" que chegou depois continua depois. */
    assert(strcmp(ranking.registros[2].nome, "Ana") == 0 && ranking.registros[2].recente);
    assert(strcmp(ranking.registros[3].nome, "Bia") == 0);
    assert(ranking.registros[4].pontuacao == 100);

    /* Vazio e um elemento nao quebram. */
    iniciar_ranking(&ranking);
    ordenar_ranking(&ranking);
    adicionar(&ranking, "Solo", 5);
    ordenar_ranking(&ranking);
    assert(ranking.quantidade == 1 && ranking.registros[0].pontuacao == 5);
}

static void testar_insercao(void) {
    Ranking ranking;
    iniciar_ranking(&ranking);
    assert(inserir_no_ranking(&ranking, "Lais", 500) == 0);
    assert(inserir_no_ranking(&ranking, "Rui", 800) == 0);
    assert(inserir_no_ranking(&ranking, "Eva", 500) == 1);   /* Eva vem antes de Lais */
    assert(inserir_no_ranking(&ranking, "", 10) == 3);       /* nome vazio vira JOGADOR */
    assert(strcmp(ranking.registros[3].nome, "JOGADOR") == 0);
    assert(inserir_no_ranking(&ranking, "a;b", 1) == 4);     /* ';' quebraria o arquivo */
    assert(strcmp(ranking.registros[4].nome, "JOGADOR") == 0);

    /* Enche o top 10 e confere o corte. */
    iniciar_ranking(&ranking);
    for (int i = 0; i < MAX_RANKING; i++) {
        assert(inserir_no_ranking(&ranking, "Base", 1000 + i * 10) >= 0);
    }
    assert(ranking.quantidade == MAX_RANKING);
    assert(inserir_no_ranking(&ranking, "Fraco", 5) == -1);      /* nao entra */
    assert(ranking.quantidade == MAX_RANKING);
    assert(inserir_no_ranking(&ranking, "Campeao", 5000) == 0);  /* entra em primeiro */
    assert(ranking.quantidade == MAX_RANKING);
    assert(ranking.registros[MAX_RANKING - 1].pontuacao == 1010); /* o pior saiu */
    for (int i = 0; i + 1 < ranking.quantidade; i++) {
        assert(ranking.registros[i].pontuacao >= ranking.registros[i + 1].pontuacao);
    }
}

static void testar_arquivo(void) {
    char caminho[] = "/tmp/final_act_ranking_XXXXXX";
    int descritor = mkstemp(caminho);
    assert(descritor >= 0 && close(descritor) == 0);

    /* Arquivo editado a mao: fora de ordem e com linhas invalidas. */
    FILE *arquivo = fopen(caminho, "w");
    assert(arquivo != NULL);
    fputs("100;Zeca\n", arquivo);
    fputs("linha sem formato\n", arquivo);
    fputs("-5;Negativo\n", arquivo);
    fputs("700;Maria Clara\r\n", arquivo);
    fputs("300;Nome@Invalido\n", arquivo);
    fputs("700;Bruno\n", arquivo);
    assert(fclose(arquivo) == 0);

    Ranking ranking;
    assert(carregar_ranking(&ranking, caminho));
    assert(ranking.quantidade == 3);
    assert(strcmp(ranking.registros[0].nome, "Bruno") == 0);
    assert(strcmp(ranking.registros[1].nome, "Maria Clara") == 0);
    assert(strcmp(ranking.registros[2].nome, "Zeca") == 0);

    /* Grava e le de novo: o conteudo precisa ser o mesmo. */
    assert(inserir_no_ranking(&ranking, "Lais", 400) == 2);
    assert(salvar_ranking(&ranking, caminho));
    Ranking relido;
    assert(carregar_ranking(&relido, caminho));
    assert(relido.quantidade == 4);
    for (int i = 0; i < relido.quantidade; i++) {
        assert(relido.registros[i].pontuacao == ranking.registros[i].pontuacao);
        assert(strcmp(relido.registros[i].nome, ranking.registros[i].nome) == 0);
        assert(!relido.registros[i].recente);
    }
    assert(unlink(caminho) == 0);

    /* Arquivo inexistente: ranking vazio, sem erro. */
    assert(carregar_ranking(&ranking, caminho) && ranking.quantidade == 0);
    /* Pasta inexistente: nao consegue salvar e avisa. */
    assert(!salvar_ranking(&relido, "/tmp/pasta_que_nao_existe_final_act/ranking.txt"));
}

int main(void) {
    testar_ordenacao();
    testar_insercao();
    testar_arquivo();
    puts("OK: ranking com Merge Sort estavel, top 10, nomes e arquivo.");
    return 0;
}
