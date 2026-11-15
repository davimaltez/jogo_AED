#include "partida.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void quadro(Partida *partida, float tempo, unsigned int apertadas, unsigned int seguradas) {
    bool novas[TOTAL_PISTAS], mantidas[TOTAL_PISTAS];
    for (int pista = 0; pista < TOTAL_PISTAS; pista++) {
        novas[pista] = (apertadas & (1u << pista)) != 0;
        mantidas[pista] = (seguradas & (1u << pista)) != 0;
    }
    atualizar_partida(partida, tempo, novas, mantidas);
}

static void preparar(Partida *partida, float inicio, float duracao) {
    iniciar_estrutura_partida(partida);
    partida->notas = criar_nota(inicio, 0);
    assert(partida->notas != NULL);
    partida->notas->duracao = duracao;
}

static void testar_pontos_por_tempo(void) {
    const int quadros[] = { 1, 30, 60, 144, 271 };
    for (unsigned int i = 0; i < sizeof(quadros) / sizeof(quadros[0]); i++) {
        Partida partida;
        preparar(&partida, 2, 3);
        quadro(&partida, 2, 1, 1);
        assert(partida.notas->estado == NOTA_SUSTENTANDO && !partida_concluida(&partida));
        for (int j = 1; j <= quadros[i]; j++) {
            quadro(&partida, 2.0f + 3.0f * j / quadros[i], 0, 1);
        }
        assert(partida.pontuacao == 250 && partida.combo == 1);
        assert(partida.notas_acertadas == 1 && partida.notas_perdidas == 0);
        assert(partida.sustentacoes_concluidas == 1 && partida_concluida(&partida));
        quadro(&partida, 8, 0, 1);
        assert(partida.pontuacao == 250 && partida.sustentacoes_concluidas == 1);
        encerrar_partida(&partida);
    }
    Partida partida;
    preparar(&partida, 2, 3);
    quadro(&partida, 1.875f, 1, 1); /* Antecipar nao estende a duracao pontuavel. */
    quadro(&partida, 5, 0, 1);
    assert(partida.pontuacao == 250);
    encerrar_partida(&partida);
    preparar(&partida, 2, 3);
    quadro(&partida, 2.125f, 1, 1); /* Atrasar perde a fracao nao sustentada. */
    quadro(&partida, 5, 0, 1);
    assert(partida.pontuacao == 243);
    encerrar_partida(&partida);
}

static void testar_interrupcoes(void) {
    Partida partida;
    preparar(&partida, 2, 3);
    quadro(&partida, 2, 1, 1);
    quadro(&partida, 3, 0, 1);
    quadro(&partida, 3.1f, 0, 0);
    assert(partida.notas->estado == NOTA_INTERROMPIDA && partida.combo == 0);
    assert(partida.pontuacao == 150 && partida.desempenho.valor == 70);
    assert(partida.sustentacoes_interrompidas == 1 && partida.notas_perdidas == 0);
    quadro(&partida, 5, 0, 1);
    assert(partida.pontuacao == 150 && partida.desempenho.valor == 70);
    assert(partida.sustentacoes_concluidas == 0);
    encerrar_partida(&partida);

    preparar(&partida, 2, 3);
    quadro(&partida, 2.23f, 0, 1); /* Segurar sem acertar a cabeca nao inicia. */
    assert(partida.notas->estado == NOTA_PERDIDA && partida.notas_perdidas == 1);
    assert(partida.pontuacao == 0 && partida.desempenho.valor == 67);
    quadro(&partida, 6, 0, 0);
    assert(partida.desempenho.valor == 67);
    encerrar_partida(&partida);

    preparar(&partida, 2, 3);
    quadro(&partida, 2, 1, 1);
    partida.desempenho.valor = PERDA_ERRO;
    quadro(&partida, 3, 0, 0);
    assert(partida.desempenho.game_over && !partida_concluida(&partida));
    quadro(&partida, 9, 1, 1);
    assert(partida.pontuacao == 100 && partida.tempo == 3);
    encerrar_partida(&partida);
}

