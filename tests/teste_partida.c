#define _POSIX_C_SOURCE 200809L
/*
 * Teste do jogo completo com uma janela oculta.
 * O jogo e conduzido por um roteiro de teclas (sem mouse), chamando
 * executar_quadro, a mesma funcao que o main usa a cada quadro.
 * As funcoes de entrada da raylib sao substituidas com --wrap.
 */
#define main main_do_jogo
#include "../principal.c"
#undef main
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ---------- Entradas controladas pelo teste ---------- */
static int teclas_do_quadro[6];
static int total_teclas_do_quadro = 0;
static const char *letras_do_quadro = "";
static float tempo_do_quadro = 0.0f;
static bool usar_tempo_real = false;

/* ---------- Textos desenhados no ultimo quadro ---------- */
static char textos_desenhados[8192];
static bool texto_fora_da_janela = false;
static char texto_que_saiu[128];

int __real_GetCharPressed(void);
float __real_GetFrameTime(void);
void __real_SetConfigFlags(unsigned int flags);
void __real_DrawText(const char *texto, int x, int y, int tamanho, Color cor);

bool __wrap_IsKeyPressed(int tecla) {
    for (int i = 0; i < total_teclas_do_quadro; i++) {
        if (teclas_do_quadro[i] == tecla) return true;
    }
    return false;
}

bool __wrap_IsKeyPressedRepeat(int tecla) { (void)tecla; return false; }
bool __wrap_IsMouseButtonPressed(int botao) { (void)botao; return false; }
Vector2 __wrap_GetMousePosition(void) { return (Vector2){ -1, -1 }; }
void __wrap_ToggleFullscreen(void) { /* mantem a janela de teste oculta */ }
void __wrap_SetConfigFlags(unsigned int flags) { __real_SetConfigFlags(flags | FLAG_WINDOW_HIDDEN); }

int __wrap_GetCharPressed(void) {
    if (*letras_do_quadro == '\0') return 0;
    return (unsigned char)*letras_do_quadro++;
}

float __wrap_GetFrameTime(void) {
    return usar_tempo_real ? __real_GetFrameTime() : tempo_do_quadro;
}

void __wrap_DrawText(const char *texto, int x, int y, int tamanho, Color cor) {
    size_t usado = strlen(textos_desenhados);
    snprintf(textos_desenhados + usado, sizeof(textos_desenhados) - usado, "%s\n", texto);
    /* Nenhum texto pode sair da janela, em nenhum tamanho. */
    int largura_texto = MeasureText(texto, tamanho);
    if (x < 0 || y < 0 || x + largura_texto > GetScreenWidth() || y + tamanho > GetScreenHeight()) {
        if (!texto_fora_da_janela) {
            snprintf(texto_que_saiu, sizeof(texto_que_saiu), "'%s' em %dx%d", texto,
                     GetScreenWidth(), GetScreenHeight());
        }
        texto_fora_da_janela = true;
    }
    __real_DrawText(texto, x, y, tamanho, cor);
}

static bool apareceu(const char *trecho) {
    return strstr(textos_desenhados, trecho) != NULL;
}

/* Roda um quadro com ate duas teclas pressionadas (KEY_NULL = nenhuma). */
static void quadro_com(Jogo *jogo, float tempo, int tecla1, int tecla2) {
    total_teclas_do_quadro = 0;
    if (tecla1 != KEY_NULL) teclas_do_quadro[total_teclas_do_quadro++] = tecla1;
    if (tecla2 != KEY_NULL) teclas_do_quadro[total_teclas_do_quadro++] = tecla2;
    tempo_do_quadro = tempo;
    textos_desenhados[0] = '\0';
    executar_quadro(jogo);
    total_teclas_do_quadro = 0;
    letras_do_quadro = "";
}

static void quadro(Jogo *jogo, int tecla) { quadro_com(jogo, 0.0f, tecla, KEY_NULL); }

/* ---------- Arquivos temporarios ---------- */
static char caminho_notas[] = "/tmp/final_act_notas_XXXXXX";
static char caminho_ranking[] = "/tmp/final_act_ranking_XXXXXX";

