#include "raylib.h"
#include <stdbool.h>

/*
 * Estados possiveis para a navegacao entre telas.
 * Cada valor representa uma tela visivel do jogo.
 */
typedef enum {
    TELA_MENU_PRINCIPAL,
    TELA_JOGAR_PROVISORIA,
    TELA_COMO_JOGAR
} EstadoTela;

/*
 * Paleta de cores inspirada na tematica de teatro musical.
 * Fundo escuro com tons de vinho, purpura e detalhes dourados.
 */
#define COR_FUNDO_ESCURO      (Color){ 18, 12, 26, 255 }
#define COR_PALCO_LATERAL     (Color){ 32, 16, 40, 255 }
#define COR_LUZ_HOLOFOTE      (Color){ 255, 220, 120, 18 }
#define COR_DOURADO           (Color){ 245, 195, 65, 255 }
#define COR_DOURADO_BRILHO    (Color){ 255, 230, 130, 255 }
#define COR_SOMBRA_TEXTO      (Color){ 10, 5, 15, 200 }

#define COR_BOTAO_FUNDO       (Color){ 45, 22, 58, 230 }
#define COR_BOTAO_BORDA       (Color){ 190, 140, 60, 255 }
#define COR_BOTAO_TEXTO       (Color){ 245, 240, 235, 255 }

#define COR_BOTAO_FOCO_FUNDO  (Color){ 85, 30, 100, 240 }
#define COR_BOTAO_FOCO_BORDA  (Color){ 255, 215, 80, 255 }
#define COR_BOTAO_FOCO_TEXTO  (Color){ 255, 245, 180, 255 }

/*
 * Desenha o fundo visual do jogo com elementos de um palco de teatro:
 * cortinas sutis nas laterais e feixes de luz cruzando a tela.
 */
void desenhar_fundo_teatro(int largura, int altura) {
    /* Preenche o fundo principal */
    ClearBackground(COR_FUNDO_ESCURO);

    /* Faixas laterais simulando cortinas de palco */
    int largura_cortina = largura * 0.08f;
    DrawRectangle(0, 0, largura_cortina, altura, COR_PALCO_LATERAL);
    DrawRectangle(largura - largura_cortina, 0, largura_cortina, altura, COR_PALCO_LATERAL);

    /* Linhas divisórias verticais douradas discretas */
    DrawLine(largura_cortina, 0, largura_cortina, altura, (Color){ 120, 90, 40, 120 });
    DrawLine(largura - largura_cortina, 0, largura - largura_cortina, altura, (Color){ 120, 90, 40, 120 });

    /* Feixes de luz suaves (holofotes) projetados do topo */
    Vector2 topo_esquerda = { largura * 0.2f, 0.0f };
    Vector2 base_esquerda_1 = { 0.0f, (float)altura };
    Vector2 base_esquerda_2 = { largura * 0.55f, (float)altura };
    DrawTriangle(topo_esquerda, base_esquerda_1, base_esquerda_2, COR_LUZ_HOLOFOTE);

    Vector2 topo_direita = { largura * 0.8f, 0.0f };
    Vector2 base_direita_1 = { largura * 0.45f, (float)altura };
    Vector2 base_direita_2 = { (float)largura, (float)altura };
    DrawTriangle(topo_direita, base_direita_1, base_direita_2, COR_LUZ_HOLOFOTE);
}

/*
 * Desenha um botao com texto centralizado e efeito visual ao passar o mouse.
 * Retorna true caso o botao tenha sido clicado.
 */
