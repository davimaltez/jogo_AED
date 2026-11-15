#ifndef MIDIA_PARTIDA_H
#define MIDIA_PARTIDA_H

#include <stdbool.h>
#include "raylib.h"
#include "catalogo_musicas.h"

typedef enum {
    AUDIO_AUSENTE,
    AUDIO_PRONTO,
    AUDIO_TOCANDO,
    AUDIO_PAUSADO,
    AUDIO_FINALIZADO,
    AUDIO_FALHOU
} EstadoAudio;

/*
 * Audio (stream) e imagem de fundo da musica em execucao.
 * Antes eram variaveis globais no principal.c; agora ficam numa struct
 * que o main guarda e passa por ponteiro para as funcoes.
 * Os campos "carregado" dizem se ha algo para tocar ou liberar.
 */
typedef struct {
    Music musica;
    bool musica_carregada;
    EstadoAudio estado_audio;
    float duracao_audio;
    float ultimo_tempo_audio;
    Texture2D fundo;
    bool fundo_carregado;
} MidiaPartida;

/* Marca a struct como vazia (nada carregado). */
void iniciar_midia_partida(MidiaPartida *midia);

/*
 * Carrega o audio e o fundo indicados na Musica; aguarda a preparacao inicial.
 * Se um arquivo nao existir, o jogo segue sem ele e avisa no terminal.
 */
void carregar_midia_partida(MidiaPartida *midia, const Musica *musica);

/* Para o audio e libera audio e fundo, se estiverem carregados. */
void descarregar_midia_partida(MidiaPartida *midia);

/* Volta o audio para o segundo zero (usado ao jogar novamente). */
void reiniciar_audio_partida(MidiaPartida *midia);

/*
 * Deve ser chamada uma vez por quadro: pausa o audio ou alimenta o
 * stream com os proximos trechos da musica.
 */
void atualizar_audio_partida(MidiaPartida *midia, bool pausada);

/* true enquanto a musica esta tocando (nem pausada, nem terminada). */
bool audio_esta_tocando(const MidiaPartida *midia);

/* Ultima amostra do relogio, colhida por atualizar_audio_partida. */
float tempo_audio_partida(const MidiaPartida *midia);

/* Desenha o fundo cobrindo a tela, com uma camada escura por cima. */
void desenhar_fundo_musica(const MidiaPartida *midia, int largura, int altura);

#endif 
