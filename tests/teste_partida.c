/* Exercita a partida real com entradas controladas e uma janela oculta. */
#define main main_do_jogo
#include "../principal.c"
#undef main
#include <assert.h>

static int tecla_teste = KEY_NULL;
static bool clique_teste = false;
static float delta_teste = 0.0f;

bool __wrap_IsKeyPressed(int tecla) { return tecla == tecla_teste; }
bool __wrap_IsMouseButtonPressed(int botao) { return clique_teste && botao == MOUSE_BUTTON_LEFT; }
float __wrap_GetFrameTime(void) { return delta_teste; }

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
    return 0;
}
