#include "midia_partida.h"
#include <math.h>

void iniciar_midia_partida(MidiaPartida *midia) {
    midia->musica_carregada = false;
    midia->fundo_carregado = false;
    midia->estado_audio = AUDIO_AUSENTE;
    midia->duracao_audio = 0.0f;
    midia->ultimo_tempo_audio = 0.0f;
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
    midia->estado_audio = AUDIO_AUSENTE;
    midia->duracao_audio = 0.0f;
    midia->ultimo_tempo_audio = 0.0f;
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

    /* Mantem o tamanho de buffer ja usado pelo projeto. */
    SetAudioStreamBufferSizeDefault(8192);
    midia->musica = LoadMusicStream(caminho_audio);
    if (!IsMusicValid(midia->musica)) {
        TraceLog(LOG_WARNING, "Nao foi possivel abrir o audio: %s", caminho_audio);
        midia->estado_audio = AUDIO_FALHOU;
        return;
    }
    midia->musica.looping = false;
    midia->musica_carregada = true;
    midia->duracao_audio = GetMusicTimeLength(midia->musica);
    midia->estado_audio = isfinite(midia->duracao_audio) && midia->duracao_audio > 0.0f
                         ? AUDIO_PRONTO : AUDIO_FALHOU;
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
    /* A reproducao so comeca depois da validacao e da contagem inicial. */
    carregar_audio(midia, musica->caminho_audio);
}

void reiniciar_audio_partida(MidiaPartida *midia) {
    if (midia->musica_carregada) {
        StopMusicStream(midia->musica);
        midia->ultimo_tempo_audio = 0.0f;
        midia->estado_audio = AUDIO_PRONTO;
    }
}

void atualizar_audio_partida(MidiaPartida *midia, bool pausada) {
    if (!midia->musica_carregada || midia->estado_audio == AUDIO_FINALIZADO ||
        midia->estado_audio == AUDIO_FALHOU) {
        return;
    }
    if (pausada) {
        if (midia->estado_audio == AUDIO_TOCANDO) {
            PauseMusicStream(midia->musica);
            midia->estado_audio = AUDIO_PAUSADO;
        }
        return;
    }
    if (midia->estado_audio == AUDIO_PRONTO) {
        PlayMusicStream(midia->musica);
        midia->estado_audio = AUDIO_TOCANDO;
    } else if (midia->estado_audio == AUDIO_PAUSADO) {
        ResumeMusicStream(midia->musica);
        midia->estado_audio = AUDIO_TOCANDO;
    }
    /* Parada fora de UpdateMusicStream nao e tratada como fim natural. */
    if (!IsMusicStreamPlaying(midia->musica)) {
        midia->estado_audio = AUDIO_FALHOU;
        return;
    }
    UpdateMusicStream(midia->musica);
    if (!IsMusicStreamPlaying(midia->musica)) {
        /* O stream sem looping foi drenado; StopMusicStream pode zerar o cursor. */
        midia->estado_audio = AUDIO_FINALIZADO;
        midia->ultimo_tempo_audio = midia->duracao_audio;
        return;
    }
    float tempo = GetMusicTimePlayed(midia->musica);
    if (isfinite(tempo) && tempo > midia->ultimo_tempo_audio) {
        midia->ultimo_tempo_audio = fminf(tempo, midia->duracao_audio);
    }
}

bool audio_esta_tocando(const MidiaPartida *midia) {
    return midia->musica_carregada && midia->estado_audio == AUDIO_TOCANDO &&
           IsMusicStreamPlaying(midia->musica);
}

float tempo_audio_partida(const MidiaPartida *midia) {
    if (!midia->musica_carregada) {
        return 0.0f;
    }
    return midia->ultimo_tempo_audio;
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
