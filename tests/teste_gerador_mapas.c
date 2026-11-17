#define _POSIX_C_SOURCE 200809L
#include "ferramentas/gerador_mapas/gerador.h"
#include "lista_notas.h"
#include "raylib.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

static AudioMono criar_audio(float duracao) {
    AudioMono audio = {0};
    audio.quantidade = (size_t)(duracao * TAXA_ANALISE);
    audio.amostras = calloc(audio.quantidade, sizeof(float));
    assert(audio.amostras);
    return audio;
}

static void adicionar_tom(AudioMono *audio, float inicio, float duracao, int midi) {
    double frequencia = 440 * pow(2, (midi - 69) / 12.0);
    size_t primeiro = (size_t)(inicio * TAXA_ANALISE);
    for (size_t i = 0; i < (size_t)(duracao * TAXA_ANALISE) && primeiro + i < audio->quantidade; i++) {
        double tempo = (double)i / TAXA_ANALISE;
        double envelope = fmin(1, tempo / 0.01) * fmin(1, (duracao - tempo) / 0.03);
        double fase = 2 * PI_GERADOR * frequencia * tempo;
        audio->amostras[primeiro + i] += (float)(0.35 * envelope * (sin(fase) + 0.35 * sin(2 * fase) + 0.15 * sin(3 * fase)));
    }
}

static void adicionar_batida(AudioMono *audio, float inicio) {
    uint32_t ruido = 1234567;
    size_t primeiro = (size_t)(inicio * TAXA_ANALISE);
    for (size_t i = 0; i < (size_t)(0.08 * TAXA_ANALISE) && primeiro + i < audio->quantidade; i++) {
        ruido = ruido * 1664525u + 1013904223u;
        double tempo = (double)i / TAXA_ANALISE;
        float amostra = (float)(ruido >> 8) / 8388607.5f - 1;
        audio->amostras[primeiro + i] += (float)(0.6 * exp(-tempo / 0.012) * amostra);
    }
}

static void testar_fft(void) {
    float real[TAMANHO_FFT], imaginaria[TAMANHO_FFT] = {0};
    for (int i = 0; i < TAMANHO_FFT; i++) real[i] = cosf((float)(2 * PI_GERADOR * 13 * i / TAMANHO_FFT));
    calcular_fft(real, imaginaria);
    for (int i = 0; i < TAMANHO_FFT; i++) {
        float esperado = i == 13 || i == TAMANHO_FFT - 13 ? TAMANHO_FFT / 2.0f : 0;
        assert(fabsf(real[i] - esperado) < 0.01f && fabsf(imaginaria[i]) < 0.01f);
    }
}

static void medir_ataques(const char *nome, const Ataques *ataques, float inicio, float intervalo, int esperados) {
    bool encontrados[32] = {0};
    int detectados = 0, corretos = 0, falsos = 0;
    float maior_erro = 0;
    assert(esperados <= 32);
    for (int i = 0; i < ataques->quadros; i++) {
        float tempo = i * (float)PASSO_FFT / TAXA_ANALISE;
        if (tempo < IGNORAR_INICIO || ataques->picos[i] <= 0) continue;
        detectados++;
        int indice = (int)roundf((tempo - inicio) / intervalo);
        float erro = fabsf(tempo - (inicio + indice * intervalo));
        if (indice >= 0 && indice < esperados && erro <= 0.060f && !encontrados[indice]) {
            encontrados[indice] = true; corretos++;
            maior_erro = fmaxf(maior_erro, erro);
        } else {
            falsos++;
            printf("  Ataque extra em %.4f s (forca %.4f).\n", tempo, ataques->picos[i]);
        }
    }
    printf("%s: %d detectados, %d/%d correspondencias, %d falsos positivos, erro maximo %.1f ms\n",
           nome, detectados, corretos, esperados, falsos, maior_erro * 1000);
    assert(corretos >= esperados * 9 / 10 && falsos <= 2);
}

