#ifndef CONFIGURACAO_GERADOR_H
#define CONFIGURACAO_GERADOR_H

/* Todos os ajustes musicais ficam aqui. Recompile depois de alterar. */
#define TAXA_ANALISE 11025
#define TAMANHO_FFT 512
#define PASSO_FFT 128
#define TOTAL_BINS (TAMANHO_FFT / 2 + 1)
#define PI_GERADOR 3.14159265358979323846
#define DURACAO_MAXIMA 900.0f
#define MEDIANA_TEMPO 17
#define MEDIANA_FREQUENCIA 17
#define GANHO_LOG 100.0f
#define RAIO_LIMIAR 12
#define FATOR_LIMIAR 1.15f
#define MINIMO_LIMIAR 0.20f
#define INTERVALO_PICOS 0.09f
#define INTERVALO_FUSAO 0.05f
#define RAZAO_ENERGIA_ATAQUE 1.00f
#define QUADROS_VAZAMENTO 4
#define RAZAO_REATAQUE 1.30f
#define PERCENTIL_FORCA 0.90f
#define PISO_REFERENCIA_FLUXO 0.10f
#define BPM_MINIMO 60.0f
#define BPM_MAXIMO 200.0f
#define BPM_PREFERIDO_MINIMO 90.0f
#define BPM_PREFERIDO_MAXIMO 160.0f
#define BONUS_BPM_PREFERIDO 1.05f
#define CONFIANCA_BATIDA 0.20f
#define MINIMO_ATAQUES_BATIDA 4
#define TOLERANCIA_GRADE_QUADROS 1.0f
#define MIDI_MINIMO 48
#define MIDI_MAXIMO 88
#define TOTAL_HARMONICOS 5
#define FRACAO_FUNDAMENTAL 0.20f
#define ENERGIA_MINIMA 0.002f
#define ENERGIA_RELATIVA 0.20f
#define TOLERANCIA_ALTURA 1.5f
#define QUADROS_ATAQUE 2
#define FALHAS_SUSTENTACAO 3
#define DURACAO_LONGA_MINIMA 0.40f
#define DURACAO_LONGA_MAXIMA 4.0f
#define FORCA_ENGOLIDA 0.80f
#define IGNORAR_INICIO 1.50f
#define FOLGA_PISTA 0.08f
#define MAXIMO_REPETICOES 2
#define LACUNA_BATIDAS 2.0f
#define FORCA_PREENCHIMENTO 0.25f
#define ENERGIA_PREENCHIMENTO 0.015f
#define AJUSTES_DIFICULDADE { \
    {0.35f, 0.48f, 0}, {0.25f, 0.34f, 1}, {0.15f, 0.25f, 1}, \
    {0.10f, 0.18f, 1}, {0.08f, 0.12f, 1} \
}

#if TAMANHO_FFT < 2 || (TAMANHO_FFT & (TAMANHO_FFT - 1)) != 0
#error TAMANHO_FFT deve ser uma potencia de dois
#endif
#if MEDIANA_TEMPO % 2 == 0 || MEDIANA_FREQUENCIA % 2 == 0
#error As janelas de mediana devem ser impares
#endif

typedef struct {
    float corte;
    float intervalo;
    int usar_longas;
} AjusteDificuldade;

extern const AjusteDificuldade ajustes_dificuldade[5];
#endif