static void criar_temporario(char *caminho, const char *conteudo) {
    int descritor = mkstemp(caminho);
    assert(descritor >= 0 && close(descritor) == 0);
    FILE *arquivo = fopen(caminho, "w");
    assert(arquivo != NULL && fputs(conteudo, arquivo) >= 0 && fclose(arquivo) == 0);
}

/* Aponta o catalogo para os temporarios e remove o audio (tempo controlado pelo teste). */
static void preparar_jogo_de_teste(Jogo *jogo) {
    iniciar_jogo(jogo);
    for (int i = 0; i < jogo->total_musicas; i++) {
        snprintf(jogo->catalogo[i].caminho_notas, sizeof(jogo->catalogo[i].caminho_notas), "%s", caminho_notas);
        snprintf(jogo->catalogo[i].caminho_ranking, sizeof(jogo->catalogo[i].caminho_ranking), "%s", caminho_ranking);
        snprintf(jogo->catalogo[i].caminho_audio, sizeof(jogo->catalogo[i].caminho_audio), "/tmp/sem_audio.wav");
    }
}

static void ler_arquivo(const char *caminho, char *destino, size_t tamanho) {
    FILE *arquivo = fopen(caminho, "r");
    assert(arquivo != NULL);
    size_t lidos = fread(destino, 1, tamanho - 1, arquivo);
    destino[lidos] = '\0';
    fclose(arquivo);
}

/* Vence a partida atual (notas 1.0/D e 2.0/D+K) e chega ao registro do nome. */
static void vencer_partida(Jogo *jogo) {
    quadro_com(jogo, 1.0f, KEY_D, KEY_NULL);
    quadro_com(jogo, 1.0f, KEY_D, KEY_K);
    assert(jogo->tela == TELA_REGISTRO_RANKING);
    assert(jogo->partida.pontuacao == 330 && jogo->partida.maior_combo == 3);
}

