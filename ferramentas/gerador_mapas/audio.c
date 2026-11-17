#include "gerador.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void normalizar_audio(AudioMono *audio) {
    float pico = 0;
    for (size_t i = 0; i < audio->quantidade; i++) {
        if (fabsf(audio->amostras[i]) > pico) pico = fabsf(audio->amostras[i]);
    }
    /* Silencio continua sendo silencio; nao dividimos por zero. */
    if (pico > 0) {
        for (size_t i = 0; i < audio->quantidade; i++) audio->amostras[i] /= pico;
    }
}

bool carregar_audio_mono(const char *caminho, AudioMono *audio) {
    *audio = (AudioMono){0};
    Wave onda = LoadWave(caminho);
    if (!IsWaveValid(onda)) return false;
    double duracao = (double)onda.frameCount / onda.sampleRate;
    if (duracao <= 0 || duracao > DURACAO_MAXIMA) {
        fprintf(stderr, "Audio deve ter entre 0 e %.0f segundos.\n", DURACAO_MAXIMA);
        UnloadWave(onda);
        return false;
    }
    /* WaveFormat usa o conversor da raylib: mono, reamostragem e float.
       Nao abre janela nem dispositivo de som: esta ferramenta e offline. */
    WaveFormat(&onda, TAXA_ANALISE, 32, 1);
    if (!IsWaveValid(onda) || onda.sampleRate != TAXA_ANALISE ||
        onda.sampleSize != 32 || onda.channels != 1) {
        UnloadWave(onda);
        return false;
    }
    audio->quantidade = onda.frameCount;
    audio->amostras = malloc(audio->quantidade * sizeof(float));
    if (audio->amostras) memcpy(audio->amostras, onda.data, audio->quantidade * sizeof(float));
    UnloadWave(onda);
    if (!audio->amostras) { *audio = (AudioMono){0}; return false; }
    for (size_t i = 0; i < audio->quantidade; i++) {
        if (!isfinite(audio->amostras[i])) { liberar_audio_mono(audio); return false; }
    }
    normalizar_audio(audio);
    return true;
}

void liberar_audio_mono(AudioMono *audio) {
    free(audio->amostras);
    *audio = (AudioMono){0};
}
