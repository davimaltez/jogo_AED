#define _POSIX_C_SOURCE 200809L
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
static bool clique_mouse = false;
static Vector2 posicao_mouse = { -1, -1 };
static Rectangle titulos_catalogo[MAX_MUSICAS];
static int total_titulos_catalogo = 0;
static unsigned int pistas_seguradas = 0;

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
bool __wrap_IsKeyDown(int tecla) {
    for (int pista = 0; pista < TOTAL_PISTAS; pista++) {
        if (tecla == TECLAS_PISTAS[pista]) return (pistas_seguradas & (1u << pista)) != 0;
    }
    return false;
}
bool __wrap_IsMouseButtonPressed(int botao) { return botao == MOUSE_BUTTON_LEFT && clique_mouse; }
Vector2 __wrap_GetMousePosition(void) { return posicao_mouse; }
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
    /* Titulos reais da coluna esquerda: limites do texto e alvos para cliques. */
    const char *titulos[] = {"Popular", "Dancing Queen", "City of Stars", "This Is Me",
                            "Breaking Free", "Twist and Shout", "I'm Still Standing", "Um Sonho Eu Tenho"};
    if (x < GetScreenWidth() / 2) {
        for (int i = 0; i < MAX_MUSICAS; i++) {
            if (strcmp(texto, titulos[i]) == 0 && total_titulos_catalogo < MAX_MUSICAS) {
                assert(tamanho >= 10);
                titulos_catalogo[total_titulos_catalogo++] = (Rectangle){x, y, largura_texto, tamanho};
            }
        }
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
    total_titulos_catalogo = 0;
    executar_quadro(jogo);
    total_teclas_do_quadro = 0;
    letras_do_quadro = "";
}

static void quadro(Jogo *jogo, int tecla) { quadro_com(jogo, 0.0f, tecla, KEY_NULL); }

static void terminar_preparacao(Jogo *jogo) {
    assert(jogo->partida.preparacao_restante > 0);
    quadro_com(jogo, jogo->partida.preparacao_restante, KEY_NULL, KEY_NULL);
    assert(jogo->partida.preparacao_restante == 0);
}

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
    terminar_preparacao(jogo);
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
    assert(jogo.tela == TELA_PARTIDA && jogo.partida.notas != NULL && jogo.partida.tempo == -TEMPO_ANTECEDENCIA);
    assert(jogo.mensagem_erro[0] == '\0');
    terminar_preparacao(&jogo);

    quadro_com(&jogo, 1.0f, KEY_D, KEY_NULL);
    assert(jogo.partida.pontuacao == 100 && apareceu("PONTOS: 100"));

    /* ESC pausa: o tempo nao anda. Enter em CONTINUAR volta. */
    quadro(&jogo, KEY_ESCAPE);
    assert(jogo.partida.pausada && apareceu("PAUSADO"));
    quadro_com(&jogo, 5.0f, KEY_NULL, KEY_NULL);
    assert(jogo.partida.tempo == 1.0f);
    quadro(&jogo, KEY_ENTER);
    assert(!jogo.partida.pausada);
    terminar_preparacao(&jogo);

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
    assert(jogo.tela == TELA_PARTIDA && jogo.partida.pontuacao == 0 && jogo.partida.tempo == -TEMPO_ANTECEDENCIA);
    assert(jogo.partida.desempenho.valor == 75);
    terminar_preparacao(&jogo);

    /* Game Over: o medidor e colocado no limite e a nota passa. */
    jogo.partida.desempenho.valor = PERDA_ERRO;
    quadro_com(&jogo, 10.0f, KEY_NULL, KEY_NULL);
    assert(jogo.partida.desempenho.game_over && jogo.tela == TELA_PARTIDA);
    quadro(&jogo, KEY_NULL);
    assert(apareceu("GAME OVER") && !apareceu("CONTINUAR"));
    quadro(&jogo, KEY_ENTER);   /* REINICIAR e o primeiro botao do Game Over */
    assert(!jogo.partida.desempenho.game_over && jogo.partida.tempo == -TEMPO_ANTECEDENCIA);

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

    /* VER RANKING pelo catalogo: direita leva a INICIAR, baixo leva a VER RANKING. */
    quadro(&jogo, KEY_RIGHT);
    quadro(&jogo, KEY_DOWN);
    assert(jogo.foco_menu == jogo.total_musicas + 4);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_RANKING_MUSICA && jogo.posicao_ranking == -1);
    assert(jogo.ranking.quantidade == 1 && jogo.partida.notas == NULL);
    quadro(&jogo, KEY_NULL);
    assert(apareceu("RANKING DA MUSICA") && apareceu("Lais") && apareceu("City of Stars"));
    quadro(&jogo, KEY_ENTER);   /* botao VOLTAR AO CATALOGO */
    assert(jogo.tela == TELA_SELECAO_MUSICAS);
    quadro(&jogo, KEY_NULL);
    quadro(&jogo, KEY_RIGHT);
    quadro(&jogo, KEY_DOWN);
    quadro(&jogo, KEY_ENTER);
    assert(jogo.tela == TELA_RANKING_MUSICA);
    quadro(&jogo, KEY_NULL);
    quadro(&jogo, KEY_ESCAPE);  /* ESC tambem volta ao catalogo */
    assert(jogo.tela == TELA_SELECAO_MUSICAS && jogo.foco_menu == jogo.indice_musica + 3);

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