static void testar_fluxo_completo(void) {
    Jogo jogo;
    preparar_jogo_de_teste(&jogo);
    char conteudo[256];

    /* Menu -> selecao */
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_SELECAO_MUSICAS);

    /* Seta para baixo seleciona a segunda musica (ordem A-Z: City of Stars). */
    quadro(&jogo, KEY_DOWN);
    assert(jogo.indice_musica == 1 && jogo.foco_menu == 4);
    assert(strcmp(jogo.catalogo[1].titulo, "City of Stars") == 0);

    /* Arquivo invalido: continua na selecao e mostra o erro. */
    snprintf(jogo.catalogo[1].caminho_notas, sizeof(jogo.catalogo[1].caminho_notas), "/dev/null");
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_SELECAO_MUSICAS && jogo.partida.notas == NULL);
    quadro(&jogo, KEY_NULL);
    assert(apareceu("Arquivo sem notas"));

    /* Arquivo valido: entra na partida. */
    snprintf(jogo.catalogo[1].caminho_notas, sizeof(jogo.catalogo[1].caminho_notas), "%s", caminho_notas);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_PARTIDA && jogo.partida.notas != NULL && jogo.partida.tempo == 0.0f);
    assert(jogo.mensagem_erro[0] == '\0');

    quadro_com(&jogo, 1.0f, KEY_D, KEY_NULL);
    assert(jogo.partida.pontuacao == 100 && apareceu("PONTOS: 100"));

    /* ESC pausa: o tempo nao anda. Enter em CONTINUAR volta. */
    quadro(&jogo, KEY_ESCAPE);
    assert(jogo.partida.pausada && apareceu("PAUSADO"));
    quadro_com(&jogo, 5.0f, KEY_NULL, KEY_NULL);
    assert(jogo.partida.tempo == 1.0f);
    quadro(&jogo, KEY_ENTER);
    assert(!jogo.partida.pausada);

    /* Acorde final: vitoria vai para o registro do nome. */
    quadro_com(&jogo, 1.0f, KEY_D, KEY_K);
    assert(jogo.tela == TELA_REGISTRO_RANKING);
    letras_do_quadro = "Lais@;";   /* '@' e ';' sao recusados */
    quadro(&jogo, KEY_NULL);
    assert(strcmp(jogo.nome_jogador, "Lais") == 0 && apareceu("VITORIA!"));
    quadro(&jogo, KEY_BACKSPACE);
    assert(strcmp(jogo.nome_jogador, "Lai") == 0);
    letras_do_quadro = "s";
    quadro(&jogo, KEY_NULL);

    /* Enter salva: Merge Sort no ranking e gravacao no arquivo. */
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_RESULTADOS && jogo.posicao_ranking == 0);
    ler_arquivo(caminho_ranking, conteudo, sizeof(conteudo));
    assert(strcmp(conteudo, "330;Lais\n") == 0);
    quadro(&jogo, KEY_NULL);
    assert(apareceu("VITORIA") && apareceu("PONTUACAO: 330") && apareceu("Lais") && apareceu("1o lugar"));

    /* JOGAR NOVAMENTE (foco 0) reinicia tudo pelo ponto central. */
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_PARTIDA && jogo.partida.pontuacao == 0 && jogo.partida.tempo == 0.0f);
    assert(jogo.partida.desempenho.valor == 75);

    /* Game Over: o medidor e colocado no limite e a nota passa. */
    jogo.partida.desempenho.valor = PERDA_ERRO;
    quadro_com(&jogo, 10.0f, KEY_NULL, KEY_NULL);
    assert(jogo.partida.desempenho.game_over && jogo.tela == TELA_PARTIDA);
    quadro(&jogo, KEY_NULL);
    assert(apareceu("GAME OVER") && !apareceu("CONTINUAR"));
    quadro(&jogo, KEY_ENTER);   /* REINICIAR e o primeiro botao do Game Over */
    assert(!jogo.partida.desempenho.game_over && jogo.partida.tempo == 0.0f);

    /* Pausa -> VOLTAR A SELECAO libera a lista. */
    quadro(&jogo, KEY_ESCAPE);
    quadro(&jogo, KEY_DOWN);
    quadro(&jogo, KEY_DOWN);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_SELECAO_MUSICAS && jogo.partida.notas == NULL);

    /* Nova vitoria, mas ESC pula o registro: o arquivo nao muda. */
    quadro(&jogo, KEY_NULL);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_PARTIDA);
    vencer_partida(&jogo);
    char antes[256];
    ler_arquivo(caminho_ranking, antes, sizeof(antes));
    quadro(&jogo, KEY_ESCAPE);
    assert(jogo.tela == TELA_RESULTADOS && jogo.posicao_ranking == -1);
    ler_arquivo(caminho_ranking, conteudo, sizeof(conteudo));
    assert(strcmp(antes, conteudo) == 0);
    quadro(&jogo, KEY_NULL);
    assert(apareceu("Pontuacao nao registrada.") && apareceu("Lais"));
    quadro(&jogo, KEY_ESCAPE);
    assert(jogo.tela == TELA_SELECAO_MUSICAS && jogo.partida.notas == NULL);

    /* Filtro Dif. Decr. (Merge Sort do catalogo) mantendo a musica selecionada. */
    for (int i = 0; i < 2; i++) quadro(&jogo, KEY_UP);     /* foco 4 -> 2 */
    assert(jogo.foco_menu == 2);
    char titulo[64];
    strcpy(titulo, jogo.catalogo[jogo.indice_musica].titulo);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.criterio == ORDEM_DIFICULDADE_DECRESCENTE);
    assert(strcmp(jogo.catalogo[jogo.indice_musica].titulo, titulo) == 0);
    for (int i = 0; i + 1 < jogo.total_musicas; i++) {
        assert(jogo.catalogo[i].dificuldade >= jogo.catalogo[i + 1].dificuldade);
    }

    /* Saida: ESC volta ao menu, ESC abre a confirmacao (comeca em NAO). */
    quadro(&jogo, KEY_ESCAPE);
    assert(jogo.tela == TELA_MENU_PRINCIPAL);
    quadro(&jogo, KEY_ESCAPE);
    assert(jogo.confirmar_saida && jogo.foco_menu == 1 && apareceu("Deseja sair do jogo?"));
    quadro(&jogo, KEY_ENTER);
    assert(!jogo.confirmar_saida && !jogo.deve_sair && jogo.tela == TELA_MENU_PRINCIPAL);
    quadro(&jogo, KEY_ESCAPE);
    quadro(&jogo, KEY_LEFT);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.deve_sair);

    liberar_jogo(&jogo);
    puts("OK: menu, selecao, erro, partida, pausa, vitoria, nome, ranking, reinicio, Game Over, filtros e saida.");
}

