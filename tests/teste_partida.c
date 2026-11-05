#define _POSIX_C_SOURCE 200809L
/* Exercita a partida real com entradas controladas e uma janela oculta. */
#define main main_do_jogo
#include "../principal.c"
#undef main
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int tecla_teste = KEY_NULL;
static bool clique_teste = false;
static float delta_teste = 0.0f;

static bool fluxo_ativo = false;
static int quadro = -1;
static int cargas = 0;
static int inicializacoes = 0;
static int avancos_tempo = 0;
static Nota **lista_fluxo = NULL;
static Nota *lista_antes_da_falha = NULL;
static Desempenho *desempenho_fluxo = NULL;
static bool erro_exibido = false;
static bool pausa_exibida = false;
static bool derrota_exibida = false;
static char pontos_exibidos[64];

typedef struct {
    float delta;
    unsigned int pistas;
    bool clicar;
    Vector2 mouse;
    bool escape;
    bool falhar_carga;
} PassoConclusao;

enum { CENARIO_VITORIA = 1, CENARIO_TODAS_PERDIDAS, CENARIO_DERROTA_FINAL, CENARIO_DERROTA_ANTECIPADA };
static int cenario_conclusao = 0;
static const PassoConclusao *roteiro_conclusao = NULL;
static int total_quadros_conclusao = 0;
static char caminho_conclusao[] = "/tmp/final_act_conclusao_XXXXXX";
static bool vitoria_exibida = false;
static bool catalogo_exibido = false;
static int pontos_resultado, acertos_resultado, perdas_resultado, combo_resultado;
static const char *captura_vitoria = NULL;
static void verificar_quadro_conclusao(void);

bool __real_carregar_notas_arquivo(Nota **cabeca, const char *caminho, char *erro, size_t tamanho);
void __real_iniciar_desempenho(Desempenho *desempenho);
void __real_SetConfigFlags(unsigned int flags);
void __real_DrawText(const char *texto, int x, int y, int tamanho, Color cor);

bool __wrap_IsKeyPressed(int tecla) {
    if (roteiro_conclusao != NULL) {
        if (tecla == KEY_ESCAPE) return roteiro_conclusao[quadro].escape;
        const int teclas[] = { KEY_D, KEY_F, KEY_G, KEY_J, KEY_K };
        for (int p = 0; p < TOTAL_PISTAS; p++) {
            if (tecla == teclas[p]) return (roteiro_conclusao[quadro].pistas & (1u << p)) != 0;
        }
        return false;
    }
    if (!fluxo_ativo) return tecla == tecla_teste;
    return (quadro == 3 && tecla == KEY_D) ||
           ((quadro == 4 || quadro == 12) && tecla == KEY_ESCAPE);
}

bool __wrap_IsMouseButtonPressed(int botao) {
    if (roteiro_conclusao != NULL) return roteiro_conclusao[quadro].clicar && botao == MOUSE_BUTTON_LEFT;
    if (!fluxo_ativo) return clique_teste && botao == MOUSE_BUTTON_LEFT;
    bool clicar = quadro == 0 || quadro == 1 || quadro == 2 || quadro == 4 ||
                  quadro == 6 || quadro == 8 || quadro == 10 || quadro == 12;
    return clicar && botao == MOUSE_BUTTON_LEFT;
}

float __wrap_GetFrameTime(void) {
    if (roteiro_conclusao != NULL) {
        avancos_tempo++;
        return roteiro_conclusao[quadro].delta;
    }
    if (!fluxo_ativo) return delta_teste;
    avancos_tempo++;
    return quadro == 3 ? 2.0f : quadro == 7 ? 100.0f : 0.0f;
}

Vector2 __wrap_GetMousePosition(void) {
    if (roteiro_conclusao != NULL) return roteiro_conclusao[quadro].mouse;
    if (quadro == 0) return (Vector2){450, 280};
    if (quadro == 1 || quadro == 2) return (Vector2){650, 390};
    if (quadro == 12) return (Vector2){450, 349};
    return (Vector2){450, 287};
}

void __wrap_SetConfigFlags(unsigned int flags) {
    __real_SetConfigFlags(flags | FLAG_WINDOW_HIDDEN);
}

void __wrap_ToggleFullscreen(void) { /* Mantem a janela de teste oculta. */ }