static void testar_componentes(const AudioMono *audio, bool batidas, bool melodia) {
    Espectrograma espectro = {0};
    Ataques harmonicos = {0}, percussivos = {0};
    assert(criar_espectrograma(audio, &espectro) && separar_componentes(&espectro));
    for (size_t i = 0; i < (size_t)espectro.quadros * TOTAL_BINS; i++) {
        assert(espectro.harmonico[i] >= 0 && espectro.percussivo[i] >= 0);
        assert(fabsf(espectro.magnitude[i] - espectro.harmonico[i] - espectro.percussivo[i]) < 0.00001f);
    }
    assert(detectar_ataques(espectro.harmonico, espectro.quadros, &harmonicos));
    assert(detectar_ataques(espectro.percussivo, espectro.quadros, &percussivos));
    filtrar_ataques_mistura(&espectro, &harmonicos, &percussivos);
    if (batidas) medir_ataques("Batidas", &percussivos, 2, 0.5f, 20);
    if (melodia) medir_ataques("Escala", &harmonicos, 2.25f, 1, 10);
    if (batidas && !melodia) {
        GradeBatidas grade = detectar_batidas(&harmonicos, &percussivos);
        assert(grade.periodo > 0);
        printf("BPM: %.2f (referencia 120)\n", 60 / grade.periodo);
        assert(fabsf(60 / grade.periodo - 120) < 3);
    }
    if (melodia && !batidas) {
        for (int i = 0; i < 10; i++) {
            int quadro = (int)((2.5f + i) * TAXA_ANALISE / PASSO_FFT);
            float energia;
            float altura = estimar_altura(espectro.harmonico + (size_t)quadro * TOTAL_BINS, &energia);
            assert(fabsf(altura - (60 + i)) <= 1);
        }
    }
    liberar_ataques(&harmonicos); liberar_ataques(&percussivos);
    liberar_espectrograma(&espectro);
}

static void testar_mapas(const AudioMono *audio, bool mistura) {
    Analise analise = {0};
    assert(analisar_audio(audio, &analise));
    char caminho[] = "/tmp/final_act_mapa_sintetico_XXXXXX";
    int descritor = mkstemp(caminho);
    assert(descritor >= 0 && close(descritor) == 0);
    for (int nivel = 1; nivel <= 5; nivel++) {
        Mapa mapa = {0};
        assert(selecionar_mapa(&analise, nivel, &mapa));
        assert(mapa.quantidade > 0);
        int longas = 0, falsos = 0;
        float fim[5] = {-1, -1, -1, -1, -1};
        for (int i = 0; i < mapa.quantidade; i++) {
            NotaMapa nota = mapa.notas[i];
            assert(nota.tempo >= IGNORAR_INICIO && nota.tempo + nota.duracao <= analise.duracao);
            assert(nota.pista >= 0 && nota.pista < 5 && nota.tempo > fim[nota.pista]);
            if (i) assert(nota.tempo - mapa.notas[i - 1].tempo >= ajustes_dificuldade[nivel - 1].intervalo - 0.00001f);
            fim[nota.pista] = nota.tempo + nota.duracao;
            if (nota.duracao > 0) longas++;
            float nota_esperada = 2.25f + roundf(nota.tempo - 2.25f);
            bool corresponde = nota_esperada >= 2.25f && nota_esperada <= 11.25f && fabsf(nota.tempo - nota_esperada) <= 0.06f;
            if (mistura) {
                float batida = 2 + roundf((nota.tempo - 2) / 0.5f) * 0.5f;
                corresponde |= batida >= 2 && batida <= 11.5f && fabsf(nota.tempo - batida) <= 0.06f;
            }
            if (!corresponde) falsos++;
            if (nota.duracao > 0) assert(nota.duracao >= 0.55f && nota.duracao <= 0.78f);
        }
        assert(nivel != 1 || longas == 0);
        assert(falsos == 0);
        if (!mistura && nivel > 1) assert(longas == 10);
        assert(salvar_mapa(&mapa, caminho));
        Nota *lista = NULL;
        char erro[256];
        assert(carregar_notas_arquivo(&lista, caminho, erro, sizeof(erro)));
        assert(validar_duracao_notas(lista, analise.duracao, erro, sizeof(erro)));
        printf("Nivel %d: %d notas, %d longas, %d falsos positivos; aceito pelo carregador real.\n", nivel, mapa.quantidade, longas, falsos);
        liberar_lista_notas(&lista);
        liberar_mapa(&mapa);
    }
    assert(unlink(caminho) == 0);
    liberar_analise(&analise);
}

