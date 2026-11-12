#include "midia_partida.h"

void iniciar_midia_partida(MidiaPartida *midia) {
    midia->musica_carregada = false;
    midia->fundo_carregado = false;
}

void descarregar_midia_partida(MidiaPartida *midia) {
    if (midia->musica_carregada) {
        StopMusicStream(midia->musica);
        UnloadMusicStream(midia->musica);
        midia->musica_carregada = false;
    }
    if (midia->fundo_carregado) {
        UnloadTexture(midia->fundo);
        midia->fundo_carregado = false;
    }
}

static void carregar_audio(MidiaPartida *midia, const char *caminho_audio) {
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_WARNING, "Dispositivo de audio indisponivel; partida sem som.");
        return;
    }
    if (!FileExists(caminho_audio)) {
        TraceLog(LOG_WARNING, "Audio nao encontrado: %s", caminho_audio);
        return;
    }

    /*
     * Buffer maior (~186 ms a 44.1 kHz) evita que o audio esvazie entre um
     * quadro e outro, o que causava chiado/estalos.
     */
    SetAudioStreamBufferSizeDefault(8192);
    midia->musica = LoadMusicStream(caminho_audio);
    if (!IsMusicValid(midia->musica)) {
        TraceLog(LOG_WARNING, "Nao foi possivel abrir o audio: %s", caminho_audio);
        return;
    }
    midia->musica.looping = false;
    midia->musica_carregada = true;
    PlayMusicStream(midia->musica);
}

static void carregar_fundo(MidiaPartida *midia, const char *caminho_fundo) {
    if (!FileExists(caminho_fundo)) {
        TraceLog(LOG_WARNING, "Fundo nao encontrado: %s", caminho_fundo);
        return;
    }
    midia->fundo = LoadTexture(caminho_fundo);
    if (IsTextureValid(midia->fundo)) {
        SetTextureFilter(midia->fundo, TEXTURE_FILTER_BILINEAR);
        midia->fundo_carregado = true;
    }
}

void carregar_midia_partida(MidiaPartida *midia, const Musica *musica) {
    /* Garante que a midia da partida anterior foi liberada antes. */
    descarregar_midia_partida(midia);
    carregar_fundo(midia, musica->caminho_fundo);
    /* O audio vem por ultimo para comecar a tocar o mais perto possivel do quadro 0. */
    carregar_audio(midia, musica->caminho_audio);
}

void reiniciar_audio_partida(MidiaPartida *midia) {
    if (midia->musica_carregada) {
        StopMusicStream(midia->musica);
        PlayMusicStream(midia->musica);
    }
}

void atualizar_audio_partida(MidiaPartida *midia, bool pausada) {
    if (!midia->musica_carregada) {
        return;
    }
    if (pausada) {
        PauseMusicStream(midia->musica);
    } else {
        ResumeMusicStream(midia->musica);
        UpdateMusicStream(midia->musica);
    }
}

bool audio_esta_tocando(const MidiaPartida *midia) {
    return midia->musica_carregada && IsMusicStreamPlaying(midia->musica);
}

float tempo_audio_partida(const MidiaPartida *midia) {
    if (!midia->musica_carregada) {
        return 0.0f;
    }
    return GetMusicTimePlayed(midia->musica);
}

void desenhar_fundo_musica(const MidiaPartida *midia, int largura, int altura) {
    if (!midia->fundo_carregado) {
        return;
    }
    Texture2D fundo = midia->fundo;
    /* Usa a maior escala para cobrir a tela inteira sem distorcer a imagem. */
    float escala_x = (float)largura / fundo.width;
    float escala_y = (float)altura / fundo.height;
    float escala = escala_x > escala_y ? escala_x : escala_y;
    float largura_desenho = fundo.width * escala;
    float altura_desenho = fundo.height * escala;
    Rectangle origem = { 0, 0, (float)fundo.width, (float)fundo.height };
    Rectangle destino = { (largura - largura_desenho) / 2.0f, (altura - altura_desenho) / 2.0f,
                          largura_desenho, altura_desenho };
    DrawTexturePro(fundo, origem, destino, (Vector2){ 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, largura, altura, Fade(BLACK, 0.55f));
}