/* Oito entradas: navegacao real, cliques e rejeicao sem alterar a partida. */
static void testar_catalogo_ampliado(bool salvar_capturas) {
    const int tamanhos[][2] = {{1280, 720}, {900, 600}, {640, 360}, {640, 480}};
    for (int t = 0; t < 4; t++) {
        SetWindowSize(tamanhos[t][0], tamanhos[t][1]);
        for (int espera = 0; espera < 5; espera++) { BeginDrawing(); EndDrawing(); }
        Jogo jogo;
        preparar_jogo_de_teste(&jogo);
        quadro(&jogo, KEY_ENTER);
        quadro(&jogo, KEY_NULL);
        assert(total_titulos_catalogo == 8);
        for (int i = 1; i < 8; i++) {
            assert(titulos_catalogo[i - 1].y + titulos_catalogo[i - 1].height < titulos_catalogo[i].y);
            quadro(&jogo, KEY_DOWN);
            assert(jogo.indice_musica == i && jogo.foco_menu == i + 3);
        }
        quadro(&jogo, KEY_RIGHT);
        assert(jogo.foco_menu == 11);
        quadro(&jogo, KEY_DOWN);
        assert(jogo.foco_menu == 12);
        quadro(&jogo, KEY_LEFT);
        assert(jogo.foco_menu == 10 && jogo.indice_musica == 7);
        for (int i = 0; i < 8; i++) {
            Rectangle titulo = titulos_catalogo[i];
            posicao_mouse = (Vector2){titulo.x + 2, titulo.y + titulo.height / 2};
            clique_mouse = true;
            quadro(&jogo, KEY_NULL);
            clique_mouse = false;
            posicao_mouse = (Vector2){-1, -1};
            assert(jogo.indice_musica == i && jogo.foco_menu == i + 3);
        }
        const char *novas[] = {"I'm Still Standing", "Um Sonho Eu Tenho"};
        for (int n = 0; n < 2; n++) {
            jogo.indice_musica = buscar_indice_por_titulo(jogo.catalogo, jogo.total_musicas, novas[n]);
            jogo.foco_menu = jogo.indice_musica + 3;
            Musica *musica = musica_atual(&jogo);
            /* Inexistente mesmo quando os mapas reais forem publicados futuramente. */
            snprintf(musica->caminho_notas, sizeof(musica->caminho_notas), "%s/ausente", caminho_notas);
            jogo.mensagem_erro[0] = '\0';
            quadro(&jogo, KEY_NULL);
            assert(apareceu("A definir") && !apareceu("Nivel 0"));
            assert(apareceu("Mapa de notas pendente.") && apareceu(musica->filme));
            if (salvar_capturas) {
                char captura[128];
                snprintf(captura, sizeof(captura), "/tmp/final_act_catalogo_%d_%dx%d.png",
                         n, tamanhos[t][0], tamanhos[t][1]);
                Image imagem = LoadImageFromScreen();
                assert(ExportImage(imagem, captura));
                UnloadImage(imagem);
            }
            jogo.partida.pontuacao = 123;
            quadro(&jogo, KEY_ENTER);
            assert(jogo.tela == TELA_SELECAO_MUSICAS && jogo.partida.notas == NULL);
            assert(jogo.partida.pontuacao == 123 && jogo.mensagem_erro[0] != '\0');
            assert(!jogo.midia.musica_carregada);
            quadro(&jogo, KEY_RIGHT);
            quadro(&jogo, KEY_ENTER);
            assert(jogo.tela == TELA_SELECAO_MUSICAS && jogo.partida.pontuacao == 123);
            for (int criterio = 0; criterio < 3; criterio++) {
                jogo.foco_menu = criterio;
                quadro(&jogo, KEY_ENTER);
                assert(strcmp(musica_atual(&jogo)->titulo, novas[n]) == 0);
            }
        }
        assert(!texto_fora_da_janela);
        liberar_jogo(&jogo);
    }
    SetWindowSize(900, 600);
    puts("OK: oito musicas, teclado, mouse, filtros, A definir e bloqueio sem mapa.");
}

