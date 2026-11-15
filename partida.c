#include "partida.h"
#include <math.h>
#include <limits.h>

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
    partida->sustentacoes_concluidas = 0;
    partida->sustentacoes_interrompidas = 0;
    partida->usa_audio = false;
    partida->audio_finalizado = false;
    partida->duracao_audio = 0.0f;
    partida->preparacao_restante = 0.0f;
    partida->preparando_inicio = false;
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
    return partida->pausada || partida->desempenho.game_over || partida->preparacao_restante > 0.0f;
}

void preparar_inicio_partida(Partida *partida) {
    partida->preparacao_restante = TEMPO_ANTECEDENCIA;
    partida->preparando_inicio = true;
    partida->tempo = -TEMPO_ANTECEDENCIA;
}

void continuar_partida(Partida *partida) {
    if (!partida->pausada || partida->desempenho.game_over) return;
    partida->pausada = false;
    if (!partida->preparando_inicio) partida->preparacao_restante = TEMPO_RETOMADA;
}

bool atualizar_preparacao(Partida *partida, float tempo_quadro) {
    if (partida->preparacao_restante <= 0.0f) return false;
    if (partida->pausada || partida->desempenho.game_over) return true;
    if (isfinite(tempo_quadro) && tempo_quadro > 0.0f) {
        partida->preparacao_restante = fmaxf(0.0f, partida->preparacao_restante - tempo_quadro);
    }
    if (partida->preparando_inicio) partida->tempo = -partida->preparacao_restante;
    if (partida->preparacao_restante == 0.0f) partida->preparando_inicio = false;
    return true; /* O audio so comeca no proximo quadro, sem consumir a sobra. */
}

float calcular_tempo_partida(float tempo_anterior, float tempo_quadro, bool bloqueada,
                             bool possui_audio, float tempo_musica,
                             bool audio_finalizado, float duracao_audio) {
    if (bloqueada) {
        return tempo_anterior;
    }
    if (possui_audio && !audio_finalizado) {
        
        if (tempo_musica > tempo_anterior) {
            return tempo_musica;
        }
        return tempo_anterior;
    }
    if (possui_audio && tempo_anterior < duracao_audio) return duracao_audio;
    /* Depois do fim natural, apenas termina as janelas de julgamento pendentes. */
    float incremento = isfinite(tempo_quadro) && tempo_quadro > 0.0f ? tempo_quadro : 0.0f;
    return tempo_anterior + incremento;
}

static void somar_pontos(Partida *partida, int pontos) {
    partida->pontuacao = pontos > INT_MAX - partida->pontuacao ? INT_MAX : partida->pontuacao + pontos;
}

/* Trata uma tecla pressionada: acerto soma pontos, tecla vazia quebra o combo. */
static void processar_tecla(Partida *partida, int pista) {
    if (verificar_acerto(partida->notas, pista, partida->tempo, JANELA_ACERTO)) {
        int pontos = partida->combo > (INT_MAX - 100) / 10 ? INT_MAX : 100 + partida->combo * 10;
        somar_pontos(partida, pontos);
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

static void processar_sustentacoes(Partida *partida, const bool seguradas[TOTAL_PISTAS]) {
    for (Nota *nota = partida->notas; nota != NULL; nota = nota->proximo) {
        if (partida->desempenho.game_over) return;
        if (nota->estado != NOTA_SUSTENTANDO) continue;
        float fim = nota->tempo_alvo + nota->duracao;
        if (!seguradas[nota->pista] && partida->tempo < fim) {
            nota->estado = NOTA_INTERROMPIDA;
            partida->sustentacoes_interrompidas++;
            partida->combo = 0;
            registrar_desempenho(&partida->desempenho, false);
            continue;
        }
        /* Total desde o acerto, limitado ao fim: nao arredonda a cada quadro. */
        float ate = fminf(partida->tempo, fim);
        /* Ao observar soltura, nao atribui retroativamente o intervalo desconhecido. */
        if (seguradas[nota->pista]) {
            double total = fmax(0.0, (double)ate - nota->inicio_sustentacao) * PONTOS_SUSTENTACAO_SEGUNDO;
            int pontos = total >= INT_MAX ? INT_MAX : (int)(total + 0.00001);
            somar_pontos(partida, pontos - nota->pontos_sustentacao);
            nota->pontos_sustentacao = pontos;
        }
        if (partida->tempo >= fim) {
            nota->estado = NOTA_ACERTADA;
            partida->sustentacoes_concluidas++;
        }
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
                       const bool teclas_pressionadas[TOTAL_PISTAS],
                       const bool teclas_seguradas[TOTAL_PISTAS]) {
    if (partida_bloqueada(partida) || !isfinite(novo_tempo) || novo_tempo < partida->tempo) {
        return;
    }

    float tempo_passado = novo_tempo - partida->tempo;
    partida->tempo = novo_tempo;
    if (partida->tempo_feedback > 0.0f) {
        partida->tempo_feedback -= tempo_passado;
    }

    processar_sustentacoes(partida, teclas_seguradas);

    for (int pista = 0; pista < TOTAL_PISTAS; pista++) {
        /* Depois do Game Over, as teclas seguintes do mesmo quadro sao ignoradas. */
        if (teclas_pressionadas[pista] && !partida->desempenho.game_over) {
            processar_tecla(partida, pista);
        }
    }

    if (!partida->desempenho.game_over) {
        /* Inclui sustains cujo inicio acabou de ser acertado neste quadro. */
        processar_sustentacoes(partida, teclas_seguradas);
        if (!partida->desempenho.game_over) processar_perdas(partida);
    }
}

bool partida_concluida(const Partida *partida) {
    return !partida_bloqueada(partida) && (!partida->usa_audio || partida->audio_finalizado) &&
           todas_notas_resolvidas(partida->notas);
}