/* Ranking com pontuacoes ja existentes: a nova entra na posicao certa. */
static void testar_ranking_existente(void) {
    FILE *arquivo = fopen(caminho_ranking, "w");
    assert(arquivo != NULL);
    fputs("120;Caio\n500;Bia\n", arquivo);   /* fora de ordem de proposito */
    assert(fclose(arquivo) == 0);

    Jogo jogo;
    preparar_jogo_de_teste(&jogo);
    quadro(&jogo, KEY_ENTER);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_PARTIDA);
    vencer_partida(&jogo);
    letras_do_quadro = "Lais";
    quadro(&jogo, KEY_NULL);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.posicao_ranking == 1 && strstr(jogo.aviso_ranking, "2o lugar") != NULL);
    char conteudo[256];
    ler_arquivo(caminho_ranking, conteudo, sizeof(conteudo));
    assert(strcmp(conteudo, "500;Bia\n330;Lais\n120;Caio\n") == 0);
    liberar_jogo(&jogo);
    puts("OK: ranking existente reordenado e gravado (500 Bia, 330 Lais, 120 Caio).");
}

/* Desenha cada tela em varios tamanhos e confere que nenhum texto sai da janela. */
static void testar_tamanhos(bool salvar_capturas) {
    const int tamanhos[][2] = { { 1920, 1080 }, { 1280, 720 }, { 900, 600 }, { 640, 360 }, { 640, 480 } };
    const EstadoTela telas[] = { TELA_MENU_PRINCIPAL, TELA_SELECAO_MUSICAS, TELA_COMO_JOGAR,
                                 TELA_PARTIDA, TELA_REGISTRO_RANKING, TELA_RESULTADOS };
    const char *nomes[] = { "menu", "selecao", "como_jogar", "partida", "registro", "resultados" };
    int total_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);

    for (int t = 0; t < total_tamanhos; t++) {
        SetWindowSize(tamanhos[t][0], tamanhos[t][1]);
        for (int espera = 0; espera < 5; espera++) { BeginDrawing(); EndDrawing(); }
        assert(GetScreenWidth() == tamanhos[t][0] && GetScreenHeight() == tamanhos[t][1]);

        for (int i = 0; i < 6; i++) {
            Jogo jogo;
            preparar_jogo_de_teste(&jogo);
            if (telas[i] == TELA_PARTIDA || telas[i] == TELA_REGISTRO_RANKING || telas[i] == TELA_RESULTADOS) {
                assert(preparar_partida(&jogo.partida, caminho_notas, jogo.mensagem_erro, sizeof(jogo.mensagem_erro)));
                jogo.partida.tempo = 0.6f;
                jogo.partida.pontuacao = 12340;
                jogo.partida.combo = 7;
                strcpy(jogo.nome_jogador, "MARIA EDUARDA");
                assert(carregar_ranking(&jogo.ranking, caminho_ranking));
                jogo.posicao_ranking = 1;
                strcpy(jogo.aviso_ranking, "Voce ficou em 2o lugar!");
            }
            jogo.tela = telas[i];
            quadro(&jogo, KEY_NULL);
            quadro(&jogo, KEY_NULL);
            if (!texto_fora_da_janela && salvar_capturas) {
                char arquivo[256];
                /* A raylib grava a captura na pasta atual. */
                snprintf(arquivo, sizeof(arquivo), "captura_%s_%dx%d.png", nomes[i],
                         tamanhos[t][0], tamanhos[t][1]);
                TakeScreenshot(arquivo);
            }
            if (telas[i] == TELA_PARTIDA) {
                /* Pausa e Game Over por cima da partida. */
                jogo.partida.desempenho.game_over = true;
                quadro(&jogo, KEY_NULL);
                jogo.partida.desempenho.game_over = false;
                jogo.partida.pausada = true;
                quadro(&jogo, KEY_NULL);
            }
            if (telas[i] == TELA_MENU_PRINCIPAL) {
                jogo.confirmar_saida = true;
                quadro(&jogo, KEY_NULL);
            }
            liberar_jogo(&jogo);
            if (texto_fora_da_janela) {
                printf("FALHA: texto fora da janela: %s (tela %s)\n", texto_que_saiu, nomes[i]);
            }
            assert(!texto_fora_da_janela);
        }
    }
    SetWindowSize(900, 600);
    puts("OK: todas as telas cabem em 1920x1080, 1280x720, 900x600, 640x360 e 640x480.");
}

