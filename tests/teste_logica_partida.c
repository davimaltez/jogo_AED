#define _POSIX_C_SOURCE 200809L
/*
 * Testa o modulo partida sem abrir janela: as teclas e o tempo sao
 * passados diretamente para atualizar_partida.
 * Os cenarios e valores sao os mesmos do antigo teste grafico.
 */
#include "partida.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static char caminho[] = "/tmp/final_act_logica_XXXXXX";

static void escrever_notas(const char *conteudo) {
    FILE *arquivo = fopen(caminho, "w");
    assert(arquivo != NULL && fputs(conteudo, arquivo) >= 0 && fclose(arquivo) == 0);
}

/* Monta o vetor de teclas: bit 0 = D, bit 1 = F, ... bit 4 = K. */
static void teclas_de(unsigned int bits, bool teclas[TOTAL_PISTAS]) {
    for (int pista = 0; pista < TOTAL_PISTAS; pista++) {
        teclas[pista] = (bits & (1u << pista)) != 0;
    }
}

static void quadro(Partida *partida, float tempo, unsigned int bits) {
    bool teclas[TOTAL_PISTAS];
    teclas_de(bits, teclas);
    atualizar_partida(partida, tempo, teclas);
}

static void testar_relogio(void) {
    /* Bloqueada: nada muda, nem com audio. */
    assert(calcular_tempo_partida(5.0f, 0.016f, true, true, 9.0f) == 5.0f);
    /* Audio tocando: o tempo do audio manda, mesmo se o quadro demorou. */
    assert(calcular_tempo_partida(5.0f, 0.5f, false, true, 5.1f) == 5.1f);
    /* O audio nunca faz o tempo voltar. */
    assert(calcular_tempo_partida(5.0f, 0.016f, false, true, 4.98f) == 5.0f);
    /* Sem audio (ausente ou terminou): soma o tempo do quadro. */
    assert(fabsf(calcular_tempo_partida(5.0f, 0.25f, false, false, 0.0f) - 5.25f) < 1e-6f);
    puts("OK: relogio da partida (pausa, audio, sem audio).");
}

static void testar_regras_basicas(void) {
    Partida partida;
    iniciar_estrutura_partida(&partida);
    inserir_nota(&partida.notas, 2.0f, 0);
    inserir_nota(&partida.notas, 3.0f, 4);
    partida.tempo = 2.0f;

    quadro(&partida, 2.0f, 1);   /* D no tempo certo */
    assert(partida.desempenho.valor == 78 && partida.pontuacao == 100);
    assert(partida.notas_acertadas == 1 && partida.combo == 1 && partida.tempo_feedback > 0.0f);

    quadro(&partida, 2.0f, 2);   /* F sem nota */
    assert(partida.desempenho.valor == 70 && partida.combo == 0 && partida.pontuacao == 100);

    partida.pausada = true;
    quadro(&partida, 4.0f, 0);   /* pausada: o tempo nao anda */
    assert(partida.tempo == 2.0f && partida.notas_perdidas == 0);
    assert(!partida_concluida(&partida));

    partida.pausada = false;
    quadro(&partida, 4.0f, 0);   /* a nota de 3.0 passou da linha */
    assert(partida.tempo == 4.0f && partida.notas_perdidas == 1 && partida.desempenho.valor == 62);

    for (int i = 0; i < 8; i++) {
        quadro(&partida, 4.0f, 2);
    }
    assert(partida.desempenho.game_over && partida.desempenho.valor == 0);
    quadro(&partida, 9.0f, 1);   /* Game Over congela tudo */
    assert(partida.tempo == 4.0f && partida.pontuacao == 100 && partida.notas_perdidas == 1);
    assert(!partida_concluida(&partida));

    encerrar_partida(&partida);
    assert(partida.notas == NULL && partida.pontuacao == 0 && partida.desempenho.valor == 75);
    puts("OK: acerto, erro, perda, pausa e bloqueio apos Game Over.");
}

