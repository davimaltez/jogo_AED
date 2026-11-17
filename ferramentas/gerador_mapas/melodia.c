#include "gerador.h"
#include <math.h>

static float magnitude_frequencia(const float *quadro, float frequencia) {
    float posicao = frequencia * TAMANHO_FFT / TAXA_ANALISE;
    int esquerda = (int)posicao;
    if (esquerda < 0 || esquerda + 1 >= TOTAL_BINS) return 0;
    float fracao = posicao - esquerda;
    return quadro[esquerda] * (1 - fracao) + quadro[esquerda + 1] * fracao;
}

float estimar_altura(const float *quadro, float *energia) {
    float maior = 0, melhor = 0, altura = -1;
    for (int i = 1; i < TOTAL_BINS; i++) if (quadro[i] > maior) maior = quadro[i];
    *energia = maior;
    if (maior < ENERGIA_MINIMA) return -1;
    /* Soma harmonicos com pesos 1/h. Exigir fundamental presente evita
       escolher suboitavas so porque seus multiplos contem uma nota forte.
       Ainda e uma estimativa: acordes e voz podem confundir o detector. */
    for (int midi = MIDI_MINIMO; midi <= MIDI_MAXIMO; midi++) {
        float frequencia = 440.0f * powf(2, (midi - 69) / 12.0f);
        float fundamental = magnitude_frequencia(quadro, frequencia);
        if (fundamental < FRACAO_FUNDAMENTAL * maior) continue;
        float soma = 0;
        for (int h = 1; h <= TOTAL_HARMONICOS; h++) soma += magnitude_frequencia(quadro, frequencia * h) / h;
        if (soma > melhor) { melhor = soma; altura = (float)midi; }
    }
    return altura;
}

float medir_sustentacao(const Espectrograma *espectro, int quadro, float *altura) {
    int inicio = quadro + QUADROS_ATAQUE;
    if (inicio >= espectro->quadros) { *altura = -1; return 0; }
    float energia_inicial;
    *altura = estimar_altura(espectro->harmonico + (size_t)inicio * TOTAL_BINS, &energia_inicial);
    if (*altura < 0) return 0;
    int ultimo = inicio, falhas = 0;
    for (int i = inicio + 1; i < espectro->quadros; i++) {
        if ((i - quadro) * (float)PASSO_FFT / TAXA_ANALISE > DURACAO_LONGA_MAXIMA) break;
        float energia;
        float altura_atual = estimar_altura(espectro->harmonico + (size_t)i * TOTAL_BINS, &energia);
        if (altura_atual < 0 || fabsf(altura_atual - *altura) > TOLERANCIA_ALTURA ||
            energia < energia_inicial * ENERGIA_RELATIVA) {
            if (++falhas >= FALHAS_SUSTENTACAO) break;
        } else { ultimo = i; falhas = 0; }
    }
    float duracao = (ultimo - quadro) * (float)PASSO_FFT / TAXA_ANALISE;
    return duracao >= DURACAO_LONGA_MINIMA ? duracao : 0;
}
