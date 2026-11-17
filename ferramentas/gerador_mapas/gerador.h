#ifndef GERADOR_MAPAS_H
#define GERADOR_MAPAS_H
#include <stdbool.h>
#include <stddef.h>
#include "configuracao.h"

typedef struct {
    float *amostras;
    size_t quantidade;
} AudioMono;

typedef struct {
    int quadros;
    float duracao;
    /* Matrizes contiguas: indice = quadro * TOTAL_BINS + frequencia. */
    float *magnitude;
    float *harmonico;
    float *percussivo;
} Espectrograma;

typedef struct {
    float *fluxo;
    float *picos;
    int quadros;
} Ataques;

typedef struct {
    float periodo;
    float fase;
    float confianca;
} GradeBatidas;

typedef struct {
    float tempo;
    float duracao;
    float altura;
    float forca;
    int melodica;
} Candidata;

typedef struct {
    Candidata *notas;
    int quantidade;
    int capacidade;
    float duracao;
    GradeBatidas grade;
} Analise;

typedef struct {
    float tempo;
    float duracao;
    int pista;
} NotaMapa;

typedef struct {
    NotaMapa *notas;
    int quantidade;
} Mapa;

bool carregar_audio_mono(const char *caminho, AudioMono *audio);
void liberar_audio_mono(AudioMono *audio);
void normalizar_audio(AudioMono *audio);
void calcular_fft(float real[TAMANHO_FFT], float imaginaria[TAMANHO_FFT]);
bool criar_espectrograma(const AudioMono *audio, Espectrograma *espectro);
bool separar_componentes(Espectrograma *espectro);
void liberar_espectrograma(Espectrograma *espectro);
bool detectar_ataques(const float *magnitude, int quadros, Ataques *ataques);
void liberar_ataques(Ataques *ataques);
void filtrar_ataques_mistura(const Espectrograma *espectro, Ataques *harmonicos, Ataques *percussivos);
GradeBatidas detectar_batidas(const Ataques *harmonicos, const Ataques *percussivos);
float alinhar_grade(float tempo, GradeBatidas grade);
float estimar_altura(const float *quadro, float *energia);
float medir_sustentacao(const Espectrograma *espectro, int quadro, float *altura);
bool analisar_audio(const AudioMono *audio, Analise *analise);
void liberar_analise(Analise *analise);
bool selecionar_mapa(const Analise *analise, int dificuldade, Mapa *mapa);
void liberar_mapa(Mapa *mapa);
bool salvar_mapa(const Mapa *mapa, const char *caminho);
#endif