bool desenhar_botao(Rectangle retangulo, const char *texto, Vector2 mouse) {
    bool mouse_em_cima = CheckCollisionPointRec(mouse, retangulo);
    bool clicado = mouse_em_cima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color cor_fundo = mouse_em_cima ? COR_BOTAO_FOCO_FUNDO : COR_BOTAO_FUNDO;
    Color cor_borda = mouse_em_cima ? COR_BOTAO_FOCO_BORDA : COR_BOTAO_BORDA;
    Color cor_texto = mouse_em_cima ? COR_BOTAO_FOCO_TEXTO : COR_BOTAO_TEXTO;

    /* Preenchimento do botao */
    DrawRectangleRec(retangulo, cor_fundo);

    /* Borda do botao com maior destaque quando o mouse esta sobre ele */
    float espessura_borda = mouse_em_cima ? 3.0f : 2.0f;
    DrawRectangleLinesEx(retangulo, espessura_borda, cor_borda);

    /* Centralizacao do texto */
    int tamanho_fonte = 20;
    int largura_texto = MeasureText(texto, tamanho_fonte);
    int posicao_x_texto = retangulo.x + (retangulo.width - largura_texto) / 2;
    int posicao_y_texto = retangulo.y + (retangulo.height - tamanho_fonte) / 2;

    DrawText(texto, posicao_x_texto, posicao_y_texto, tamanho_fonte, cor_texto);

    return clicado;
}

/*
 * Renderiza o menu inicial com titulo em destaque e botoes de navegacao.
 */
void desenhar_menu_principal(int largura, int altura, Vector2 mouse, EstadoTela *estado, bool *deve_sair) {
    desenhar_fundo_teatro(largura, altura);

    /* Titulo principal: FINAL ACT */
    const char *titulo = "FINAL ACT";
    int tamanho_titulo = altura * 0.11f;
    if (tamanho_titulo < 36) tamanho_titulo = 36;
    int largura_titulo = MeasureText(titulo, tamanho_titulo);
    int posicao_x_titulo = (largura - largura_titulo) / 2;
    int posicao_y_titulo = altura * 0.18f;

    /* Sombra e texto do titulo em dourado */
    DrawText(titulo, posicao_x_titulo + 3, posicao_y_titulo + 3, tamanho_titulo, COR_SOMBRA_TEXTO);
    DrawText(titulo, posicao_x_titulo, posicao_y_titulo, tamanho_titulo, COR_DOURADO);

    /* Subtitulo */
    const char *subtitulo = "UM JOGO DE RITMO EM CENA";
    int tamanho_subtitulo = 16;
    int largura_subtitulo = MeasureText(subtitulo, tamanho_subtitulo);
    int posicao_x_subtitulo = (largura - largura_subtitulo) / 2;
    int posicao_y_subtitulo = posicao_y_titulo + tamanho_titulo + 10;
    DrawText(subtitulo, posicao_x_subtitulo, posicao_y_subtitulo, tamanho_subtitulo, COR_DOURADO_BRILHO);

    /* Medidas proporcionais dos botoes */
    float largura_botao = largura * 0.36f;
    if (largura_botao < 240.0f) largura_botao = 240.0f;
    if (largura_botao > 360.0f) largura_botao = 360.0f;
    float altura_botao = 50.0f;
    float posicao_x_botao = (largura - largura_botao) / 2;
    float inicio_y_botoes = altura * 0.46f;
    float espaco_entre_botoes = 20.0f;

    /* Botao JOGAR */
    Rectangle ret_jogar = { posicao_x_botao, inicio_y_botoes, largura_botao, altura_botao };
    if (desenhar_botao(ret_jogar, "JOGAR", mouse)) {
        *estado = TELA_JOGAR_PROVISORIA;
    }

    /* Botao COMO JOGAR */
    Rectangle ret_como_jogar = { posicao_x_botao, inicio_y_botoes + (altura_botao + espaco_entre_botoes), largura_botao, altura_botao };
    if (desenhar_botao(ret_como_jogar, "COMO JOGAR", mouse)) {
        *estado = TELA_COMO_JOGAR;
    }

    /* Botao SAIR */
    Rectangle ret_sair = { posicao_x_botao, inicio_y_botoes + 2 * (altura_botao + espaco_entre_botoes), largura_botao, altura_botao };
    if (desenhar_botao(ret_sair, "SAIR", mouse)) {
        *deve_sair = true;
    }
}

/*
 * Renderiza a tela provisoria acessada pelo botao JOGAR.
 */
