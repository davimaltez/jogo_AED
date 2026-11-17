#include "gerador.h"
#include <math.h>

void calcular_fft(float real[TAMANHO_FFT], float imaginaria[TAMANHO_FFT]) {
    /* Inverte os bits dos indices para agrupar pares da FFT radix-2. */
    for (int i = 1, invertido = 0; i < TAMANHO_FFT; i++) {
        int bit = TAMANHO_FFT / 2;
        while (invertido & bit) { invertido ^= bit; bit /= 2; }
        invertido ^= bit;
        if (i < invertido) {
            float troca = real[i]; real[i] = real[invertido]; real[invertido] = troca;
            troca = imaginaria[i]; imaginaria[i] = imaginaria[invertido]; imaginaria[invertido] = troca;
        }
    }
    /* Cada etapa combina dois grupos ja transformados (borboleta). */
    for (int tamanho = 2; tamanho <= TAMANHO_FFT; tamanho *= 2) {
        float angulo = (float)(-2 * PI_GERADOR / tamanho);
        float passo_real = cosf(angulo), passo_imaginario = sinf(angulo);
        for (int inicio = 0; inicio < TAMANHO_FFT; inicio += tamanho) {
            float giro_real = 1, giro_imaginario = 0;
            for (int j = 0; j < tamanho / 2; j++) {
                int esquerda = inicio + j, direita = esquerda + tamanho / 2;
                float produto_real = giro_real * real[direita] - giro_imaginario * imaginaria[direita];
                float produto_imaginario = giro_real * imaginaria[direita] + giro_imaginario * real[direita];
                real[direita] = real[esquerda] - produto_real;
                imaginaria[direita] = imaginaria[esquerda] - produto_imaginario;
                real[esquerda] += produto_real;
                imaginaria[esquerda] += produto_imaginario;
                float anterior = giro_real;
                giro_real = anterior * passo_real - giro_imaginario * passo_imaginario;
                giro_imaginario = anterior * passo_imaginario + giro_imaginario * passo_real;
            }
        }
    }
}
