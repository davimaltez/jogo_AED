#ifndef PARTIDA_H
#define PARTIDA_H

#include <stdbool.h>
#include <stddef.h>
#include "lista_notas.h"
#include "desempenho.h"

#define JANELA_ACERTO 0.20f     
#define LIMITE_PERDA 0.22f      
#define DURACAO_FEEDBACK 0.30f  


typedef struct {
    Nota *notas;              
    float tempo;              
    int pontuacao;
    int combo;
    int maior_combo;
    int notas_acertadas;
    int notas_perdidas;
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
                             bool musica_tocando, float tempo_musica);


void atualizar_partida(Partida *partida, float novo_tempo,
                       const bool teclas_pressionadas[TOTAL_PISTAS]);

bool partida_concluida(const Partida *partida);

#endif 