void __wrap_iniciar_desempenho(Desempenho *desempenho) {
    __real_iniciar_desempenho(desempenho);
    if (fluxo_ativo || roteiro_conclusao != NULL) {
        desempenho_fluxo = desempenho;
        inicializacoes++;
        /* Cria uma condicao de fronteira, sem mudar as regras do jogo. */
        if (cenario_conclusao == CENARIO_DERROTA_FINAL || cenario_conclusao == CENARIO_DERROTA_ANTECIPADA) {
            desempenho->valor = PERDA_ERRO;
        }
    }
}

bool __wrap_carregar_notas_arquivo(Nota **cabeca, const char *caminho, char *erro, size_t tamanho) {
    if (roteiro_conclusao != NULL) {
        assert(strcmp(caminho, "assets/musicas/breaking_free/notas.txt") == 0);
        cargas++;
        lista_fluxo = cabeca;
        bool falhar = roteiro_conclusao[quadro].falhar_carga;
        if (falhar) lista_antes_da_falha = *cabeca;
        bool resultado = __real_carregar_notas_arquivo(cabeca, falhar ? "/dev/null" : caminho_conclusao, erro, tamanho);
        assert(resultado != falhar);
        if (falhar) assert(*cabeca == lista_antes_da_falha);
        return resultado;
    }
    if (!fluxo_ativo) return __real_carregar_notas_arquivo(cabeca, caminho, erro, tamanho);
    cargas++;
    lista_fluxo = cabeca;
    /* O catalogo inicia por titulo: Breaking Free deve fornecer o caminho. */
    assert(strcmp(caminho, "assets/musicas/breaking_free/notas.txt") == 0);
    bool falhar = quadro == 1 || quadro == 4 || quadro == 8;
    if (falhar) lista_antes_da_falha = *cabeca;
    bool resultado = __real_carregar_notas_arquivo(cabeca, falhar ? "/dev/null" : caminho, erro, tamanho);
    assert(resultado != falhar);
    if (falhar) assert(*cabeca == lista_antes_da_falha);
    return resultado;
}

void __wrap_DrawText(const char *texto, int x, int y, int tamanho, Color cor) {
    if (fluxo_ativo || roteiro_conclusao != NULL) {
        if (strstr(texto, "Arquivo sem notas") != NULL) erro_exibido = true;
        if (strcmp(texto, "PAUSADO") == 0) pausa_exibida = true;
        if (strcmp(texto, "GAME OVER") == 0) derrota_exibida = true;
        if (strncmp(texto, "PONTOS:", 7) == 0) snprintf(pontos_exibidos, sizeof(pontos_exibidos), "%s", texto);
        if (strcmp(texto, "VITORIA") == 0) vitoria_exibida = true;
        if (strcmp(texto, "SELECAO DE MUSICAS") == 0) catalogo_exibido = true;
        sscanf(texto, "PONTUACAO: %d", &pontos_resultado);
        sscanf(texto, "ACERTOS: %d", &acertos_resultado);
        sscanf(texto, "NOTAS PERDIDAS: %d", &perdas_resultado);
        sscanf(texto, "MAIOR COMBO: %d", &combo_resultado);
    }
    __real_DrawText(texto, x, y, tamanho, cor);
}

