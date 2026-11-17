#include "gerador.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Cortes e intervalos iniciais: precisam de ajuste auditivo por repertorio. */
const AjusteDificuldade ajustes_dificuldade[5] = AJUSTES_DIFICULDADE;

static void ordenar_candidatas(Candidata *notas, Candidata *apoio, int inicio, int fim) {
    if (fim - inicio < 2) return;
    int meio = inicio + (fim - inicio) / 2;
    ordenar_candidatas(notas, apoio, inicio, meio);
    ordenar_candidatas(notas, apoio, meio, fim);
    int esquerda = inicio, direita = meio;
    for (int i = inicio; i < fim; i++) {
        if (direita == fim || (esquerda < meio && notas[esquerda].tempo <= notas[direita].tempo)) apoio[i] = notas[esquerda++];
        else apoio[i] = notas[direita++];
    }
    memcpy(notas + inicio, apoio + inicio, (size_t)(fim - inicio) * sizeof(Candidata));
}

static bool adicionar_candidata(Analise *analise, Candidata candidata) {
    if (analise->quantidade >= analise->capacidade) return false;
    analise->notas[analise->quantidade++] = candidata;
    return true;
}

static void fundir_ataques(Analise *analise) {
    /* Uma tecla de piano tem ataque percussivo e cauda harmonica. Unimos
       os dois eventos proximos, preservando altura e duracao da melodia. */
    for (int i = 0; i < analise->quantidade; i++) {
        if (analise->notas[i].melodica != 1) continue;
        int inicio = i;
        while (inicio > 0 && analise->notas[i].tempo - analise->notas[inicio - 1].tempo <= INTERVALO_FUSAO) inicio--;
        for (int j = inicio; j < analise->quantidade && analise->notas[j].tempo - analise->notas[i].tempo <= INTERVALO_FUSAO; j++) {
            if (analise->notas[j].melodica != 0) continue;
            analise->notas[i].forca = fmaxf(analise->notas[i].forca, analise->notas[j].forca);
            analise->notas[j].melodica = -2;
        }
    }
    int quantidade = 0;
    for (int i = 0; i < analise->quantidade; i++) {
        if (analise->notas[i].melodica != -2) analise->notas[quantidade++] = analise->notas[i];
    }
    analise->quantidade = quantidade;
}

bool analisar_audio(const AudioMono *audio, Analise *analise) {
    *analise = (Analise){0};
    Espectrograma espectro = {0};
    Ataques harmonicos = {0}, percussivos = {0};
    Candidata *apoio = NULL;
    bool sucesso = false;
    if (!criar_espectrograma(audio, &espectro) || !separar_componentes(&espectro)) goto finalizar;
    if (!detectar_ataques(espectro.harmonico, espectro.quadros, &harmonicos) ||
        !detectar_ataques(espectro.percussivo, espectro.quadros, &percussivos)) goto finalizar;
    filtrar_ataques_mistura(&espectro, &harmonicos, &percussivos);
    analise->duracao = espectro.duracao;
    analise->grade = detectar_batidas(&harmonicos, &percussivos);
    /* No maximo duas candidatas e uma batida por quadro. */
    analise->capacidade = espectro.quadros * 3;
    analise->notas = calloc((size_t)analise->capacidade, sizeof(Candidata));
    apoio = malloc((size_t)analise->capacidade * sizeof(Candidata));
    if (!analise->notas || !apoio) goto finalizar;
    float passo = (float)PASSO_FFT / TAXA_ANALISE;
    for (int i = 1; i < espectro.quadros; i++) {
        float tempo = i * passo;
        if (tempo < IGNORAR_INICIO) continue;
        if (harmonicos.picos[i] > 0) {
            float altura;
            float duracao = medir_sustentacao(&espectro, i, &altura);
            if (altura >= 0 && !adicionar_candidata(analise, (Candidata){tempo, duracao, altura, harmonicos.picos[i], 1})) goto finalizar;
        }
        if (percussivos.picos[i] > 0 &&
            !adicionar_candidata(analise, (Candidata){tempo, 0, -1, percussivos.picos[i], 0})) goto finalizar;
    }
    /* Preenchimentos sao marcados com melodica = -1. A selecao so os usa
       em lacunas; energia evita inventar notas nos trechos silenciosos. */
    if (analise->grade.periodo > 0) {
        for (float tempo = analise->grade.fase; tempo < espectro.duracao; tempo += analise->grade.periodo) {
            if (tempo < IGNORAR_INICIO) continue;
            int quadro = (int)roundf(tempo / passo);
            if (quadro >= espectro.quadros) break;
            float energia = 0;
            for (int bin = 1; bin < TOTAL_BINS; bin++) energia += espectro.magnitude[(size_t)quadro * TOTAL_BINS + bin];
            if (energia >= ENERGIA_PREENCHIMENTO &&
                !adicionar_candidata(analise, (Candidata){tempo, 0, -1, FORCA_PREENCHIMENTO, -1})) goto finalizar;
        }
    }
    ordenar_candidatas(analise->notas, apoio, 0, analise->quantidade);
    fundir_ataques(analise);
    sucesso = true;
finalizar:
    free(apoio);
    liberar_ataques(&harmonicos);
    liberar_ataques(&percussivos);
    liberar_espectrograma(&espectro);
    if (!sucesso) liberar_analise(analise);
    return sucesso;
}

void liberar_analise(Analise *analise) {
    free(analise->notas);
    *analise = (Analise){0};
}

