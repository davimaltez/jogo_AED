#include "partida.h"

void iniciar_estrutura_partida(Partida *partida) {
    partida->notas = NULL;
    reiniciar_estado_partida(partida);
}

void reiniciar_estado_partida(Partida *partida) {
    partida->tempo = 0.0f;
    partida->pontuacao = 0;
    partida->combo = 0;
    partida->maior_combo = 0;
    partida->notas_acertadas = 0;
    partida->notas_perdidas = 0;
    partida->pausada = false;
    partida->tempo_feedback = 0.0f;
    partida->pista_feedback = -1;
    iniciar_desempenho(&partida->desempenho);
}

bool preparar_partida(Partida *partida, const char *caminho_notas,
                      char *mensagem_erro, size_t tamanho_mensagem) {
    /* O carregador so troca a lista quando o arquivo inteiro e valido. */
    if (!carregar_notas_arquivo(&partida->notas, caminho_notas,
                                mensagem_erro, tamanho_mensagem)) {
        return false;
    }
    reiniciar_estado_partida(partida);
    return true;
}

void encerrar_partida(Partida *partida) {
    liberar_lista_notas(&partida->notas);
    reiniciar_estado_partida(partida);
}

bool partida_bloqueada(const Partida *partida) {
    return partida->pausada || partida->desempenho.game_over;
}

float calcular_tempo_partida(float tempo_anterior, float tempo_quadro, bool bloqueada,
                             bool musica_tocando, float tempo_musica) {
    if (bloqueada) {
        return tempo_anterior;
    }
    if (musica_tocando) {
        
        if (tempo_musica > tempo_anterior) {
            return tempo_musica;
        }
        return tempo_anterior;
    }
    return tempo_anterior + tempo_quadro;
}

/* Trata uma tecla pressionada: acerto soma pontos, tecla vazia quebra o combo. */
static void processar_tecla(Partida *partida, int pista) {
    if (verificar_acerto(partida->notas, pista, partida->tempo, JANELA_ACERTO)) {
        partida->pontuacao += 100 + (partida->combo * 10);
        partida->combo++;
        if (partida->combo > partida->maior_combo) {
            partida->maior_combo = partida->combo;
        }
        partida->notas_acertadas++;
        registrar_desempenho(&partida->desempenho, true);
        partida->pista_feedback = pista;
        partida->tempo_feedback = DURACAO_FEEDBACK;
    } else {
        partida->combo = 0;
        registrar_desempenho(&partida->desempenho, false);
    }
}

/* Marca as notas que passaram da linha e penaliza cada uma uma unica vez. */
static void processar_perdas(Partida *partida) {
    int perdas_anteriores = partida->notas_perdidas;
    atualizar_notas(partida->notas, partida->tempo, LIMITE_PERDA,
                    &partida->combo, &partida->notas_perdidas);
    for (int i = perdas_anteriores; i < partida->notas_perdidas; i++) {
        registrar_desempenho(&partida->desempenho, false);
    }
}

void atualizar_partida(Partida *partida, float novo_tempo,
                       const bool teclas_pressionadas[TOTAL_PISTAS]) {
    if (partida_bloqueada(partida)) {
        return;
    }

    float tempo_passado = novo_tempo - partida->tempo;
    partida->tempo = novo_tempo;
    if (partida->tempo_feedback > 0.0f) {
        partida->tempo_feedback -= tempo_passado;
    }

    for (int pista = 0; pista < TOTAL_PISTAS; pista++) {
        /* Depois do Game Over, as teclas seguintes do mesmo quadro sao ignoradas. */
        if (teclas_pressionadas[pista] && !partida->desempenho.game_over) {
            processar_tecla(partida, pista);
        }
    }

    if (!partida->desempenho.game_over) {
        processar_perdas(partida);
    }
}

bool partida_concluida(const Partida *partida) {
    return !partida_bloqueada(partida) && todas_notas_resolvidas(partida->notas);
}