static void testar_acordes_e_limites(void) {
    Partida partida;
    preparar(&partida, 2, 2);
    inserir_nota(&partida.notas, 2, 1);
    inserir_nota(&partida.notas, 2, 4);
    partida.notas->proximo->proximo->duracao = 1;
    quadro(&partida, 2, 1 | 2 | 16, 1 | 2 | 16);
    assert(partida.pontuacao == 330 && partida.notas_acertadas == 3);
    quadro(&partida, 3, 0, 1 | 16);
    assert(partida.pontuacao == 430 && partida.sustentacoes_concluidas == 1);
    quadro(&partida, 4, 0, 1);
    assert(partida.pontuacao == 480 && partida.sustentacoes_concluidas == 2);
    assert(partida_concluida(&partida));
    encerrar_partida(&partida);

    preparar(&partida, 2, 1);
    quadro(&partida, 2, 1, 1);
    quadro(&partida, nextafterf(3.0f, 0.0f), 0, 0);
    assert(partida.sustentacoes_interrompidas == 1);
    encerrar_partida(&partida);
    preparar(&partida, 2, 1);
    quadro(&partida, 2, 1, 1);
    quadro(&partida, 3, 0, 0); /* Soltar no fim e valido, mas nao inventa pontos no intervalo. */
    assert(partida.sustentacoes_concluidas == 1 && partida.sustentacoes_interrompidas == 0);
    encerrar_partida(&partida);

    preparar(&partida, 2, 1);
    inserir_nota(&partida.notas, 3, 0);
    quadro(&partida, 2, 1, 1);
    quadro(&partida, 2.9f, 1, 1); /* A proxima cabeca nao pode ser acertada dentro do sustain. */
    assert(partida.notas->proximo->estado == NOTA_PENDENTE);
    quadro(&partida, 3, 1, 1);
    assert(partida.sustentacoes_concluidas == 1 && partida.notas_acertadas == 2);
    encerrar_partida(&partida);
}

static void testar_preparacao_e_pausa(void) {
    Partida partida;
    preparar(&partida, 0, 3);
    preparar_inicio_partida(&partida);
    assert(partida.tempo == -TEMPO_ANTECEDENCIA);
    quadro(&partida, 0, 1, 1);
    assert(partida.pontuacao == 0);
    assert(atualizar_preparacao(&partida, 10));
    assert(partida.tempo == 0 && !partida_bloqueada(&partida));
    quadro(&partida, 0, 1, 1);
    quadro(&partida, 1, 0, 1);
    partida.pausada = true;
    quadro(&partida, 10, 0, 0);
    assert(partida.pontuacao == 150 && partida.tempo == 1);
    continuar_partida(&partida);
    assert(partida_bloqueada(&partida));
    quadro(&partida, 10, 0, 0);
    assert(atualizar_preparacao(&partida, TEMPO_RETOMADA));
    assert(partida.tempo == 1 && partida.pontuacao == 150);
    quadro(&partida, 3, 0, 1);
    assert(partida.pontuacao == 250 && partida.sustentacoes_concluidas == 1);
    encerrar_partida(&partida);
    assert(partida.sustentacoes_concluidas == 0 && partida.preparacao_restante == 0);
}

static void testar_fim_da_musica(void) {
    Partida partida;
    preparar(&partida, 1, 0);
    partida.usa_audio = true;
    partida.duracao_audio = 10;
    quadro(&partida, 1, 1, 1);
    assert(todas_notas_resolvidas(partida.notas) && !partida_concluida(&partida));
    quadro(&partida, 9.9f, 0, 0);
    assert(!partida_concluida(&partida));
    partida.audio_finalizado = true;
    quadro(&partida, 10, 0, 0);
    assert(partida_concluida(&partida));
    partida.pausada = true;
    assert(!partida_concluida(&partida));
    encerrar_partida(&partida);

    preparar(&partida, 10, 0);
    partida.usa_audio = true;
    partida.audio_finalizado = true;
    quadro(&partida, 10, 0, 0);
    assert(!partida_concluida(&partida));
    quadro(&partida, 10.22f, 0, 0);
    assert(!partida_concluida(&partida));
    quadro(&partida, nextafterf(10.22f, INFINITY), 0, 0);
    assert(partida_concluida(&partida));
    encerrar_partida(&partida);

    preparar(&partida, 8, 2);
    partida.usa_audio = true;
    quadro(&partida, 8, 1, 1);
    quadro(&partida, 10, 0, 1);
    assert(partida.sustentacoes_concluidas == 1 && !partida_concluida(&partida));
    partida.audio_finalizado = true;
    assert(partida_concluida(&partida));
    encerrar_partida(&partida);
}

int main(void) {
    testar_pontos_por_tempo();
    testar_interrupcoes();
    testar_acordes_e_limites();
    testar_preparacao_e_pausa();
    testar_fim_da_musica();
    puts("OK: sustains, acordes, FPS, pausa, preparacao e fim integral da musica.");
    return 0;
}