/* So roda se houver dispositivo de audio e o arquivo da musica Popular. */
static void testar_audio_sincronizado(void) {
    InitAudioDevice();
    Jogo jogo;
    iniciar_jogo(&jogo);
    int indice = buscar_indice_por_titulo(jogo.catalogo, jogo.total_musicas, "Popular");
    if (!IsAudioDeviceReady() || indice < 0 || !FileExists(jogo.catalogo[indice].caminho_audio)) {
        puts("AVISO: sem dispositivo de audio ou sem popular.wav; teste de audio pulado.");
        if (IsAudioDeviceReady()) CloseAudioDevice();
        return;
    }
    snprintf(jogo.catalogo[indice].caminho_ranking, sizeof(jogo.catalogo[indice].caminho_ranking), "%s", caminho_ranking);
    /* Uma unica nota bem no futuro: nenhuma perda atrapalha a medicao. */
    FILE *arquivo = fopen(caminho_notas, "w");
    assert(arquivo != NULL && fputs("200 0\n", arquivo) >= 0 && fclose(arquivo) == 0);
    jogo.indice_musica = indice;
    aplicar_acao(&jogo, ACAO_INICIAR_PARTIDA);
    assert(jogo.tela == TELA_PARTIDA && jogo.midia.musica_carregada);

    /* O tempo do quadro fica em zero: so o audio pode fazer o tempo andar. */
    for (int i = 0; i < 15; i++) { WaitTime(0.02); quadro(&jogo, KEY_NULL); }
    float tempo_tocando = jogo.partida.tempo;
    assert(tempo_tocando > 0.05f);
    /* O audio continua tocando depois do quadro; a partida nunca fica a frente dele. */
    assert(tempo_tocando <= tempo_audio_partida(&jogo.midia) + 0.01f);

    /* Pausa: tempo da partida e do audio param juntos. */
    quadro(&jogo, KEY_ESCAPE);
    float tempo_pausa = jogo.partida.tempo;
    float audio_pausa = tempo_audio_partida(&jogo.midia);
    for (int i = 0; i < 10; i++) { WaitTime(0.02); quadro(&jogo, KEY_NULL); }
    assert(jogo.partida.tempo == tempo_pausa);
    assert(fabsf(tempo_audio_partida(&jogo.midia) - audio_pausa) < 0.01f);

    /* Continuar: volta a andar a partir do mesmo ponto. */
    quadro(&jogo, KEY_ESCAPE);
    for (int i = 0; i < 5; i++) { WaitTime(0.02); quadro(&jogo, KEY_NULL); }
    assert(jogo.partida.tempo > tempo_pausa);

    /* Reiniciar: audio e notas voltam ao zero juntos. */
    aplicar_acao(&jogo, ACAO_REINICIAR);
    assert(jogo.partida.tempo == 0.0f);
    quadro(&jogo, KEY_NULL);
    assert(jogo.partida.tempo < tempo_pausa);

    aplicar_acao(&jogo, ACAO_VOLTAR_SELECAO);
    assert(!jogo.midia.musica_carregada && jogo.partida.notas == NULL);
    liberar_jogo(&jogo);
    CloseAudioDevice();
    printf("OK: audio sincronizado (tocou %.2f s, pausa congelou, reinicio voltou ao zero).\n", tempo_tocando);
}

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    SetConfigFlags(0);
    InitWindow(900, 600, "Teste FINAL ACT");
    SetTraceLogLevel(LOG_WARNING);
    criar_temporario(caminho_notas, "2 4\n1 0\n2 0\n");
    criar_temporario(caminho_ranking, "");

    testar_fluxo_completo();
    testar_ranking_existente();
    testar_tamanhos(argc > 1 && strcmp(argv[1], "capturas") == 0);
    testar_audio_sincronizado();

    assert(unlink(caminho_notas) == 0 && unlink(caminho_ranking) == 0);
    CloseWindow();
    return 0;
}