void desenhar_tela_jogar_provisoria(int largura, int altura, Vector2 mouse, EstadoTela *estado) {
    desenhar_fundo_teatro(largura, altura);

    /* Painel centralizado com detalhes dourados */
    float largura_quadro = largura * 0.70f;
    float altura_quadro = altura * 0.55f;
    float quadro_x = (largura - largura_quadro) / 2;
    float quadro_y = altura * 0.20f;

    Rectangle quadro = { quadro_x, quadro_y, largura_quadro, altura_quadro };
    DrawRectangleRec(quadro, (Color){ 28, 14, 38, 235 });
    DrawRectangleLinesEx(quadro, 2.0f, COR_DOURADO);

    /* Titulo da secao */
    const char *titulo = "SELECAO DE MUSICAS";
    int tamanho_titulo = 28;
    int largura_titulo = MeasureText(titulo, tamanho_titulo);
    DrawText(titulo, quadro_x + (largura_quadro - largura_titulo) / 2, quadro_y + 35, tamanho_titulo, COR_DOURADO);

    /* Linha decorativa */
    DrawLine(quadro_x + 40, quadro_y + 75, quadro_x + largura_quadro - 40, quadro_y + 75, (Color){ 180, 140, 60, 160 });

    /* Mensagem explicativa didatica */
    const char *msg1 = "O catalogo de musicas, a ordenacao por Merge Sort";
    const char *msg2 = "e a lista encadeada da partida serao desenvolvidos";
    const char *msg3 = "nas proximas etapas do projeto.";

    int tamanho_texto = 18;
    DrawText(msg1, quadro_x + (largura_quadro - MeasureText(msg1, tamanho_texto)) / 2, quadro_y + 115, tamanho_texto, (Color){ 230, 220, 220, 255 });
    DrawText(msg2, quadro_x + (largura_quadro - MeasureText(msg2, tamanho_texto)) / 2, quadro_y + 145, tamanho_texto, (Color){ 230, 220, 220, 255 });
    DrawText(msg3, quadro_x + (largura_quadro - MeasureText(msg3, tamanho_texto)) / 2, quadro_y + 175, tamanho_texto, (Color){ 230, 220, 220, 255 });

    /* Botao para retornar ao menu inicial */
    float largura_botao = 220.0f;
    float altura_botao = 45.0f;
    Rectangle ret_voltar = { quadro_x + (largura_quadro - largura_botao) / 2, quadro_y + altura_quadro - 65, largura_botao, altura_botao };

    if (desenhar_botao(ret_voltar, "VOLTAR AO MENU", mouse)) {
        *estado = TELA_MENU_PRINCIPAL;
    }
}

/*
 * Renderiza a tela explicativa de instrucoes do jogo.
 */