static int escolher_pista(int desejada, float tempo, const float ocupadas[5], int ultima, int repeticoes) {
    /* Procura a pista livre mais proxima da altura desejada. */
    int melhor = -1, menor_custo = 100;
    for (int pista = 0; pista < 5; pista++) {
        if (tempo < ocupadas[pista] + FOLGA_PISTA) continue;
        if (pista == ultima && repeticoes >= MAXIMO_REPETICOES) continue;
        int custo = abs(pista - desejada);
        if (custo < menor_custo) { menor_custo = custo; melhor = pista; }
    }
    return melhor;
}

bool selecionar_mapa(const Analise *analise, int dificuldade, Mapa *mapa) {
    *mapa = (Mapa){0};
    if (!analise || dificuldade < 1 || dificuldade > 5 || analise->quantidade < 0) return false;
    if (!analise->quantidade) return true;
    mapa->notas = calloc((size_t)analise->quantidade, sizeof(NotaMapa));
    if (!mapa->notas) return false;
    AjusteDificuldade ajuste = ajustes_dificuldade[dificuldade - 1];
    float menor_altura = MIDI_MAXIMO, maior_altura = MIDI_MINIMO;
    for (int i = 0; i < analise->quantidade; i++) {
        if (analise->notas[i].melodica == 1) {
            menor_altura = fminf(menor_altura, analise->notas[i].altura);
            maior_altura = fmaxf(maior_altura, analise->notas[i].altura);
        }
    }
    float ocupadas[5] = {-1, -1, -1, -1, -1};
    float ultimo_tempo = -100, fim_longa = -1, forca_longa = 0;
    int ultima_pista = -1, repeticoes = 0, percussao = 0;
    for (int i = 0; i < analise->quantidade; i++) {
        Candidata candidata = analise->notas[i];
        if (candidata.melodica == -1) {
            float lacuna = LACUNA_BATIDAS * analise->grade.periodo;
            if (candidata.tempo - ultimo_tempo < lacuna) continue;
            /* Nao preencher logo antes de uma candidata real aceitavel. */
            bool proxima = false;
            for (int j = i + 1; j < analise->quantidade && analise->notas[j].tempo - candidata.tempo < ajuste.intervalo; j++) {
                if (analise->notas[j].melodica >= 0 && analise->notas[j].forca >= ajuste.corte) proxima = true;
            }
            if (proxima) continue;
        } else if (candidata.forca < ajuste.corte) continue;
        if (candidata.tempo < fim_longa && candidata.forca < forca_longa * FORCA_ENGOLIDA) continue;
        /* Em um pequeno grupo de ataques, usa o mais forte; no empate,
           prefere melodia. Isso evita duplicar o mesmo ataque nas partes. */
        if (candidata.melodica >= 0) {
            bool existe_melhor = false;
            for (int j = i + 1; j < analise->quantidade && analise->notas[j].tempo - candidata.tempo < ajuste.intervalo; j++) {
                Candidata outra = analise->notas[j];
                if (outra.melodica >= 0 && (outra.forca > candidata.forca ||
                    (outra.forca == candidata.forca && outra.melodica > candidata.melodica))) existe_melhor = true;
            }
            if (existe_melhor) continue;
        }
        float tempo = alinhar_grade(candidata.tempo, analise->grade);
        if (tempo < IGNORAR_INICIO || tempo >= analise->duracao || tempo - ultimo_tempo < ajuste.intervalo) continue;
        int desejada = percussao % 5;
        if (candidata.melodica == 1) {
            desejada = maior_altura > menor_altura ? (int)roundf(4 * (candidata.altura - menor_altura) / (maior_altura - menor_altura)) : 2;
        }
        int pista = escolher_pista(desejada, tempo, ocupadas, ultima_pista, repeticoes);
        if (pista < 0) continue;
        float duracao = ajuste.usar_longas ? candidata.duracao : 0;
        /* Preserva o fim acustico ao mover a cabeca para a grade. */
        if (duracao > 0) duracao = fminf(candidata.tempo + duracao, analise->duracao) - tempo;
        if (duracao < DURACAO_LONGA_MINIMA) duracao = 0;
        mapa->notas[mapa->quantidade++] = (NotaMapa){tempo, duracao, pista};
        ocupadas[pista] = tempo + duracao;
        if (duracao > 0) { fim_longa = tempo + duracao; forca_longa = candidata.forca; }
        repeticoes = pista == ultima_pista ? repeticoes + 1 : 1;
        ultima_pista = pista;
        ultimo_tempo = tempo;
        if (candidata.melodica != 1) percussao++;
    }
    return true;
}

void liberar_mapa(Mapa *mapa) {
    free(mapa->notas);
    *mapa = (Mapa){0};
}

bool salvar_mapa(const Mapa *mapa, const char *caminho) {
    /* O chamador cria uma pasta nova: nunca publica sobre o mapa do jogo. */
    if (!mapa || !mapa->quantidade) return false;
    FILE *arquivo = fopen(caminho, "w");
    if (!arquivo) return false;
    bool sucesso = true;
    for (int i = 0; i < mapa->quantidade; i++) {
        NotaMapa nota = mapa->notas[i];
        if (fprintf(arquivo, "%.6f %d %.6f\n", nota.tempo, nota.pista, nota.duracao) < 0) { sucesso = false; break; }
    }
    if (fclose(arquivo) != 0) sucesso = false;
    return sucesso;
}