static void testar_silencio(void) {
    AudioMono audio = criar_audio(3);
    Analise analise = {0};
    assert(analisar_audio(&audio, &analise));
    assert(analise.quantidade == 0 && analise.grade.periodo == 0);
    Mapa mapa = {0};
    assert(selecionar_mapa(&analise, 5, &mapa) && mapa.quantidade == 0);
    liberar_mapa(&mapa); liberar_analise(&analise); liberar_audio_mono(&audio);
    audio = criar_audio(0.005f);
    assert(analisar_audio(&audio, &analise) && analise.quantidade == 0);
    liberar_analise(&analise); liberar_audio_mono(&audio);
    assert(!analisar_audio(NULL, &analise));
    puts("Silencio e audio curtissimo: zero falsos positivos; entrada nula rejeitada.");
}

static void testar_wav(void) {
    char caminho[] = "/tmp/final_act_audio_XXXXXX.wav";
    /* mkstemp exige XXXXXX no fim; criamos nome unico e usamos ExportWave
       com extensao .wav em um segundo nome derivado. */
    char base[] = "/tmp/final_act_onda_XXXXXX";
    int descritor = mkstemp(base);
    assert(descritor >= 0 && close(descritor) == 0);
    snprintf(caminho, sizeof(caminho), "%s.wav", base);
    float *amostras = calloc(48000 * 2, sizeof(float));
    assert(amostras);
    for (int i = 0; i < 48000; i++) amostras[i * 2] = amostras[i * 2 + 1] = 0.25f * sinf((float)(2 * PI_GERADOR * 440 * i / 48000));
    Wave onda = {48000, 48000, 32, 2, amostras};
    assert(ExportWave(onda, caminho));
    AudioMono audio = {0};
    assert(carregar_audio_mono(caminho, &audio));
    assert(labs((long)audio.quantidade - TAXA_ANALISE) <= 1);
    float pico = 0;
    for (size_t i = 0; i < audio.quantidade; i++) pico = fmaxf(pico, fabsf(audio.amostras[i]));
    assert(fabsf(pico - 1) < 0.0001f);
    liberar_audio_mono(&audio); free(amostras);
    assert(unlink(caminho) == 0 && unlink(base) == 0);
    puts("WAV stereo 48000 Hz: conversao mono 11025 Hz e normalizacao OK.");
}

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    SetTraceLogLevel(LOG_WARNING);
    testar_fft(); testar_silencio(); testar_wav();
    AudioMono batidas = criar_audio(14), escala = criar_audio(14), mistura = criar_audio(14);
    for (int i = 0; i < 20; i++) adicionar_batida(&batidas, 2 + i * 0.5f);
    for (int i = 0; i < 10; i++) adicionar_tom(&escala, 2.25f + i, 0.7f, 60 + i);
    for (size_t i = 0; i < mistura.quantidade; i++) mistura.amostras[i] = batidas.amostras[i] + escala.amostras[i];
    normalizar_audio(&batidas); normalizar_audio(&escala); normalizar_audio(&mistura);
    testar_componentes(&batidas, true, false);
    testar_componentes(&escala, false, true);
    testar_componentes(&mistura, true, true);
    testar_mapas(&escala, false); testar_mapas(&mistura, true);
    liberar_audio_mono(&batidas); liberar_audio_mono(&escala); liberar_audio_mono(&mistura);
    puts("OK: FFT, HPSS, ataques, BPM, alturas, mapas e carregamento.");
    return 0;
}