bool __wrap_WindowShouldClose(void) {
    if (roteiro_conclusao != NULL) {
        if (quadro >= 0) verificar_quadro_conclusao();
        if (captura_vitoria != NULL && cenario_conclusao == CENARIO_VITORIA && quadro == 7) {
            TakeScreenshot(captura_vitoria);
        }
        erro_exibido = pausa_exibida = derrota_exibida = false;
        vitoria_exibida = catalogo_exibido = false;
        quadro++;
        return quadro >= total_quadros_conclusao;
    }
    /* Verifica o quadro anterior antes que as variaveis locais do main expirem. */
    if (quadro == 1) {
        assert(cargas == 1 && inicializacoes == 1 && avancos_tempo == 0);
        assert(*lista_fluxo == NULL && erro_exibido);
    } else if (quadro == 2 || quadro == 6 || quadro == 10) {
        int esperadas = quadro == 2 ? 2 : quadro == 6 ? 3 : 4;
        assert(inicializacoes == esperadas && !erro_exibido);
        assert(desempenho_fluxo->valor == 75 && !desempenho_fluxo->game_over);
        Nota *atual = *lista_fluxo;
        assert(atual != NULL && atual->tempo_alvo == 2.0f && atual->pista == 0);
        while (atual != NULL) {
            assert(atual->estado == NOTA_PENDENTE);
            if (atual->proximo != NULL) assert(atual->tempo_alvo <= atual->proximo->tempo_alvo);
            atual = atual->proximo;
        }
    } else if (quadro >= 3 && quadro <= 5) {
        assert(desempenho_fluxo->valor == 78 && inicializacoes == 2 && avancos_tempo == 1);
        assert((*lista_fluxo)->estado == NOTA_ACERTADA);
        assert(strcmp(pontos_exibidos, "PONTOS: 100") == 0);
        if (quadro >= 4) {
            assert(cargas == 3 && *lista_fluxo == lista_antes_da_falha);
            assert(erro_exibido && pausa_exibida);
        }
    } else if (quadro >= 7 && quadro <= 9) {
        assert(desempenho_fluxo->game_over && desempenho_fluxo->valor == 0);
        assert(inicializacoes == 3 && avancos_tempo == 2 && derrota_exibida);
        if (quadro >= 8) assert(cargas == 5 && *lista_fluxo == lista_antes_da_falha && erro_exibido);
    } else if (quadro == 11) {
        assert(strcmp(pontos_exibidos, "PONTOS: 0") == 0);
        assert(avancos_tempo == 3 && desempenho_fluxo->valor == 75);
    } else if (quadro == 12) {
        assert(*lista_fluxo == NULL && cargas == 6 && inicializacoes == 4);
    }
    erro_exibido = pausa_exibida = derrota_exibida = false;
    quadro++;
    return quadro >= 13;
}

static void verificar_resultados(int pontos, int acertos, int perdas, int maior_combo) {
    assert(vitoria_exibida && !derrota_exibida && !pausa_exibida);
    assert(pontos_resultado == pontos && acertos_resultado == acertos);
    assert(perdas_resultado == perdas && combo_resultado == maior_combo);
}

static void verificar_quadro_conclusao(void) {
    if (quadro < 2) return;
    assert(lista_fluxo != NULL && desempenho_fluxo != NULL);
    if (cenario_conclusao == CENARIO_VITORIA) {
        if (quadro >= 2 && quadro <= 5) {
            assert(!vitoria_exibida && !derrota_exibida && !todas_notas_resolvidas(*lista_fluxo));
        }
        if (quadro == 5) assert(strcmp(pontos_exibidos, "PONTOS: 210") == 0);
        if (quadro == 6 || quadro == 12 || quadro == 18) {
            assert(todas_notas_resolvidas(*lista_fluxo) && !desempenho_fluxo->game_over);
            assert(strcmp(pontos_exibidos, "PONTOS: 330") == 0);
        }
        if (quadro >= 7 && quadro <= 9) {
            verificar_resultados(330, 3, 0, 3);
            assert(avancos_tempo == 5 && inicializacoes == 2 && desempenho_fluxo->valor == 84);
            if (quadro >= 8) assert(erro_exibido && *lista_fluxo == lista_antes_da_falha && cargas == 2);
        }
        if (quadro == 10) {
            verificar_resultados(330, 3, 0, 3);
            assert(cargas == 3 && inicializacoes == 3 && avancos_tempo == 5);
            assert(!erro_exibido && desempenho_fluxo->valor == 75 && !todas_notas_resolvidas(*lista_fluxo));
        }
        if (quadro == 11 || quadro == 17) {
            assert(strcmp(pontos_exibidos, "PONTOS: 100") == 0 && desempenho_fluxo->valor == 78);
        }
        if (quadro == 13 || quadro == 14 || quadro == 19) verificar_resultados(330, 3, 0, 3);
        if (quadro == 14 || quadro == 15 || quadro == 20) assert(*lista_fluxo == NULL);
        if (quadro == 15 || quadro == 20) assert(catalogo_exibido);
        if (quadro == 20) assert(cargas == 4 && inicializacoes == 4 && avancos_tempo == 9);
    } else if (cenario_conclusao == CENARIO_TODAS_PERDIDAS) {
        if (quadro == 2) {
            assert(!todas_notas_resolvidas(*lista_fluxo) && !vitoria_exibida);
            assert(desempenho_fluxo->valor == 67);
        } else {
            assert(todas_notas_resolvidas(*lista_fluxo));
            assert(!desempenho_fluxo->game_over && desempenho_fluxo->valor == 51);
            assert(avancos_tempo == 2);
            if (quadro >= 4) verificar_resultados(0, 0, 3, 0);
        }
    } else {
        assert(desempenho_fluxo->game_over && desempenho_fluxo->valor == 0);
        assert(!vitoria_exibida && derrota_exibida && avancos_tempo == 1);
        assert(todas_notas_resolvidas(*lista_fluxo) == (cenario_conclusao == CENARIO_DERROTA_FINAL));
    }
}

