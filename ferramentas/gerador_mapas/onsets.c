#include "gerador.h"
#include <math.h>
#include <stdlib.h>

static int comparar_float(const void *esquerda, const void *direita) {
    float primeiro = *(const float *)esquerda, segundo = *(const float *)direita;
    return (primeiro > segundo) - (primeiro < segundo);
}

bool detectar_ataques(const float *magnitude, int quadros, Ataques *ataques) {
    *ataques = (Ataques){0};
    if (!magnitude || quadros <= 0) return false;
    ataques->quadros = quadros;
    ataques->fluxo = calloc((size_t)quadros, sizeof(float));
    ataques->picos = calloc((size_t)quadros, sizeof(float));
    float *ordenados = malloc((size_t)quadros * sizeof(float));
    if (!ataques->fluxo || !ataques->picos || !ordenados) {
        free(ordenados); liberar_ataques(ataques); return false;
    }
    int positivos = 0;
    for (int quadro = 1; quadro < quadros; quadro++) {
        float fluxo = 0, energia_atual = 0, energia_anterior = 0, maior = 0;
        for (int bin = 1; bin < TOTAL_BINS; bin++) {
            float amplitude = magnitude[(size_t)quadro * TOTAL_BINS + bin];
            float passada = magnitude[(size_t)(quadro - 1) * TOTAL_BINS + bin];
            energia_atual += amplitude * amplitude;
            energia_anterior += passada * passada;
            maior = fmaxf(maior, amplitude);
            float atual = log1pf(GANHO_LOG * amplitude);
            float anterior = log1pf(GANHO_LOG * passada);
            if (atual > anterior) fluxo += atual - anterior;
        }
        /* A soltura espalha energia entre bins, mas reduz a energia total.
           Nao a confundir com um novo ataque de melodia ou percussao. */
        if (energia_atual < energia_anterior * RAZAO_ENERGIA_ATAQUE || maior < ENERGIA_MINIMA) fluxo = 0;
        ataques->fluxo[quadro] = fluxo;
        if (fluxo > 0) ordenados[positivos++] = fluxo;
    }
    /* Percentil dos valores positivos: silencio nao reduz a referencia a zero. */
    qsort(ordenados, (size_t)positivos, sizeof(float), comparar_float);
    float referencia = positivos ? ordenados[(int)((positivos - 1) * PERCENTIL_FORCA)] : 0;
    /* Em uma escala sustentada ha muitos fluxos numericos minusculos.
       O piso impede amplificar essas oscilacoes como ataques verdadeiros. */
    if (positivos) referencia = fmaxf(referencia, ordenados[positivos - 1] * PISO_REFERENCIA_FLUXO);
    if (referencia <= 0) { free(ordenados); return true; }
    for (int i = 0; i < quadros; i++) ataques->fluxo[i] /= referencia;
    int ultimo = -quadros;
    int distancia = (int)ceilf(INTERVALO_PICOS * TAXA_ANALISE / PASSO_FFT);
    for (int i = 1; i < quadros - 1; i++) {
        float soma = 0;
        int vizinhos = 0;
        for (int j = i - RAIO_LIMIAR; j <= i + RAIO_LIMIAR; j++) {
            if (j >= 0 && j < quadros && j != i) { soma += ataques->fluxo[j]; vizinhos++; }
        }
        float limiar = FATOR_LIMIAR * (vizinhos ? soma / vizinhos : 0) + MINIMO_LIMIAR;
        float pico = ataques->fluxo[i];
        if (pico <= limiar || pico < ataques->fluxo[i - 1] || pico <= ataques->fluxo[i + 1]) continue;
        /* Se os picos estao proximos, conserva o mais forte. */
        if (i - ultimo < distancia) {
            if (pico <= ataques->picos[ultimo]) continue;
            ataques->picos[ultimo] = 0;
        }
        ataques->picos[i] = pico;
        ultimo = i;
    }
    /* Forca das candidatas: percentil 90 dos picos, nao dos quadros de
       sustentacao. A curva de fluxo continua disponivel para o BPM. */
    positivos = 0;
    for (int i = 0; i < quadros; i++) if (ataques->picos[i] > 0) ordenados[positivos++] = ataques->picos[i];
    qsort(ordenados, (size_t)positivos, sizeof(float), comparar_float);
    if (positivos) {
        referencia = ordenados[(int)((positivos - 1) * PERCENTIL_FORCA)];
        for (int i = 0; i < quadros; i++) ataques->picos[i] /= referencia;
    }
    free(ordenados);
    return true;
}

void liberar_ataques(Ataques *ataques) {
    free(ataques->fluxo);
    free(ataques->picos);
    *ataques = (Ataques){0};
}

void filtrar_ataques_mistura(const Espectrograma *espectro, Ataques *harmonicos, Ataques *percussivos) {
    /* A mascara HPSS pode aumentar enquanto o som original esta acabando.
       Confere tambem a energia original para nao criar notas na soltura. */
    for (int i = 1; i < espectro->quadros; i++) {
        if (harmonicos->picos[i] <= 0 && percussivos->picos[i] <= 0) continue;
        float atual = 0, anterior = 0;
        for (int bin = 1; bin < TOTAL_BINS; bin++) {
            float amplitude = espectro->magnitude[(size_t)i * TOTAL_BINS + bin];
            float passada = espectro->magnitude[(size_t)(i - 1) * TOTAL_BINS + bin];
            atual += amplitude * amplitude;
            anterior += passada * passada;
        }
        if (atual < anterior * RAZAO_ENERGIA_ATAQUE) harmonicos->picos[i] = percussivos->picos[i] = 0;
    }
    /* Um tambor pode perturbar uma nota que ja estava soando. Se nao ha
       mudanca de altura nem reataque de energia, conserva so a percussao. */
    for (int i = QUADROS_VAZAMENTO; i + QUADROS_ATAQUE < espectro->quadros; i++) {
        if (harmonicos->picos[i] <= 0) continue;
        bool perto_percussao = false;
        for (int j = i - QUADROS_VAZAMENTO; j <= i + QUADROS_VAZAMENTO && j < espectro->quadros; j++) {
            if (percussivos->picos[j] > 0) perto_percussao = true;
        }
        if (!perto_percussao) continue;
        float energia_antes, energia_depois;
        float antes = estimar_altura(espectro->harmonico + (size_t)(i - QUADROS_VAZAMENTO) * TOTAL_BINS, &energia_antes);
        float depois = estimar_altura(espectro->harmonico + (size_t)(i + QUADROS_ATAQUE) * TOTAL_BINS, &energia_depois);
        if (antes >= 0 && depois >= 0 && fabsf(antes - depois) <= TOLERANCIA_ALTURA &&
            energia_depois <= energia_antes * RAZAO_REATAQUE) harmonicos->picos[i] = 0;
    }
}