/* Abre os WAV reais pelo modulo de midia, sem gerar mapas. */
static void testar_novos_wav(void) {
    InitAudioDevice();
    assert(IsAudioDeviceReady());
    Musica catalogo[MAX_MUSICAS];
    int total;
    inicializar_catalogo(catalogo, &total);
    const char *titulos[] = {"I'm Still Standing", "Um Sonho Eu Tenho"};
    for (int i = 0; i < 2; i++) {
        int indice = buscar_indice_por_titulo(catalogo, total, titulos[i]);
        assert(indice >= 0);
        MidiaPartida midia;
        iniciar_midia_partida(&midia);
        carregar_midia_partida(&midia, &catalogo[indice]);
        assert(midia.musica_carregada && midia.estado_audio == AUDIO_PRONTO);
        assert(midia.duracao_audio > 180 && midia.duracao_audio < 200);
        SetMusicVolume(midia.musica, 0);
        atualizar_audio_partida(&midia, false);
        for (int quadro_audio = 0; quadro_audio < 20; quadro_audio++) {
            WaitTime(0.01);
            atualizar_audio_partida(&midia, false);
        }
        assert(midia.estado_audio == AUDIO_TOCANDO && tempo_audio_partida(&midia) > 0);
        atualizar_audio_partida(&midia, true);
        float congelado = tempo_audio_partida(&midia);
        WaitTime(0.02);
        atualizar_audio_partida(&midia, true);
        assert(midia.estado_audio == AUDIO_PAUSADO && tempo_audio_partida(&midia) == congelado);
        atualizar_audio_partida(&midia, false);
        assert(midia.estado_audio == AUDIO_TOCANDO);
        reiniciar_audio_partida(&midia);
        assert(midia.estado_audio == AUDIO_PRONTO && tempo_audio_partida(&midia) == 0);
        printf("OK: WAV %s (%.3f s), abertura, relogio, pausa, retomada e reinicio.\n",
               titulos[i], midia.duracao_audio);
        descarregar_midia_partida(&midia);
        assert(!midia.musica_carregada && midia.estado_audio == AUDIO_AUSENTE);
    }
    CloseAudioDevice();
}