static void executar_cenario_conclusao(int cenario, const PassoConclusao *roteiro,
                                     int quantidade, const char *eventos) {
    FILE *arquivo = fopen(caminho_conclusao, "w");
    assert(arquivo != NULL && fputs(eventos, arquivo) >= 0 && fclose(arquivo) == 0);
    cenario_conclusao = cenario;
    roteiro_conclusao = roteiro;
    total_quadros_conclusao = quantidade;
    quadro = -1;
    cargas = inicializacoes = avancos_tempo = 0;
    lista_fluxo = NULL;
    desempenho_fluxo = NULL;
    erro_exibido = pausa_exibida = derrota_exibida = false;
    vitoria_exibida = catalogo_exibido = false;
    assert(main_do_jogo() == 0);
    roteiro_conclusao = NULL;
    cenario_conclusao = 0;
}

static void testar_fluxos_conclusao(void) {
    int descritor = mkstemp(caminho_conclusao);
    assert(descritor >= 0 && close(descritor) == 0);
    const PassoConclusao vitoria[] = {
        [0] = { .clicar = true, .mouse = {450, 280} },
        [1] = { .clicar = true, .mouse = {650, 390} },
        [2] = { .delta = 0.5f },
        [3] = { .delta = 0.5f, .pistas = 1 },
        [4] = { .delta = 0.8f },
        [5] = { .delta = 0.2f, .pistas = 1 },
        [6] = { .pistas = 16 },
        [7] = { .delta = 100.0f, .pistas = 31 },
        [8] = { .clicar = true, .mouse = {450, 445}, .falhar_carga = true },
        [9] = {0},
        [10] = { .clicar = true, .mouse = {450, 445} },
        [11] = { .delta = 1.0f, .pistas = 1 },
        [12] = { .delta = 1.0f, .pistas = 17 },
        [13] = {0},
        [14] = { .clicar = true, .mouse = {450, 505} },
        [15] = {0},
        [16] = { .clicar = true, .mouse = {650, 390} },
        [17] = { .delta = 1.0f, .pistas = 1 },
        [18] = { .delta = 1.0f, .pistas = 17 },
        [19] = {0},
        [20] = { .escape = true }
    };
    executar_cenario_conclusao(CENARIO_VITORIA, vitoria, sizeof(vitoria) / sizeof(vitoria[0]), "2 4\n1 0\n2 0\n");

    float limite = 2.0f + 0.22f;
    const PassoConclusao perdidas[] = {
        [0] = { .clicar = true, .mouse = {450, 280} },
        [1] = { .clicar = true, .mouse = {650, 390} },
        [2] = { .delta = limite },
        [3] = { .delta = nextafterf(limite, INFINITY) - limite },
        [4] = {0},
        [5] = { .delta = 100.0f, .pistas = 31 }
    };
    executar_cenario_conclusao(CENARIO_TODAS_PERDIDAS, perdidas, sizeof(perdidas) / sizeof(perdidas[0]), "2 4\n1 0\n2 0\n");

    const PassoConclusao derrota[] = {
        [0] = { .clicar = true, .mouse = {450, 280} },
        [1] = { .clicar = true, .mouse = {650, 390} },
        [2] = { .delta = 2.23f },
        [3] = { .delta = 1000.0f, .pistas = 31 }
    };
    executar_cenario_conclusao(CENARIO_DERROTA_FINAL, derrota, sizeof(derrota) / sizeof(derrota[0]), "2 0\n");
    executar_cenario_conclusao(CENARIO_DERROTA_ANTECIPADA, derrota, sizeof(derrota) / sizeof(derrota[0]), "2 0\n100 4\n");
    assert(unlink(caminho_conclusao) == 0);
    puts("OK: vitoria, resultados congelados, reinicio, catalogo, simultaneas e prioridade da derrota.");
}

