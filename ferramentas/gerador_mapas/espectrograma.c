#include "gerador.h"
#include <math.h>
#include <stdlib.h>

bool criar_espectrograma(const AudioMono *audio, Espectrograma *espectro) {
    *espectro = (Espectrograma){0};
    if (!audio || !audio->amostras || !audio->quantidade ||
        audio->quantidade > (size_t)(DURACAO_MAXIMA * TAXA_ANALISE)) return false;
    for (size_t i = 0; i < audio->quantidade; i++) if (!isfinite(audio->amostras[i])) return false;
    espectro->quadros = (int)((audio->quantidade + PASSO_FFT - 1) / PASSO_FFT);
    espectro->duracao = (float)audio->quantidade / TAXA_ANALISE;
    espectro->magnitude = calloc((size_t)espectro->quadros * TOTAL_BINS, sizeof(float));
    if (!espectro->magnitude) return false;
    float janela[TAMANHO_FFT];
    for (int i = 0; i < TAMANHO_FFT; i++) janela[i] = (float)(0.5 - 0.5 * cos(2 * PI_GERADOR * i / (TAMANHO_FFT - 1)));
    for (int quadro = 0; quadro < espectro->quadros; quadro++) {
        float real[TAMANHO_FFT] = {0}, imaginaria[TAMANHO_FFT] = {0};
        /* Janela centrada: o tempo deste quadro e quadro * hop / taxa.
           Zeros nas bordas evitam deslocar todos os ataques em meia janela. */
        for (int i = 0; i < TAMANHO_FFT; i++) {
            long indice = (long)quadro * PASSO_FFT + i - TAMANHO_FFT / 2;
            if (indice >= 0 && (size_t)indice < audio->quantidade) real[i] = audio->amostras[indice] * janela[i];
        }
        calcular_fft(real, imaginaria);
        for (int bin = 0; bin < TOTAL_BINS; bin++) {
            espectro->magnitude[(size_t)quadro * TOTAL_BINS + bin] = hypotf(real[bin], imaginaria[bin]) / TAMANHO_FFT;
        }
    }
    return true;
}

static float mediana(float *valores, int quantidade) {
    /* Insercao e suficiente para uma janela pequena de 17 valores. */
    for (int i = 1; i < quantidade; i++) {
        float valor = valores[i];
        int j = i;
        while (j > 0 && valores[j - 1] > valor) { valores[j] = valores[j - 1]; j--; }
        valores[j] = valor;
    }
    return valores[quantidade / 2];
}

bool separar_componentes(Espectrograma *espectro) {
    size_t quantidade = (size_t)espectro->quadros * TOTAL_BINS;
    espectro->harmonico = calloc(quantidade, sizeof(float));
    espectro->percussivo = calloc(quantidade, sizeof(float));
    if (!espectro->harmonico || !espectro->percussivo) return false;
    for (int quadro = 0; quadro < espectro->quadros; quadro++) {
        for (int bin = 0; bin < TOTAL_BINS; bin++) {
            float tempo[MEDIANA_TEMPO], frequencia[MEDIANA_FREQUENCIA];
            for (int i = 0; i < MEDIANA_TEMPO; i++) {
                int vizinho = quadro + i - MEDIANA_TEMPO / 2;
                if (vizinho < 0) vizinho = 0;
                if (vizinho >= espectro->quadros) vizinho = espectro->quadros - 1;
                tempo[i] = espectro->magnitude[(size_t)vizinho * TOTAL_BINS + bin];
            }
            for (int i = 0; i < MEDIANA_FREQUENCIA; i++) {
                int vizinho = bin + i - MEDIANA_FREQUENCIA / 2;
                if (vizinho < 0) vizinho = 0;
                if (vizinho >= TOTAL_BINS) vizinho = TOTAL_BINS - 1;
                frequencia[i] = espectro->magnitude[(size_t)quadro * TOTAL_BINS + vizinho];
            }
            float horizontal = mediana(tempo, MEDIANA_TEMPO);
            float vertical = mediana(frequencia, MEDIANA_FREQUENCIA);
            float soma = horizontal * horizontal + vertical * vertical;
            /* Mascara suave: valores entre zero e um, complementares. */
            float mascara = soma > 0 ? horizontal * horizontal / soma : 0;
            size_t indice = (size_t)quadro * TOTAL_BINS + bin;
            espectro->harmonico[indice] = espectro->magnitude[indice] * mascara;
            espectro->percussivo[indice] = espectro->magnitude[indice] * (1 - mascara);
        }
    }
    return true;
}

void liberar_espectrograma(Espectrograma *espectro) {
    free(espectro->magnitude);
    free(espectro->harmonico);
    free(espectro->percussivo);
    *espectro = (Espectrograma){0};
}