/* Desenha cada tela em varios tamanhos e confere que nenhum texto sai da janela. */
static void testar_tamanhos(bool salvar_capturas) {
    const int tamanhos[][2] = { { 1920, 1080 }, { 1280, 720 }, { 900, 600 }, { 640, 360 }, { 640, 480 } };
    const EstadoTela telas[] = { TELA_MENU_PRINCIPAL, TELA_SELECAO_MUSICAS, TELA_COMO_JOGAR,
                                 TELA_PARTIDA, TELA_REGISTRO_RANKING, TELA_RESULTADOS,
                                 TELA_RANKING_MUSICA };
    const char *nomes[] = { "menu", "selecao", "como_jogar", "partida", "registro", "resultados",
                            "ranking" };
    int total_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);

    for (int t = 0; t < total_tamanhos; t++) {
        SetWindowSize(tamanhos[t][0], tamanhos[t][1]);
        for (int espera = 0; espera < 5; espera++) { BeginDrawing(); EndDrawing(); }
        assert(GetScreenWidth() == tamanhos[t][0] && GetScreenHeight() == tamanhos[t][1]);

        for (int i = 0; i < 7; i++) {
            Jogo jogo;
            preparar_jogo_de_teste(&jogo);
            if (telas[i] != TELA_MENU_PRINCIPAL && telas[i] != TELA_SELECAO_MUSICAS && telas[i] != TELA_COMO_JOGAR) {
                assert(preparar_partida(&jogo.partida, caminho_notas, jogo.mensagem_erro, sizeof(jogo.mensagem_erro)));
                jogo.partida.tempo = 0.6f;
                jogo.partida.pontuacao = 12340;
                jogo.partida.combo = 7;
                strcpy(jogo.nome_jogador, "MARIA EDUARDA");
                assert(carregar_ranking(&jogo.ranking, caminho_ranking));
                jogo.posicao_ranking = 1;
                strcpy(jogo.aviso_ranking, "Voce ficou em 2o lugar!");
            }
            if (telas[i] == TELA_RANKING_MUSICA) {
                jogo.posicao_ranking = -1;
                jogo.aviso_ranking[0] = '\0';
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
                /* Sustains pendentes e ativos, cauda maior que a tela e contagem. */
                jogo.partida.notas->duracao = 4;
                quadro(&jogo, KEY_NULL);
                jogo.partida.notas->estado = NOTA_SUSTENTANDO;
                pistas_seguradas = 1u << jogo.partida.notas->pista;
                quadro(&jogo, KEY_NULL);
                if (tamanhos[t][0] == 900 && tamanhos[t][1] == 600) {
                    Image captura = LoadImageFromScreen();
                    assert(ExportImage(captura, "/tmp/final_act_sustentacao.png"));
                    UnloadImage(captura);
                }
                jogo.partida.preparacao_restante = 1;
                quadro(&jogo, KEY_NULL);
                jogo.partida.preparacao_restante = 0;
                pistas_seguradas = 0;
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

/* Audio gerado em /tmp: eventos conhecidos, sem depender dos mapas do catalogo. */
static void testar_faixa_completa(void) {
    char pasta[] = "/tmp/final_act_audio_XXXXXX";
    assert(mkdtemp(pasta) != NULL);
    char caminho_audio[128];
    snprintf(caminho_audio, sizeof(caminho_audio), "%s/audio.wav", pasta);
    Wave onda = { 0 };
    onda.frameCount = 4 * 44100;
    onda.sampleRate = 44100;
    onda.sampleSize = 16;
    onda.channels = 1;
    short *amostras = calloc(onda.frameCount, sizeof(short));
    assert(amostras != NULL);
    /* Pulsos de referencia a cada meio segundo; o arquivo termina em silencio. */
    for (unsigned int i = 0; i < onda.frameCount; i++) {
        if (i % 22050 < 2205 && i < 3 * 44100) {
            amostras[i] = (short)(3000 * sin(2 * 3.141592653589793 * 440 * i / 44100));
        }
    }
    onda.data = amostras;
    assert(ExportWave(onda, caminho_audio));
    free(amostras);
    const char *mapa = "0.2 0 0.8\n1 4\n3.99 2\n";
    FILE *arquivo = fopen(caminho_notas, "w");
    assert(arquivo != NULL && fputs(mapa, arquivo) >= 0 && fclose(arquivo) == 0);

    InitAudioDevice();
    assert(IsAudioDeviceReady());
    Jogo jogo;
    preparar_jogo_de_teste(&jogo);
    Musica *musica = musica_atual(&jogo);
    snprintf(musica->caminho_audio, sizeof(musica->caminho_audio), "%s", caminho_audio);
    musica->caminho_fundo[0] = '\0';
    iniciar_partida(&jogo);
    assert(jogo.tela == TELA_PARTIDA && jogo.midia.estado_audio == AUDIO_PRONTO);
    assert(fabsf(jogo.midia.duracao_audio - 4) < 0.001f);
    SetMusicVolume(jogo.midia.musica, 0); /* Teste automatico nao precisa emitir som. */
    terminar_preparacao(&jogo);
    bool iniciou = false, pausou = false, acertou_comum = false;
    double prazo = GetTime() + 12;
    while (jogo.tela == TELA_PARTIDA && GetTime() < prazo) {
        WaitTime(0.01);
        float tempo = jogo.partida.tempo;
        int tecla = KEY_NULL;
        if (!iniciou && tempo >= 0.2f) {
            tecla = KEY_D;
            pistas_seguradas = 1;
            iniciou = true;
        }
        if (!pausou && tempo >= 0.5f) {
            quadro(&jogo, KEY_ESCAPE);
            float congelado = jogo.partida.tempo;
            int pontos = jogo.partida.pontuacao;
            pistas_seguradas = 0;
            for (int i = 0; i < 5; i++) { WaitTime(0.01); quadro_com(&jogo, 1, KEY_NULL, KEY_NULL); }
            assert(jogo.partida.tempo == congelado && jogo.partida.pontuacao == pontos);
            quadro(&jogo, KEY_ESCAPE);
            pistas_seguradas = 1;
            terminar_preparacao(&jogo);
            assert(jogo.partida.tempo == congelado);
            pausou = true;
        }
        if (!acertou_comum && tempo >= 1) { tecla = KEY_K; acertou_comum = true; }
        if (tempo >= 1.1f) pistas_seguradas = 0;
        quadro_com(&jogo, 0.01f, tecla, KEY_NULL);
        if (!jogo.partida.audio_finalizado) assert(jogo.tela == TELA_PARTIDA);
    }
    assert(jogo.tela == TELA_REGISTRO_RANKING);
    assert(iniciou && pausou && acertou_comum);
    assert(jogo.midia.estado_audio == AUDIO_FINALIZADO && jogo.partida.tempo > 4.2f);
    assert(jogo.partida.sustentacoes_concluidas == 1 && jogo.partida.sustentacoes_interrompidas == 0);
    assert(jogo.partida.notas_acertadas == 2 && jogo.partida.notas_perdidas == 1);

    /* Reinicio invalido conserva a lista e os resultados anteriores. */
    Nota *lista_anterior = jogo.partida.notas;
    int pontos = jogo.partida.pontuacao;
    arquivo = fopen(caminho_notas, "w");
    assert(arquivo != NULL && fputs("3 0 2\n", arquivo) >= 0 && fclose(arquivo) == 0);
    reiniciar_partida(&jogo);
    assert(jogo.tela == TELA_REGISTRO_RANKING && jogo.partida.notas == lista_anterior);
    assert(jogo.partida.pontuacao == pontos && strstr(jogo.mensagem_erro, "ultrapassa") != NULL);
    arquivo = fopen(caminho_notas, "w");
    assert(arquivo != NULL && fputs(mapa, arquivo) >= 0 && fclose(arquivo) == 0);
    reiniciar_partida(&jogo);
    assert(jogo.midia.estado_audio == AUDIO_PRONTO && !jogo.partida.audio_finalizado);
    assert(jogo.partida.sustentacoes_concluidas == 0 && jogo.partida.pontuacao == 0);
    terminar_preparacao(&jogo);
    quadro(&jogo, KEY_NULL);
    assert(jogo.midia.estado_audio == AUDIO_TOCANDO);
    /* Uma parada externa nao pode produzir vitoria nem relogio artificial. */
    StopMusicStream(jogo.midia.musica);
    float congelado = jogo.partida.tempo;
    quadro_com(&jogo, 5, KEY_NULL, KEY_NULL);
    assert(jogo.midia.estado_audio == AUDIO_FALHOU && jogo.partida.pausada);
    assert(jogo.partida.tempo == congelado && jogo.tela == TELA_PARTIDA);
    liberar_jogo(&jogo);
    CloseAudioDevice();
    assert(unlink(caminho_audio) == 0 && rmdir(pasta) == 0);
    puts("OK: faixa WAV inteira, silencio final, sustain, pausa, janela final e falha de audio.");
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
    terminar_preparacao(&jogo);

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
    terminar_preparacao(&jogo);
    for (int i = 0; i < 5; i++) { WaitTime(0.02); quadro(&jogo, KEY_NULL); }
    assert(jogo.partida.tempo > tempo_pausa);

    /* Reiniciar: audio e notas voltam ao zero juntos. */
    aplicar_acao(&jogo, ACAO_REINICIAR);
    assert(jogo.partida.tempo == -TEMPO_ANTECEDENCIA);
    terminar_preparacao(&jogo);
    quadro(&jogo, KEY_NULL);
    assert(jogo.partida.tempo < tempo_pausa);

    aplicar_acao(&jogo, ACAO_VOLTAR_SELECAO);
    assert(!jogo.midia.musica_carregada && jogo.partida.notas == NULL);
    liberar_jogo(&jogo);
    CloseAudioDevice();
    printf("OK: audio sincronizado (tocou %.2f s, pausa congelou, reinicio voltou ao zero).\n", tempo_tocando);
}

/* Opcional: percorre o MP3 real inteiro, sem editar mapa, midia ou ranking do usuario. */
static void testar_mp3_completo(bool mapa_real) {
    InitAudioDevice();
    assert(IsAudioDeviceReady());
    Jogo jogo;
    iniciar_jogo(&jogo);
    jogo.indice_musica = buscar_indice_por_titulo(jogo.catalogo, jogo.total_musicas, "Dancing Queen");
    assert(jogo.indice_musica >= 0);
    Musica *musica = musica_atual(&jogo);
    if (!mapa_real) snprintf(musica->caminho_notas, sizeof(musica->caminho_notas), "%s", caminho_notas);
    snprintf(musica->caminho_ranking, sizeof(musica->caminho_ranking), "%s", caminho_ranking);
    FILE *arquivo = fopen(caminho_notas, "w");
    assert(arquivo != NULL && fputs("0.2 0 0.8\n", arquivo) >= 0 && fclose(arquivo) == 0);
    iniciar_partida(&jogo);
    assert(jogo.midia.musica_carregada && jogo.tela == TELA_PARTIDA);
    SetMusicVolume(jogo.midia.musica, 0);
    terminar_preparacao(&jogo);
    int total_notas = 0, total_sustains = 0;
    for (Nota *nota = jogo.partida.notas; nota != NULL; nota = nota->proximo) {
        total_notas++;
        if (nota->duracao > 0) total_sustains++;
    }
    double inicio = GetTime();
    double prazo = inicio + jogo.midia.duracao_audio + 15;
    bool acertou = false;
    float anterior = 0;
    printf("MP3 completo: %.3f segundos, volume zero.\n", jogo.midia.duracao_audio);
    while (jogo.tela == TELA_PARTIDA && GetTime() < prazo) {
        WaitTime(0.01);
        int tecla = KEY_NULL, segunda_tecla = KEY_NULL;
        if (mapa_real) {
            pistas_seguradas = 0;
            for (const Nota *nota = jogo.partida.notas; nota != NULL; nota = nota->proximo) {
                if (nota->estado == NOTA_SUSTENTANDO) pistas_seguradas |= 1u << nota->pista;
                if (nota->estado == NOTA_PENDENTE && nota->tempo_alvo <= jogo.partida.tempo) {
                    if (tecla == KEY_NULL) tecla = TECLAS_PISTAS[nota->pista];
                    else { assert(segunda_tecla == KEY_NULL); segunda_tecla = TECLAS_PISTAS[nota->pista]; }
                    pistas_seguradas |= 1u << nota->pista;
                }
            }
        } else if (!acertou && jogo.partida.tempo >= 0.2f) {
            tecla = KEY_D;
            pistas_seguradas = 1;
            acertou = true;
        }
        quadro_com(&jogo, 0.01f, tecla, segunda_tecla);
        assert(jogo.partida.tempo >= anterior);
        anterior = jogo.partida.tempo;
        if (!jogo.partida.audio_finalizado) assert(jogo.tela == TELA_PARTIDA);
    }
    assert(jogo.tela == TELA_REGISTRO_RANKING && jogo.midia.estado_audio == AUDIO_FINALIZADO);
    assert(jogo.partida.sustentacoes_concluidas == total_sustains);
    assert(jogo.partida.notas_acertadas == total_notas && jogo.partida.notas_perdidas == 0);
    assert(jogo.partida.sustentacoes_interrompidas == 0);
    assert(jogo.partida.tempo >= jogo.midia.duracao_audio);
    assert(GetTime() - inicio >= jogo.midia.duracao_audio - 0.5);
    printf("OK: MP3 inteiro, %d notas, %d sustains e vitoria apos %.3f segundos de audio.\n",
           total_notas, total_sustains, jogo.midia.duracao_audio);
    pistas_seguradas = 0;
    liberar_jogo(&jogo);
    CloseAudioDevice();
}

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    SetConfigFlags(0);
    InitWindow(900, 600, "Teste FINAL ACT");
    SetTraceLogLevel(LOG_WARNING);
    criar_temporario(caminho_notas, "2 4\n1 0\n2 0\n");
    criar_temporario(caminho_ranking, "");

    if (argc > 1 && strcmp(argv[1], "catalogo") == 0) {
        testar_catalogo_ampliado(true);
    } else if (argc > 1 && (strcmp(argv[1], "audio-completo") == 0 || strcmp(argv[1], "mapa-completo") == 0)) {
        testar_mp3_completo(strcmp(argv[1], "mapa-completo") == 0);
    } else {
        testar_fluxo_completo();
        testar_ranking_existente();
        testar_catalogo_ampliado(false);
        testar_novos_wav();
        testar_tamanhos(argc > 1 && strcmp(argv[1], "capturas") == 0);
        testar_audio_sincronizado();
        testar_faixa_completa();
    }

    assert(unlink(caminho_notas) == 0 && unlink(caminho_ranking) == 0);
    CloseWindow();
    return 0;
}