static void testar_preparo_e_reinicio(void) {
    Partida partida;
    iniciar_estrutura_partida(&partida);
    escrever_notas("2 4\n1 0\n2 0\n");
    char erro[160];
    assert(preparar_partida(&partida, caminho, erro, sizeof(erro)));
    /* Merge Sort: 1.0 vem primeiro; as notas de 2.0 mantem a ordem do arquivo. */
    assert(partida.notas->tempo_alvo == 1.0f);
    assert(partida.notas->proximo->pista == 4 && partida.notas->proximo->proximo->pista == 0);

    quadro(&partida, 1.0f, 1);
    Nota *lista_antes = partida.notas;
    int pontos_antes = partida.pontuacao;

    /* Falha no carregamento: lista e indicadores ficam como estavam. */
    assert(!preparar_partida(&partida, "/tmp/arquivo_que_nao_existe_final_act.txt", erro, sizeof(erro)));
    assert(partida.notas == lista_antes && partida.pontuacao == pontos_antes && pontos_antes == 100);

    /* Sucesso: tudo volta ao inicio, num unico ponto (reiniciar_estado_partida). */
    partida.pausada = true;
    assert(preparar_partida(&partida, caminho, erro, sizeof(erro)));
    assert(partida.tempo == 0.0f && partida.pontuacao == 0 && partida.combo == 0);
    assert(partida.maior_combo == 0 && partida.notas_acertadas == 0 && partida.notas_perdidas == 0);
    assert(!partida.pausada && partida.desempenho.valor == 75 && !partida.desempenho.game_over);
    assert(partida.pista_feedback == -1 && partida.tempo_feedback == 0.0f);
    assert(partida.notas->estado == NOTA_PENDENTE);
    encerrar_partida(&partida);
    puts("OK: preparo valida antes de trocar; reinicio centralizado.");
}

static void testar_conclusao(void) {
    Partida partida;
    iniciar_estrutura_partida(&partida);
    char erro[160];
    escrever_notas("2 4\n1 0\n2 0\n");

    /* Vitoria com acorde: D em 1.0, depois D+K juntos em 2.0. */
    assert(preparar_partida(&partida, caminho, erro, sizeof(erro)));
    quadro(&partida, 1.0f, 1);
    assert(!partida_concluida(&partida));
    quadro(&partida, 2.0f, 1 | 16);
    assert(partida_concluida(&partida));
    assert(partida.pontuacao == 330 && partida.notas_acertadas == 3 && partida.maior_combo == 3);
    assert(partida.desempenho.valor == 84);
    /* Pausada nao conclui. */
    partida.pausada = true;
    assert(!partida_concluida(&partida));

    /* Todas perdidas, mas o medidor sobrevive: tambem conclui. */
    float limite = 2.0f + LIMITE_PERDA;
    assert(preparar_partida(&partida, caminho, erro, sizeof(erro)));
    quadro(&partida, limite, 0);
    assert(partida.desempenho.valor == 67 && !partida_concluida(&partida));
    quadro(&partida, nextafterf(limite, INFINITY), 0);
    assert(partida.notas_perdidas == 3 && partida.desempenho.valor == 51);
    assert(partida_concluida(&partida));

    /* Ultima nota perdida zera o medidor: Game Over tem prioridade. */
    escrever_notas("2 0\n");
    assert(preparar_partida(&partida, caminho, erro, sizeof(erro)));
    partida.desempenho.valor = PERDA_ERRO;
    quadro(&partida, 2.23f, 0);
    assert(partida.desempenho.game_over && todas_notas_resolvidas(partida.notas));
    assert(!partida_concluida(&partida));
    encerrar_partida(&partida);
    puts("OK: vitoria, todas perdidas, acorde e prioridade do Game Over.");
}

int main(void) {
    int descritor = mkstemp(caminho);
    assert(descritor >= 0 && close(descritor) == 0);
    testar_relogio();
    testar_regras_basicas();
    testar_preparo_e_reinicio();
    testar_conclusao();
    assert(unlink(caminho) == 0);
    return 0;
}