void desenhar_tela_como_jogar(int largura, int altura, Vector2 mouse, EstadoTela *estado) {
    desenhar_fundo_teatro(largura, altura);

    /* Painel centralizado */
    float largura_quadro = largura * 0.75f;
    float altura_quadro = altura * 0.65f;
    float quadro_x = (largura - largura_quadro) / 2;
    float quadro_y = altura * 0.15f;

    Rectangle quadro = { quadro_x, quadro_y, largura_quadro, altura_quadro };
    DrawRectangleRec(quadro, (Color){ 28, 14, 38, 235 });
    DrawRectangleLinesEx(quadro, 2.0f, COR_DOURADO);

    /* Titulo */
    const char *titulo = "COMO JOGAR";
    int tamanho_titulo = 28;
    int largura_titulo = MeasureText(titulo, tamanho_titulo);
    DrawText(titulo, quadro_x + (largura_quadro - largura_titulo) / 2, quadro_y + 30, tamanho_titulo, COR_DOURADO);

    DrawLine(quadro_x + 40, quadro_y + 68, quadro_x + largura_quadro - 40, quadro_y + 68, (Color){ 180, 140, 60, 160 });

    /* Instrucoes */
    const char *instrucao1 = "As notas musicais descerao pelas pistas da tela.";
    const char *instrucao2 = "Pressione a tecla correspondente quando a nota cruzar a zona de acerto:";
    int tamanho_texto = 17;

    DrawText(instrucao1, quadro_x + (largura_quadro - MeasureText(instrucao1, tamanho_texto)) / 2, quadro_y + 90, tamanho_texto, (Color){ 230, 220, 220, 255 });
    DrawText(instrucao2, quadro_x + (largura_quadro - MeasureText(instrucao2, tamanho_texto)) / 2, quadro_y + 118, tamanho_texto, (Color){ 230, 220, 220, 255 });

    /* Indicadores visuais das teclas D, F, J, K */
    const char *teclas[] = { "D", "F", "J", "K" };
    Color cores_teclas[] = {
        (Color){ 190, 60, 100, 255 },  /* Pista 1: Tom rosa/vinho */
        (Color){ 70, 120, 210, 255 },  /* Pista 2: Azul */
        (Color){ 220, 160, 40, 255 },  /* Pista 3: Dourado */
        (Color){ 60, 170, 120, 255 }   /* Pista 4: Verde */
    };

    float tamanho_tecla = 52.0f;
    float espaco_teclas = 20.0f;
    float largura_total_teclas = (4 * tamanho_tecla) + (3 * espaco_teclas);
    float inicio_x_teclas = quadro_x + (largura_quadro - largura_total_teclas) / 2;
    float posicao_y_teclas = quadro_y + 160;

    for (int i = 0; i < 4; i++) {
        Rectangle ret_tecla = { inicio_x_teclas + i * (tamanho_tecla + espaco_teclas), posicao_y_teclas, tamanho_tecla, tamanho_tecla };
        DrawRectangleRec(ret_tecla, cores_teclas[i]);
        DrawRectangleLinesEx(ret_tecla, 2.0f, (Color){ 255, 255, 255, 200 });

        int largura_letra = MeasureText(teclas[i], 24);
        int letra_x = ret_tecla.x + (tamanho_tecla - largura_letra) / 2;
        int letra_y = ret_tecla.y + (tamanho_tecla - 24) / 2;
        DrawText(teclas[i], letra_x, letra_y, 24, WHITE);
    }

    /* Informacoes adicionais sobre pontuacao e combo */
    const char *dica1 = "Acertos aumentam sua pontuacao e constroem sequencias de combo.";
    const char *dica2 = "Erros ou notas perdidas interrompem a contagem do combo!";
    int tamanho_dica = 15;
    DrawText(dica1, quadro_x + (largura_quadro - MeasureText(dica1, tamanho_dica)) / 2, quadro_y + 240, tamanho_dica, (Color){ 200, 190, 200, 255 });
    DrawText(dica2, quadro_x + (largura_quadro - MeasureText(dica2, tamanho_dica)) / 2, quadro_y + 265, tamanho_dica, (Color){ 200, 190, 200, 255 });

    /* Botao para voltar ao menu */
    float largura_botao = 220.0f;
    float altura_botao = 45.0f;
    Rectangle ret_voltar = { quadro_x + (largura_quadro - largura_botao) / 2, quadro_y + altura_quadro - 60, largura_botao, altura_botao };

    if (desenhar_botao(ret_voltar, "VOLTAR AO MENU", mouse)) {
        *estado = TELA_MENU_PRINCIPAL;
    }
}

int main(void) {
    const int largura_inicial = 900;
    const int altura_inicial = 600;

    /* Permite redimensionar a janela mantendo proporcoes dinamicas */
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(largura_inicial, altura_inicial, "FINAL ACT - Jogo de Ritmo");
    SetTargetFPS(60);

    EstadoTela estado_atual = TELA_MENU_PRINCIPAL;
    bool deve_sair = false;

    /* Loop principal do jogo */
    while (!WindowShouldClose() && !deve_sair) {
        int largura_atual = GetScreenWidth();
        int altura_atual = GetScreenHeight();
        Vector2 posicao_mouse = GetMousePosition();

        BeginDrawing();

        /* Controle de telas atraves do estado atual */
        switch (estado_atual) {
            case TELA_MENU_PRINCIPAL:
                desenhar_menu_principal(largura_atual, altura_atual, posicao_mouse, &estado_atual, &deve_sair);
                break;

            case TELA_JOGAR_PROVISORIA:
                desenhar_tela_jogar_provisoria(largura_atual, altura_atual, posicao_mouse, &estado_atual);
                break;

            case TELA_COMO_JOGAR:
                desenhar_tela_como_jogar(largura_atual, altura_atual, posicao_mouse, &estado_atual);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