int main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(900, 600, "Teste de partida");
    Desempenho desempenho;
    iniciar_desempenho(&desempenho);
    Musica musica = {0};
    Nota *notas = NULL;
    inserir_nota(&notas, 2.0f, 0);
    inserir_nota(&notas, 3.0f, 4);
    float tempo = 2.0f, feedback = 0.0f;
    int pontos = 0, combo = 0, maior_combo = 0, acertos = 0, perdas = 0, pista = -1;

    BeginDrawing();
    tecla_teste = KEY_D;
    desenhar_tela_partida(900, 600, false, &desempenho, musica, &notas,
        &tempo, &pontos, &combo, &maior_combo, &acertos, &perdas, &feedback, &pista);
    assert(desempenho.valor == 78 && pontos == 100 && acertos == 1 && combo == 1);
    EndDrawing();
    if (argc > 1) TakeScreenshot(argv[1]);
    BeginDrawing();

    tecla_teste = KEY_F;
    desenhar_tela_partida(900, 600, false, &desempenho, musica, &notas,
        &tempo, &pontos, &combo, &maior_combo, &acertos, &perdas, &feedback, &pista);
    assert(desempenho.valor == 70 && combo == 0 && pontos == 100);

    delta_teste = 2.0f;
    desenhar_tela_partida(900, 600, true, &desempenho, musica, &notas,
        &tempo, &pontos, &combo, &maior_combo, &acertos, &perdas, &feedback, &pista);
    assert(tempo == 2.0f && desempenho.valor == 70 && perdas == 0);

    tecla_teste = KEY_NULL;
    desenhar_tela_partida(900, 600, false, &desempenho, musica, &notas,
        &tempo, &pontos, &combo, &maior_combo, &acertos, &perdas, &feedback, &pista);
    assert(tempo == 4.0f && desempenho.valor == 62 && perdas == 1);

    delta_teste = 0.0f;
    tecla_teste = KEY_F;
    for (int i = 0; i < 8; i++) {
        desenhar_tela_partida(900, 600, false, &desempenho, musica, &notas,
            &tempo, &pontos, &combo, &maior_combo, &acertos, &perdas, &feedback, &pista);
    }
    assert(desempenho.game_over && desempenho.valor == 0);
    delta_teste = 5.0f;
    desenhar_tela_partida(900, 600, false, &desempenho, musica, &notas,
        &tempo, &pontos, &combo, &maior_combo, &acertos, &perdas, &feedback, &pista);
    assert(tempo == 4.0f && pontos == 100 && perdas == 1);

    clique_teste = true;
    /* Continuar existe apenas na pausa. Os outros botoes mantem seus codigos. */
    assert(desenhar_menu_pausa(900, 600, (Vector2){450, 225}, false) == 1);
    assert(desenhar_menu_pausa(900, 600, (Vector2){450, 225}, true) == 0);
    assert(desenhar_menu_pausa(900, 600, (Vector2){450, 287}, true) == 2);
    assert(desenhar_menu_pausa(900, 600, (Vector2){450, 349}, true) == 3);
    assert(desenhar_menu_pausa(900, 600, (Vector2){450, 411}, true) == 4);
    desenhar_tela_partida(900, 600, false, &desempenho, musica, &notas,
        &tempo, &pontos, &combo, &maior_combo, &acertos, &perdas, &feedback, &pista);
    desenhar_menu_pausa(900, 600, (Vector2){-1, -1}, true);
    EndDrawing();
    if (argc > 2) TakeScreenshot(argv[2]);
    liberar_lista_notas(&notas);
    CloseWindow();
    puts("OK: partida real, acerto, erro, perda, pausa, bloqueio apos derrota e botoes.");
    fluxo_ativo = true;
    assert(main_do_jogo() == 0);
    puts("OK: inicio por arquivo, erro visivel, pausa e Game Over preservados, reinicio e retorno.");
    fluxo_ativo = false;
    if (argc > 3) captura_vitoria = argv[3];
    testar_fluxos_conclusao();
    return 0;
}
