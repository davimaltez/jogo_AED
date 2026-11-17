#include "gerador.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

int main(int quantidade, char **argumentos) {
    if (quantidade != 3) {
        fprintf(stderr, "Uso: %s musica.mp3 pasta_nova_de_saida\nGera notas_1.txt ate notas_5.txt; aceita MP3 e WAV.\n", argumentos[0]);
        return EXIT_FAILURE;
    }
    SetTraceLogLevel(LOG_WARNING);
    AudioMono audio = {0};
    Analise analise = {0};
    Mapa mapas[5] = {{0}};
    int resultado = EXIT_FAILURE;
    bool pasta_criada = false;
    size_t tamanho = strlen(argumentos[2]) + 32;
    char *caminho = malloc(tamanho);
    if (!caminho) goto finalizar;
    struct stat estado;
    if (stat(argumentos[2], &estado) == 0) {
        fprintf(stderr, "Destino ja existe; escolha uma pasta nova. Nenhum arquivo foi substituido.\n");
        goto finalizar;
    }
    if (!carregar_audio_mono(argumentos[1], &audio)) {
        fprintf(stderr, "Falha ao decodificar/preparar audio.\n"); goto finalizar;
    }
    printf("Analisando %.3f s, mono a %d Hz...\n", (double)audio.quantidade / TAXA_ANALISE, TAXA_ANALISE);
    if (!analisar_audio(&audio, &analise)) {
        fprintf(stderr, "Falha na analise (entrada invalida ou memoria insuficiente).\n"); goto finalizar;
    }
    for (int i = 0; i < 5; i++) {
        if (!selecionar_mapa(&analise, i + 1, &mapas[i]) || !mapas[i].quantidade) {
            fprintf(stderr, "Nivel %d sem notas ou sem memoria; nenhum mapa publicado.\n", i + 1); goto finalizar;
        }
    }
#ifdef _WIN32
    int erro = _mkdir(argumentos[2]);
#else
    int erro = mkdir(argumentos[2], 0777);
#endif
    if (erro != 0) { fprintf(stderr, "Nao foi possivel criar destino: %s\n", strerror(errno)); goto finalizar; }
    pasta_criada = true;
    printf("BPM estimado: %.2f; confianca: %.3f; candidatas: %d\n",
           analise.grade.periodo > 0 ? 60 / analise.grade.periodo : 0, analise.grade.confianca, analise.quantidade);
    for (int i = 0; i < 5; i++) {
        snprintf(caminho, tamanho, "%s/notas_%d.txt", argumentos[2], i + 1);
        if (!salvar_mapa(&mapas[i], caminho)) { fprintf(stderr, "Falha de escrita: %s\n", caminho); goto finalizar; }
        int longas = 0;
        for (int j = 0; j < mapas[i].quantidade; j++) if (mapas[i].notas[j].duracao > 0) longas++;
        printf("Nivel %d: %d notas, %d longas -> %s\n", i + 1, mapas[i].quantidade, longas, caminho);
    }
    resultado = EXIT_SUCCESS;
finalizar:
    if (pasta_criada && resultado != EXIT_SUCCESS) {
        for (int i = 1; i <= 5; i++) {
            snprintf(caminho, tamanho, "%s/notas_%d.txt", argumentos[2], i);
            remove(caminho);
        }
#ifdef _WIN32
        _rmdir(argumentos[2]);
#else
        rmdir(argumentos[2]);
#endif
    }
    for (int i = 0; i < 5; i++) liberar_mapa(&mapas[i]);
    liberar_analise(&analise);
    liberar_audio_mono(&audio);
    free(caminho);
    return resultado;
}
