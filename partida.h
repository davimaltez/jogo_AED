#ifndef PARTIDA_H
#define PARTIDA_H

#include <stdbool.h>
#include <stddef.h>
#include "lista_notas.h"
#include "desempenho.h"

#define JANELA_ACERTO 0.20f     
#define LIMITE_PERDA 0.22f      
#define DURACAO_FEEDBACK 0.30f  
#define TEMPO_ANTECEDENCIA 1.9f
#define TEMPO_RETOMADA 1.0f
#define PONTOS_SUSTENTACAO_SEGUNDO 50


typedef struct {
    Nota *notas;              
    float tempo;              
    int pontuacao;
    int combo;
    int maior_combo;
    int notas_acertadas;
    int notas_perdidas;
    int sustentacoes_concluidas;
    int sustentacoes_interrompidas;
    bool usa_audio;
    bool audio_finalizado;
    float duracao_audio;
    float preparacao_restante;
    bool preparando_inicio;
    Desempenho desempenho;    
    bool pausada;
    float tempo_feedback;     
    int pista_feedback;       
} Partida;


void iniciar_estrutura_partida(Partida *partida);


void reiniciar_estado_partida(Partida *partida);


bool preparar_partida(Partida *partida, const char *caminho_notas,
                      char *mensagem_erro, size_t tamanho_mensagem);


void encerrar_partida(Partida *partida);

bool partida_bloqueada(const Partida *partida);


float calcular_tempo_partida(float tempo_anterior, float tempo_quadro, bool bloqueada,
                             bool possui_audio, float tempo_musica,
                             bool audio_finalizado, float duracao_audio);


void atualizar_partida(Partida *partida, float novo_tempo,
                       const bool teclas_pressionadas[TOTAL_PISTAS],
                       const bool teclas_seguradas[TOTAL_PISTAS]);

/* A preparacao nao pontua. Na retomada, o jogador pode reposicionar as teclas. */
void preparar_inicio_partida(Partida *partida);
void continuar_partida(Partida *partida);
bool atualizar_preparacao(Partida *partida, float tempo_quadro);

bool partida_concluida(const Partida *partida);

#endif 
