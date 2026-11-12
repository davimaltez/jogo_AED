#include "interface.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

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
#define COR_TEXTO_SUAVE       (Color){ 210, 200, 220, 230 }
#define COR_ROTULO            (Color){ 195, 185, 205, 255 }
#define COR_LINHA_DOURADA     (Color){ 120, 90, 40, 150 }

#define COR_BOTAO_FUNDO       (Color){ 45, 22, 58, 230 }
#define COR_BOTAO_BORDA       (Color){ 190, 140, 60, 255 }
#define COR_BOTAO_TEXTO       (Color){ 245, 240, 235, 255 }

#define COR_BOTAO_FOCO_FUNDO  (Color){ 85, 30, 100, 240 }
#define COR_BOTAO_FOCO_BORDA  (Color){ 255, 215, 80, 255 }
#define COR_BOTAO_FOCO_TEXTO  (Color){ 255, 245, 180, 255 }

#define COR_PAINEL_FUNDO      (Color){ 28, 14, 38, 235 }

#define LARGURA_REFERENCIA 1280.0f
#define ALTURA_REFERENCIA 720.0f
#define ESCALA_MINIMA 0.45f
#define FONTE_MINIMA 10

/* ================= Escala e texto ================= */

float calcular_escala(int largura, int altura) {
    float escala_largura = largura / LARGURA_REFERENCIA;
    float escala_altura = altura / ALTURA_REFERENCIA;
    /* A menor das duas garante que tudo caiba nas duas direcoes. */
    float escala = escala_largura < escala_altura ? escala_largura : escala_altura;
    if (escala < ESCALA_MINIMA) {
        escala = ESCALA_MINIMA;
    }
    return escala;
}

int escalar(float valor, float escala) {
    int resultado = (int)(valor * escala + 0.5f);
    return resultado < 1 ? 1 : resultado;
}

/* Tamanho de fonte escalado, sem ficar ilegivel em janelas pequenas. */
static int fonte(int tamanho_base, float escala) {
    int tamanho = escalar((float)tamanho_base, escala);
    return tamanho < FONTE_MINIMA ? FONTE_MINIMA : tamanho;
}

/* Diminui a fonte ate o texto caber na largura disponivel. */
static int fonte_que_cabe(const char *texto, int tamanho, float largura_maxima) {
    while (tamanho > FONTE_MINIMA && MeasureText(texto, tamanho) > largura_maxima) {
        tamanho--;
    }
    return tamanho;
}

static void desenhar_texto_centralizado(const char *texto, float centro_x, float y,
                                        int tamanho, Color cor) {
    int largura_texto = MeasureText(texto, tamanho);
    DrawText(texto, (int)(centro_x - largura_texto / 2.0f), (int)y, tamanho, cor);
}

static void desenhar_painel(Rectangle painel, Color borda) {
    DrawRectangleRec(painel, COR_PAINEL_FUNDO);
    DrawRectangleLinesEx(painel, 2.0f, borda);
}

/* ================= Navegacao e botoes ================= */

void navegar_menu(int *foco, int quantidade) {
    if (foco == NULL || quantidade <= 0) return;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT)) {
        *foco = (*foco + quantidade - 1) % quantidade;
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT)) {
        *foco = (*foco + 1) % quantidade;
    }
}

/* O mouse so muda o foco quando se move ou clica sobre o item. */
static bool item_em_foco(Rectangle retangulo, Vector2 mouse, int *foco, int indice) {
    Vector2 movimento = GetMouseDelta();
    bool apontado = CheckCollisionPointRec(mouse, retangulo);
    if (foco != NULL && apontado &&
        (movimento.x != 0 || movimento.y != 0 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        *foco = indice;
    }
    return foco != NULL && *foco == indice;
}

/* Desenha um botao com texto centralizado. Retorna true se foi clicado. */
static bool desenhar_botao(Rectangle retangulo, const char *texto, Vector2 mouse, int tamanho_fonte) {
    bool mouse_em_cima = CheckCollisionPointRec(mouse, retangulo);
    bool clicado = mouse_em_cima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color cor_fundo = mouse_em_cima ? COR_BOTAO_FOCO_FUNDO : COR_BOTAO_FUNDO;
    Color cor_borda = mouse_em_cima ? COR_BOTAO_FOCO_BORDA : COR_BOTAO_BORDA;
    Color cor_texto = mouse_em_cima ? COR_BOTAO_FOCO_TEXTO : COR_BOTAO_TEXTO;

    DrawRectangleRec(retangulo, cor_fundo);
    DrawRectangleLinesEx(retangulo, mouse_em_cima ? 3.0f : 2.0f, cor_borda);

    int tamanho = fonte_que_cabe(texto, tamanho_fonte, retangulo.width - 8.0f);
    int largura_texto = MeasureText(texto, tamanho);
    DrawText(texto, (int)(retangulo.x + (retangulo.width - largura_texto) / 2),
             (int)(retangulo.y + (retangulo.height - tamanho) / 2), tamanho, cor_texto);
    return clicado;
}

/* Botao que tambem responde ao teclado (foco + Enter). */
static bool desenhar_botao_navegavel(Rectangle retangulo, const char *texto, Vector2 mouse,
                                     int tamanho_fonte, int *foco, int indice) {
    bool selecionado = item_em_foco(retangulo, mouse, foco, indice);
    bool clicado = desenhar_botao(retangulo, texto, mouse, tamanho_fonte);
    if (selecionado) DrawRectangleLinesEx(retangulo, 4.0f, COR_DOURADO_BRILHO);
    return clicado || (selecionado && IsKeyPressed(KEY_ENTER));
}

/* Botao dos filtros de ordenacao; "ativo" destaca o criterio atual. */
static bool desenhar_botao_filtro(Rectangle retangulo, const char *texto, bool ativo,
                                  Vector2 mouse, int tamanho_fonte, int *foco, int indice) {
    bool selecionado = item_em_foco(retangulo, mouse, foco, indice);
    bool mouse_em_cima = CheckCollisionPointRec(mouse, retangulo);
    bool clicado = mouse_em_cima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color cor_fundo = ativo ? (Color){ 95, 35, 110, 240 } : (mouse_em_cima ? COR_BOTAO_FOCO_FUNDO : COR_BOTAO_FUNDO);
    Color cor_borda = ativo ? COR_DOURADO : (mouse_em_cima ? COR_BOTAO_FOCO_BORDA : COR_BOTAO_BORDA);
    Color cor_texto = ativo ? COR_DOURADO_BRILHO : (mouse_em_cima ? COR_BOTAO_FOCO_TEXTO : COR_BOTAO_TEXTO);

    DrawRectangleRec(retangulo, cor_fundo);
    DrawRectangleLinesEx(retangulo, ativo ? 2.5f : 1.5f, cor_borda);

    int tamanho = fonte_que_cabe(texto, tamanho_fonte, retangulo.width - 6.0f);
    int largura_texto = MeasureText(texto, tamanho);
    DrawText(texto, (int)(retangulo.x + (retangulo.width - largura_texto) / 2),
             (int)(retangulo.y + (retangulo.height - tamanho) / 2), tamanho, cor_texto);

    if (selecionado) DrawRectangleLinesEx(retangulo, 4.0f, COR_DOURADO_BRILHO);
    return clicado || (selecionado && IsKeyPressed(KEY_ENTER));
}

/* ================= Fundo ================= */

/* Palco de teatro: cortinas nas laterais e feixes de luz cruzando a tela. */
void desenhar_fundo_teatro(int largura, int altura) {
    ClearBackground(COR_FUNDO_ESCURO);

    int largura_cortina = (int)(largura * 0.08f);
    DrawRectangle(0, 0, largura_cortina, altura, COR_PALCO_LATERAL);
    DrawRectangle(largura - largura_cortina, 0, largura_cortina, altura, COR_PALCO_LATERAL);
    DrawLine(largura_cortina, 0, largura_cortina, altura, (Color){ 120, 90, 40, 120 });
    DrawLine(largura - largura_cortina, 0, largura - largura_cortina, altura, (Color){ 120, 90, 40, 120 });

    Vector2 topo_esquerda = { largura * 0.2f, 0.0f };
    Vector2 base_esquerda_1 = { 0.0f, (float)altura };
    Vector2 base_esquerda_2 = { largura * 0.55f, (float)altura };
    DrawTriangle(topo_esquerda, base_esquerda_1, base_esquerda_2, COR_LUZ_HOLOFOTE);

    Vector2 topo_direita = { largura * 0.8f, 0.0f };
    Vector2 base_direita_1 = { largura * 0.45f, (float)altura };
    Vector2 base_direita_2 = { (float)largura, (float)altura };
    DrawTriangle(topo_direita, base_direita_1, base_direita_2, COR_LUZ_HOLOFOTE);
}

/* ================= Menu principal ================= */

AcaoMenu desenhar_menu_principal(int largura, int altura, Vector2 mouse, int *foco) {
    float escala = calcular_escala(largura, altura);
    navegar_menu(foco, 3);
    desenhar_fundo_teatro(largura, altura);

    const char *titulo = "FINAL ACT";
    int tamanho_titulo = fonte(100, escala);
    float y_titulo = altura * 0.14f;
    int sombra = escalar(4, escala);
    desenhar_texto_centralizado(titulo, largura / 2.0f + sombra, y_titulo + sombra, tamanho_titulo, COR_SOMBRA_TEXTO);
    desenhar_texto_centralizado(titulo, largura / 2.0f, y_titulo, tamanho_titulo, COR_DOURADO);
    desenhar_texto_centralizado("UM JOGO DE RITMO EM CENA", largura / 2.0f,
                                y_titulo + tamanho_titulo + escalar(14, escala),
                                fonte(22, escala), COR_DOURADO_BRILHO);

    float largura_botao = (float)escalar(400, escala);
    float altura_botao = (float)escalar(60, escala);
    float espaco = (float)escalar(20, escala);
    float x_botao = (largura - largura_botao) / 2.0f;
    float y_inicio = altura * 0.44f;
    int tamanho_fonte = fonte(26, escala);

    const char *textos[] = { "JOGAR", "COMO JOGAR", "SAIR" };
    const AcaoMenu acoes[] = { ACAO_JOGAR, ACAO_COMO_JOGAR, ACAO_PEDIR_SAIDA };
    for (int i = 0; i < 3; i++) {
        Rectangle botao = { x_botao, y_inicio + i * (altura_botao + espaco), largura_botao, altura_botao };
        if (desenhar_botao_navegavel(botao, textos[i], mouse, tamanho_fonte, foco, i)) {
            return acoes[i];
        }
    }
    return ACAO_NENHUMA;
}

/* ================= Como jogar ================= */

AcaoMenu desenhar_tela_como_jogar(int largura, int altura, Vector2 mouse, int *foco) {
    float escala = calcular_escala(largura, altura);
    navegar_menu(foco, 1);
    desenhar_fundo_teatro(largura, altura);

    Rectangle quadro = { largura * 0.09f, altura * 0.10f, largura * 0.82f, altura * 0.78f };
    desenhar_painel(quadro, COR_DOURADO);
    float centro_x = quadro.x + quadro.width / 2.0f;
    float largura_texto = quadro.width - escalar(40, escala);

    desenhar_texto_centralizado("COMO JOGAR", centro_x, quadro.y + escalar(24, escala),
                                fonte(36, escala), COR_DOURADO);
    DrawLine((int)(quadro.x + escalar(40, escala)), (int)(quadro.y + escalar(70, escala)),
             (int)(quadro.x + quadro.width - escalar(40, escala)), (int)(quadro.y + escalar(70, escala)),
             (Color){ 180, 140, 60, 160 });

    const char *linhas[] = {
        "As notas musicais descem pelas pistas da tela.",
        "Pressione a tecla da pista quando a nota cruzar a linha dourada:"
    };
    int tamanhos[] = { 22, 20 };
    float posicoes[] = { 90, 124 };
    for (int i = 0; i < 2; i++) {
        int tamanho = fonte_que_cabe(linhas[i], fonte(tamanhos[i], escala), largura_texto);
        desenhar_texto_centralizado(linhas[i], centro_x, quadro.y + escalar(posicoes[i], escala),
                                    tamanho, (Color){ 230, 220, 220, 255 });
    }

    const char *teclas[] = { "D", "F", "G", "J", "K" };
    Color cores_teclas[] = {
        (Color){ 190, 60, 100, 255 }, (Color){ 70, 120, 210, 255 }, (Color){ 220, 160, 40, 255 },
        (Color){ 60, 170, 120, 255 }, (Color){ 180, 90, 245, 255 }
    };
    float tamanho_tecla = (float)escalar(70, escala);
    float espaco_teclas = (float)escalar(22, escala);
    float largura_teclas = TOTAL_PISTAS * tamanho_tecla + (TOTAL_PISTAS - 1) * espaco_teclas;
    float x_teclas = centro_x - largura_teclas / 2.0f;
    float y_teclas = quadro.y + escalar(166, escala);
    int fonte_tecla = fonte(34, escala);
    for (int i = 0; i < TOTAL_PISTAS; i++) {
        Rectangle tecla = { x_teclas + i * (tamanho_tecla + espaco_teclas), y_teclas, tamanho_tecla, tamanho_tecla };
        DrawRectangleRec(tecla, cores_teclas[i]);
        DrawRectangleLinesEx(tecla, 2.5f, (Color){ 255, 255, 255, 220 });
        desenhar_texto_centralizado(teclas[i], tecla.x + tamanho_tecla / 2.0f,
                                    tecla.y + (tamanho_tecla - fonte_tecla) / 2.0f, fonte_tecla, WHITE);
    }

    const char *dicas[] = {
        "Acertos aumentam a pontuacao e constroem combos.",
        "Erros e notas perdidas zeram o combo e baixam o medidor.",
        "Termine a musica para gravar seu nome no ranking. ESC pausa."
    };
    for (int i = 0; i < 3; i++) {
        int tamanho = fonte_que_cabe(dicas[i], fonte(19, escala), largura_texto);
        desenhar_texto_centralizado(dicas[i], centro_x, quadro.y + escalar(262 + i * 30, escala),
                                    tamanho, COR_TEXTO_SUAVE);
    }

    float largura_botao = (float)escalar(260, escala);
    float altura_botao = (float)escalar(48, escala);
    Rectangle voltar = { centro_x - largura_botao / 2.0f,
                         quadro.y + quadro.height - altura_botao - escalar(18, escala),
                         largura_botao, altura_botao };
    if (desenhar_botao_navegavel(voltar, "VOLTAR AO MENU", mouse, fonte(22, escala), foco, 0)) {
        return ACAO_VOLTAR_MENU;
    }
    return ACAO_NENHUMA;
}

/* ================= Selecao de musicas ================= */

/* Teclado da selecao: esquerda/direita nos filtros e atalho para iniciar. */
static void navegar_selecao(int *foco, int total_musicas, int indice_selecionado) {
    if (foco == NULL) return;
    int primeiro_item = 3;
    int botao_iniciar = total_musicas + 3;
    if (*foco < 3 && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        *foco = (*foco + (IsKeyPressed(KEY_LEFT) ? 2 : 1)) % 3;
    } else if (*foco >= primeiro_item && *foco < botao_iniciar && IsKeyPressed(KEY_RIGHT)) {
        *foco = botao_iniciar;
    } else if ((*foco == botao_iniciar || *foco == botao_iniciar + 1) && IsKeyPressed(KEY_LEFT)) {
        *foco = indice_selecionado + primeiro_item;
    } else {
        navegar_menu(foco, total_musicas + 6);
    }
}

static AcaoMenu desenhar_filtros(Rectangle painel, float escala, Vector2 mouse,
                                 CriterioOrdenacao criterio, int *foco) {
    float margem = (float)escalar(15, escala);
    float espaco = (float)escalar(5, escala);
    float largura_botao = (painel.width - 2 * margem - 2 * espaco) / 3.0f;
    float altura_botao = (float)escalar(42, escala);
    float y = painel.y + escalar(40, escala);

    const char *textos[] = { "Titulo A-Z", "Dif. Cresc.", "Dif. Decr." };
    const CriterioOrdenacao criterios[] = {
        ORDEM_TITULO_AZ, ORDEM_DIFICULDADE_CRESCENTE, ORDEM_DIFICULDADE_DECRESCENTE
    };
    const AcaoMenu acoes[] = {
        ACAO_ORDENAR_TITULO, ACAO_ORDENAR_DIFICULDADE_CRESCENTE, ACAO_ORDENAR_DIFICULDADE_DECRESCENTE
    };
    AcaoMenu escolhida = ACAO_NENHUMA;
    for (int i = 0; i < 3; i++) {
        Rectangle botao = { painel.x + margem + i * (largura_botao + espaco), y, largura_botao, altura_botao };
        bool ativo = criterio == criterios[i];
        if (desenhar_botao_filtro(botao, textos[i], ativo, mouse, fonte(17, escala), foco, i) && !ativo) {
            escolhida = acoes[i];
        }
    }
    return escolhida;
}

/* Desenha os cartoes das musicas. Retorna true se Enter foi usado numa musica. */
static bool desenhar_lista_musicas(Rectangle painel, float y_inicio, float escala, Vector2 mouse,
                                   const Musica catalogo[], int total_musicas,
                                   int *indice_selecionado, int *foco) {
    float margem = (float)escalar(15, escala);
    float espaco_disponivel = painel.y + painel.height - escalar(12, escala) - y_inicio;
    float espaco_itens = (float)escalar(8, escala);
    float altura_item = (espaco_disponivel - (total_musicas - 1) * espaco_itens) / total_musicas;
    float altura_maxima = (float)escalar(80, escala);
    if (altura_item > altura_maxima) {
        altura_item = altura_maxima;
        if (total_musicas > 1) {
            espaco_itens = (espaco_disponivel - total_musicas * altura_item) / (total_musicas - 1);
            if (espaco_itens > escalar(16, escala)) espaco_itens = (float)escalar(16, escala);
        }
    }

    bool enter_em_musica = false;
    for (int i = 0; i < total_musicas; i++) {
        Rectangle item = { painel.x + margem, y_inicio + i * (altura_item + espaco_itens),
                           painel.width - 2 * margem, altura_item };
        bool mouse_em_cima = CheckCollisionPointRec(mouse, item);
        bool tem_foco = item_em_foco(item, mouse, foco, i + 3);
        if (mouse_em_cima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            *indice_selecionado = i;
        }
        if (tem_foco && IsKeyPressed(KEY_ENTER)) {
            *indice_selecionado = i;
            enter_em_musica = true;
        }
        bool selecionada = *indice_selecionado == i;

        Color cor_fundo = selecionada ? (Color){ 110, 40, 125, 250 } :
                          (mouse_em_cima ? COR_BOTAO_FOCO_FUNDO : COR_BOTAO_FUNDO);
        Color cor_borda = selecionada ? COR_DOURADO : (mouse_em_cima ? COR_BOTAO_FOCO_BORDA : COR_BOTAO_BORDA);
        DrawRectangleRec(item, cor_fundo);
        DrawRectangleLinesEx(item, selecionada ? 3.5f : 1.5f, cor_borda);
        if (tem_foco) DrawRectangleLinesEx(item, 4.0f, COR_DOURADO_BRILHO);

        float recuo_texto = (float)escalar(14, escala);
        if (selecionada) {
            DrawRectangle((int)item.x, (int)item.y, escalar(8, escala), (int)item.height, COR_DOURADO);
            recuo_texto = (float)escalar(22, escala);
        }
        float largura_texto = item.width - recuo_texto - escalar(8, escala);

        int fonte_titulo = fonte(24, escala);
        if (fonte_titulo > altura_item * 0.36f) fonte_titulo = (int)(altura_item * 0.36f);
        fonte_titulo = fonte_que_cabe(catalogo[i].titulo, fonte_titulo, largura_texto);
        DrawText(catalogo[i].titulo, (int)(item.x + recuo_texto), (int)(item.y + altura_item * 0.14f),
                 fonte_titulo, selecionada ? COR_DOURADO : WHITE);

        char informacoes[96];
        snprintf(informacoes, sizeof(informacoes), "%s  *  Nivel %d", catalogo[i].filme, catalogo[i].dificuldade);
        int fonte_info = fonte(17, escala);
        if (fonte_info > altura_item * 0.26f) fonte_info = (int)(altura_item * 0.26f);
        fonte_info = fonte_que_cabe(informacoes, fonte_info, largura_texto);
        DrawText(informacoes, (int)(item.x + recuo_texto), (int)(item.y + altura_item * 0.56f),
                 fonte_info, COR_TEXTO_SUAVE);
    }
    return enter_em_musica;
}

/* Painel da direita: dados da musica selecionada e botao de iniciar. */
static AcaoMenu desenhar_detalhes_musica(Rectangle painel, float escala, Vector2 mouse,
                                         const Musica *musica, int indice_botao,
                                         const char *mensagem_erro, int *foco) {
    float x = painel.x + escalar(25, escala);
    float largura_texto = painel.width - escalar(50, escala);
    float y = painel.y;

    DrawText("Titulo:", (int)x, (int)(y + escalar(54, escala)), fonte(18, escala), COR_ROTULO);
    int fonte_titulo = fonte_que_cabe(musica->titulo, fonte(34, escala), largura_texto);
    DrawText(musica->titulo, (int)x, (int)(y + escalar(78, escala)), fonte_titulo, COR_DOURADO);

    DrawText("Filme Musical:", (int)x, (int)(y + escalar(130, escala)), fonte(18, escala), COR_ROTULO);
    int fonte_filme = fonte_que_cabe(musica->filme, fonte(24, escala), largura_texto);
    DrawText(musica->filme, (int)x, (int)(y + escalar(154, escala)), fonte_filme, WHITE);

    DrawText("Dificuldade:", (int)x, (int)(y + escalar(200, escala)), fonte(18, escala), COR_ROTULO);
    char texto_nivel[32];
    snprintf(texto_nivel, sizeof(texto_nivel), "Nivel %d de 5", musica->dificuldade);
    DrawText(texto_nivel, (int)x, (int)(y + escalar(224, escala)), fonte(22, escala), COR_DOURADO_BRILHO);

    for (int nivel = 1; nivel <= 5; nivel++) {
        Rectangle bloco = { x + (nivel - 1) * escalar(44, escala), y + escalar(258, escala),
                            (float)escalar(36, escala), (float)escalar(20, escala) };
        if (nivel <= musica->dificuldade) {
            DrawRectangleRec(bloco, COR_DOURADO);
        } else {
            DrawRectangleRec(bloco, (Color){ 60, 40, 70, 200 });
            DrawRectangleLinesEx(bloco, 1.0f, (Color){ 120, 90, 130, 200 });
        }
    }

    Rectangle iniciar = { x, y + escalar(300, escala), largura_texto, (float)escalar(54, escala) };
    AcaoMenu acao = ACAO_NENHUMA;
    if (desenhar_botao_navegavel(iniciar, "INICIAR PARTIDA", mouse, fonte(22, escala),
                                 foco, indice_botao)) {
        acao = ACAO_INICIAR_PARTIDA;
    }

    /* Abre o top 10 desta musica sem precisar vence-la. */
    Rectangle ver_ranking = { x, y + escalar(366, escala), largura_texto, (float)escalar(46, escala) };
    if (desenhar_botao_navegavel(ver_ranking, "VER RANKING", mouse, fonte(20, escala),
                                 foco, indice_botao + 1)) {
        acao = ACAO_VER_RANKING;
    }

    float y_aviso = y + escalar(426, escala);
    if (mensagem_erro != NULL && mensagem_erro[0] != '\0') {
        int tamanho = fonte_que_cabe(mensagem_erro, fonte(15, escala), largura_texto);
        DrawText(mensagem_erro, (int)x, (int)y_aviso, tamanho, RED);
    } else {
        const char *dica = "ENTER numa musica tambem inicia a partida.";
        int tamanho = fonte_que_cabe(dica, fonte(15, escala), largura_texto);
        DrawText(dica, (int)x, (int)y_aviso, tamanho, COR_TEXTO_SUAVE);
    }
    return acao;
}

AcaoMenu desenhar_tela_selecao_musicas(int largura, int altura, Vector2 mouse,
                                       const Musica catalogo[], int total_musicas,
                                       int *indice_selecionado, CriterioOrdenacao criterio,
                                       const char *mensagem_erro, int *foco) {
    float escala = calcular_escala(largura, altura);
    int foco_anterior = foco != NULL ? *foco : -1;
    navegar_selecao(foco, total_musicas, *indice_selecionado);
    /* Andar com as setas pelas musicas tambem muda a musica selecionada. */
    if (foco != NULL && *foco != foco_anterior && *foco >= 3 && *foco < total_musicas + 3) {
        *indice_selecionado = *foco - 3;
    }

    desenhar_fundo_teatro(largura, altura);
    desenhar_texto_centralizado("SELECAO DE MUSICAS", largura / 2.0f, (float)escalar(24, escala),
                                fonte(36, escala), COR_DOURADO);

    float margem_x = largura * 0.06f;
    float topo_y = (float)escalar(80, escala);
    float largura_util = largura - 2 * margem_x;
    float altura_paineis = altura - topo_y - escalar(90, escala);
    float largura_coluna = largura_util * 0.485f;
    float espaco_colunas = largura_util * 0.03f;

    /* Coluna esquerda: filtros e lista */
    Rectangle painel_lista = { margem_x, topo_y, largura_coluna, altura_paineis };
    desenhar_painel(painel_lista, (Color){ 140, 100, 50, 200 });
    const char *cabecalho = "ORDENAR CATALOGO (MERGE SORT)";
    int fonte_cabecalho = fonte_que_cabe(cabecalho, fonte(18, escala), largura_coluna - escalar(30, escala));
    DrawText(cabecalho, (int)(margem_x + escalar(18, escala)), (int)(topo_y + escalar(12, escala)),
             fonte_cabecalho, COR_DOURADO_BRILHO);

    AcaoMenu acao = desenhar_filtros(painel_lista, escala, mouse, criterio, foco);

    float y_linha = topo_y + escalar(92, escala);
    DrawLine((int)(margem_x + escalar(15, escala)), (int)y_linha,
             (int)(margem_x + largura_coluna - escalar(15, escala)), (int)y_linha, COR_LINHA_DOURADA);

    if (desenhar_lista_musicas(painel_lista, y_linha + escalar(10, escala), escala, mouse,
                               catalogo, total_musicas, indice_selecionado, foco)) {
        acao = ACAO_INICIAR_PARTIDA;
    }

    /* Coluna direita: detalhes */
    Rectangle painel_detalhes = { margem_x + largura_coluna + espaco_colunas, topo_y, largura_coluna, altura_paineis };
    desenhar_painel(painel_detalhes, (Color){ 140, 100, 50, 200 });
    DrawText("DETALHES DA FAIXA", (int)(painel_detalhes.x + escalar(20, escala)),
             (int)(topo_y + escalar(12, escala)), fonte(20, escala), COR_DOURADO_BRILHO);
    DrawLine((int)(painel_detalhes.x + escalar(20, escala)), (int)(topo_y + escalar(42, escala)),
             (int)(painel_detalhes.x + largura_coluna - escalar(20, escala)), (int)(topo_y + escalar(42, escala)),
             COR_LINHA_DOURADA);

    if (*indice_selecionado >= 0 && *indice_selecionado < total_musicas) {
        AcaoMenu acao_detalhes = desenhar_detalhes_musica(painel_detalhes, escala, mouse,
                                                          &catalogo[*indice_selecionado],
                                                          total_musicas + 3, mensagem_erro, foco);
        if (acao_detalhes != ACAO_NENHUMA) {
            acao = acao_detalhes;
        }
    }

    float largura_voltar = (float)escalar(260, escala);
    float altura_voltar = (float)escalar(48, escala);
    Rectangle voltar = { (largura - largura_voltar) / 2.0f, altura - escalar(72, escala), largura_voltar, altura_voltar };
    if (desenhar_botao_navegavel(voltar, "VOLTAR AO MENU", mouse, fonte(22, escala), foco, total_musicas + 5)) {
        acao = ACAO_VOLTAR_MENU;
    }
    return acao;
}

/* ================= Partida ================= */

/* O semicirculo vai do vermelho (esquerda) ao verde (direita). */
static void desenhar_medidor(int largura, int altura, float escala, int valor) {
    float raio = largura * 0.085f;
    float raio_maximo = 110.0f * escala;
    if (raio > raio_maximo) raio = raio_maximo;
    Vector2 centro = { largura * 0.11f, altura * 0.48f };
    const int segmentos = 100;
    for (int i = 0; i < segmentos; i++) {
        float angulo_inicio = PI + PI * i / segmentos;
        float angulo_fim = PI + PI * (i + 1) / segmentos;
        Vector2 inicio = { centro.x + cosf(angulo_inicio) * raio, centro.y + sinf(angulo_inicio) * raio };
        Vector2 fim = { centro.x + cosf(angulo_fim) * raio, centro.y + sinf(angulo_fim) * raio };
        Color cor = i < LIMITE_VERMELHO ? (Color){ 225, 65, 80, 255 } :
                    (i < LIMITE_VERDE ? COR_DOURADO : (Color){ 45, 210, 125, 255 });
        DrawLineEx(inicio, fim, raio * 0.15f, cor);
    }
    float angulo = PI + PI * valor / DESEMPENHO_MAXIMO;
    Vector2 ponta = { centro.x + cosf(angulo) * raio * 0.8f, centro.y + sinf(angulo) * raio * 0.8f };
    DrawLineEx(centro, ponta, 3.0f, WHITE);
    DrawCircleV(centro, 5.0f * escala, COR_DOURADO);

    int tamanho = fonte(16, escala);
    desenhar_texto_centralizado("DESEMPENHO", centro.x, centro.y - raio - tamanho * 2, tamanho, COR_DOURADO);
    char texto[32];
    snprintf(texto, sizeof(texto), "%d / 100", valor);
    desenhar_texto_centralizado(texto, centro.x, centro.y + tamanho, tamanho, WHITE);
}

/*
 * Medidas da pista em perspectiva (trapezio que converge para o topo).
 * Calculadas uma vez por quadro e usadas por todas as partes do desenho.
 */
typedef struct {
    float centro_x;
    float y_topo;          /* horizonte da pista */
    float y_base;          /* fim da pista perto do jogador */
    float y_alvo;          /* linha de acerto */
    float largura_topo;
    float largura_base;
} GeometriaPista;

static GeometriaPista calcular_geometria_pista(int largura, int altura) {
    GeometriaPista pista;
    pista.centro_x = largura / 2.0f;
    pista.y_topo = altura * 0.16f;
    pista.y_base = altura * 0.94f;
    pista.y_alvo = altura * 0.82f;
    pista.largura_topo = largura * 0.20f;
    pista.largura_base = largura * 0.56f;
    return pista;
}

/* Largura do trapezio numa altura y qualquer (interpolacao linear). */
static float largura_pista_em(const GeometriaPista *pista, float y) {
    float fator = (y - pista->y_topo) / (pista->y_base - pista->y_topo);
    if (fator < 0.0f) fator = 0.0f;
    if (fator > 1.0f) fator = 1.0f;
    return pista->largura_topo + (pista->largura_base - pista->largura_topo) * fator;
}

static const Color CORES_PISTAS[TOTAL_PISTAS] = {
    { 225, 50, 110, 255 },   /* D: rosa/vinho */
    { 50, 140, 245, 255 },   /* F: azul */
    { 245, 195, 65, 255 },   /* G: dourado */
    { 45, 210, 125, 255 },   /* J: verde */
    { 180, 90, 245, 255 }    /* K: roxo */
};

static void desenhar_corpo_pista(const GeometriaPista *pista) {
    Vector2 topo_esquerda = { pista->centro_x - pista->largura_topo / 2.0f, pista->y_topo };
    Vector2 topo_direita = { pista->centro_x + pista->largura_topo / 2.0f, pista->y_topo };
    Vector2 base_direita = { pista->centro_x + pista->largura_base / 2.0f, pista->y_base };
    Vector2 base_esquerda = { pista->centro_x - pista->largura_base / 2.0f, pista->y_base };
    DrawTriangle(topo_esquerda, base_esquerda, base_direita, (Color){ 16, 9, 24, 235 });
    DrawTriangle(topo_esquerda, base_direita, topo_direita, (Color){ 16, 9, 24, 235 });

    /* A raylib 5.5 recebe coordenadas inteiras; a 6 recebe Vector2. */
#if RAYLIB_VERSION_MAJOR >= 6
    DrawCircleGradient((Vector2){ pista->centro_x, pista->y_topo }, pista->largura_topo * 0.6f,
                       (Color){ 255, 220, 130, 45 }, (Color){ 0, 0, 0, 0 });
#else
    DrawCircleGradient((int)pista->centro_x, (int)pista->y_topo, pista->largura_topo * 0.6f,
                       (Color){ 255, 220, 130, 45 }, (Color){ 0, 0, 0, 0 });
#endif

    for (int p = 0; p <= TOTAL_PISTAS; p++) {
        float fracao = p / (float)TOTAL_PISTAS;
        Vector2 inicio = { pista->centro_x - pista->largura_topo / 2.0f + fracao * pista->largura_topo, pista->y_topo };
        Vector2 fim = { pista->centro_x - pista->largura_base / 2.0f + fracao * pista->largura_base, pista->y_base };
        if (p == 0 || p == TOTAL_PISTAS) {
            DrawLineEx(inicio, fim, 3.0f, COR_DOURADO);
        } else {
            DrawLineEx(inicio, fim, 1.5f, (Color){ 110, 70, 120, 160 });
        }
    }
}

static void desenhar_receptores(const GeometriaPista *pista, float escala,
                                const bool teclas_seguradas[TOTAL_PISTAS]) {
    float largura_alvo = largura_pista_em(pista, pista->y_alvo);
    float x_alvo_esquerda = pista->centro_x - largura_alvo / 2.0f;
    Vector2 linha_inicio = { x_alvo_esquerda, pista->y_alvo };
    Vector2 linha_fim = { x_alvo_esquerda + largura_alvo, pista->y_alvo };
    DrawLineEx(linha_inicio, linha_fim, 6.0f, COR_DOURADO);
    DrawLineEx(linha_inicio, linha_fim, 2.0f, WHITE);

    const char *nomes_teclas[] = { "D", "F", "G", "J", "K" };
    float largura_coluna = largura_alvo / TOTAL_PISTAS;
    float altura_receptor = (float)escalar(34, escala);
    int fonte_letra = fonte(24, escala);

    for (int p = 0; p < TOTAL_PISTAS; p++) {
        float centro_coluna = x_alvo_esquerda + (p + 0.5f) * largura_coluna;
        float largura_receptor = largura_coluna * 0.85f;
        Rectangle receptor = { centro_coluna - largura_receptor / 2.0f, pista->y_alvo - altura_receptor / 2.0f,
                               largura_receptor, altura_receptor };
        bool ativa = teclas_seguradas[p];
        DrawRectangleRec(receptor, ativa ? CORES_PISTAS[p] : (Color){ 35, 18, 48, 230 });
        DrawRectangleLinesEx(receptor, ativa ? 3.5f : 2.0f, ativa ? WHITE : CORES_PISTAS[p]);

        if (ativa) {
            /* Feixe de luz subindo pela pista da tecla segurada. */
            float fracao_esquerda = (float)p / TOTAL_PISTAS;
            float fracao_direita = (float)(p + 1) / TOTAL_PISTAS;
            float x_topo = pista->centro_x - pista->largura_topo / 2.0f;
            float x_base = pista->centro_x - pista->largura_base / 2.0f;
            Vector2 topo_esquerda = { x_topo + fracao_esquerda * pista->largura_topo, pista->y_topo };
            Vector2 topo_direita = { x_topo + fracao_direita * pista->largura_topo, pista->y_topo };
            Vector2 base_esquerda = { x_base + fracao_esquerda * pista->largura_base, pista->y_alvo };
            Vector2 base_direita = { x_base + fracao_direita * pista->largura_base, pista->y_alvo };
            Color cor_feixe = CORES_PISTAS[p];
            cor_feixe.a = 40;
            DrawTriangle(topo_esquerda, base_esquerda, base_direita, cor_feixe);
            DrawTriangle(topo_esquerda, base_direita, topo_direita, cor_feixe);
        }
        desenhar_texto_centralizado(nomes_teclas[p], centro_coluna, pista->y_alvo - fonte_letra / 2.0f,
                                    fonte_letra, WHITE);
    }
}

/*
 * Percorre a lista encadeada e desenha cada nota pendente.
 * A posicao vem do tempo que falta para a nota (tempo_alvo - tempo atual),
 * e nao da quantidade de quadros: por isso a nota acompanha o audio.
 */
static void desenhar_notas(const GeometriaPista *pista, float escala, const Partida *partida) {
    const float tempo_antecedencia = 1.9f;   /* segundos que a nota leva do horizonte ate a linha */

    for (const Nota *atual = partida->notas; atual != NULL; atual = atual->proximo) {
        if (atual->estado != NOTA_PENDENTE) {
            continue;
        }
        float tempo_restante = atual->tempo_alvo - partida->tempo;
        float progresso = 1.0f - (tempo_restante / tempo_antecedencia);
        /* Visivel do horizonte (0.0) ate um pouco depois da linha (1.15). */
        if (progresso < 0.0f || progresso > 1.15f) {
            continue;
        }
        /* Curva de perspectiva: a nota acelera ao se aproximar. */
        float fator_curva = powf(progresso, 1.55f);
        float y_nota = pista->y_topo + (pista->y_alvo - pista->y_topo) * fator_curva;

        float fator_largura = (y_nota - pista->y_topo) / (pista->y_base - pista->y_topo);
        if (fator_largura > 1.0f) fator_largura = 1.0f;
        float largura_pista = largura_pista_em(pista, y_nota);
        float largura_coluna = largura_pista / TOTAL_PISTAS;
        float centro_nota = pista->centro_x - largura_pista / 2.0f + (atual->pista + 0.5f) * largura_coluna;
        float largura_nota = largura_coluna * 0.85f;
        float altura_nota = (14.0f + 18.0f * fator_largura) * escala;

        Rectangle nota = { centro_nota - largura_nota / 2.0f, y_nota - altura_nota / 2.0f, largura_nota, altura_nota };
        DrawRectangleRec(nota, CORES_PISTAS[atual->pista]);
        DrawRectangleLinesEx(nota, 2.0f, WHITE);
        DrawRectangle((int)nota.x + 3, (int)nota.y + 2, (int)nota.width - 6, (int)(altura_nota * 0.35f),
                      (Color){ 255, 255, 255, 170 });
    }
}

static void desenhar_painel_informacoes(int largura, int altura, float escala,
                                        const Partida *partida, const Musica *musica) {
    float margem = (float)escalar(35, escala);
    DrawText(musica->titulo, (int)margem, escalar(25, escala), fonte(26, escala), COR_DOURADO);
    DrawText(musica->filme, (int)margem, escalar(58, escala), fonte(19, escala), COR_TEXTO_SUAVE);

    char texto_pontos[64];
    snprintf(texto_pontos, sizeof(texto_pontos), "PONTOS: %d", partida->pontuacao);
    int fonte_pontos = fonte(26, escala);
    DrawText(texto_pontos, (int)(largura - MeasureText(texto_pontos, fonte_pontos) - margem),
             escalar(25, escala), fonte_pontos, COR_DOURADO);

    if (partida->combo > 0) {
        char texto_combo[32];
        snprintf(texto_combo, sizeof(texto_combo), "COMBO: %dx", partida->combo);
        int fonte_combo = fonte(24, escala);
        DrawText(texto_combo, (int)(largura - MeasureText(texto_combo, fonte_combo) - margem),
                 escalar(58, escala), fonte_combo, (Color){ 140, 255, 170, 255 });
    }

    desenhar_texto_centralizado("Pressione [ESC] para pausar", largura / 2.0f, altura - escalar(28, escala),
                                fonte(18, escala), (Color){ 190, 180, 200, 220 });
}

void desenhar_tela_partida(int largura, int altura, const Partida *partida,
                           const Musica *musica, const MidiaPartida *midia,
                           const bool teclas_seguradas[TOTAL_PISTAS]) {
    float escala = calcular_escala(largura, altura);
    GeometriaPista pista = calcular_geometria_pista(largura, altura);

    desenhar_fundo_teatro(largura, altura);
    desenhar_fundo_musica(midia, largura, altura);
    desenhar_corpo_pista(&pista);
    desenhar_receptores(&pista, escala, teclas_seguradas);
    desenhar_notas(&pista, escala, partida);
    desenhar_medidor(largura, altura, escala, partida->desempenho.valor);
    desenhar_painel_informacoes(largura, altura, escala, partida, musica);

    if (partida->tempo_feedback > 0.0f) {
        desenhar_texto_centralizado("PERFEITO!", largura / 2.0f, pista.y_alvo - escalar(75, escala),
                                    fonte(30, escala), COR_DOURADO_BRILHO);
    }
}

/* ================= Pausa e Game Over ================= */

AcaoMenu desenhar_menu_pausa(int largura, int altura, Vector2 mouse, bool game_over, int *foco) {
    float escala = calcular_escala(largura, altura);
    navegar_menu(foco, game_over ? 3 : 4);
    DrawRectangle(0, 0, largura, altura, (Color){ 0, 0, 0, 180 });

    float largura_painel = (float)escalar(360, escala);
    float altura_painel = (float)escalar(350, escala);
    Rectangle painel = { (largura - largura_painel) / 2.0f, (altura - altura_painel) / 2.0f,
                         largura_painel, altura_painel };
    desenhar_painel(painel, COR_DOURADO);
    desenhar_texto_centralizado(game_over ? "GAME OVER" : "PAUSADO", largura / 2.0f,
                                painel.y + escalar(20, escala), fonte(30, escala), COR_DOURADO);
    if (game_over) {
        const char *motivo = "Voce perdeu por excesso de erros.";
        int tamanho = fonte_que_cabe(motivo, fonte(17, escala), largura_painel - escalar(20, escala));
        desenhar_texto_centralizado(motivo, largura / 2.0f, painel.y + escalar(68, escala), tamanho, WHITE);
    }

    /* No Game Over nao existe CONTINUAR, entao a lista comeca no segundo botao. */
    const char *opcoes[] = { "CONTINUAR", "REINICIAR", "VOLTAR A SELECAO", "MENU INICIAL" };
    const AcaoMenu acoes[] = { ACAO_CONTINUAR, ACAO_REINICIAR, ACAO_VOLTAR_SELECAO, ACAO_VOLTAR_MENU };
    for (int i = game_over ? 1 : 0; i < 4; i++) {
        Rectangle botao = { painel.x + escalar(20, escala), painel.y + escalar(75 + i * 62, escala),
                            largura_painel - escalar(40, escala), (float)escalar(50, escala) };
        int indice_foco = game_over ? i - 1 : i;
        if (desenhar_botao_navegavel(botao, opcoes[i], mouse, fonte(22, escala), foco, indice_foco)) {
            return acoes[i];
        }
    }
    return ACAO_NENHUMA;
}

/* ================= Registro do nome no ranking ================= */

void desenhar_tela_registro_ranking(int largura, int altura, const Musica *musica,
                                    int pontuacao, const char *nome_digitado) {
    float escala = calcular_escala(largura, altura);
    desenhar_fundo_teatro(largura, altura);

    float largura_painel = (float)escalar(600, escala);
    float altura_painel = (float)escalar(380, escala);
    Rectangle painel = { (largura - largura_painel) / 2.0f, (altura - altura_painel) / 2.0f,
                         largura_painel, altura_painel };
    desenhar_painel(painel, COR_DOURADO);
    float centro_x = largura / 2.0f;
    float largura_texto = largura_painel - escalar(40, escala);

    desenhar_texto_centralizado("VITORIA!", centro_x, painel.y + escalar(24, escala),
                                fonte(40, escala), COR_DOURADO_BRILHO);
    int fonte_titulo = fonte_que_cabe(musica->titulo, fonte(24, escala), largura_texto);
    desenhar_texto_centralizado(musica->titulo, centro_x, painel.y + escalar(80, escala), fonte_titulo, COR_DOURADO);

    char texto_pontos[48];
    snprintf(texto_pontos, sizeof(texto_pontos), "PONTUACAO: %d", pontuacao);
    desenhar_texto_centralizado(texto_pontos, centro_x, painel.y + escalar(120, escala), fonte(26, escala), WHITE);

    desenhar_texto_centralizado("Digite seu nome para o ranking:", centro_x, painel.y + escalar(176, escala),
                                fonte(20, escala), COR_TEXTO_SUAVE);

    Rectangle caixa = { centro_x - escalar(210, escala), painel.y + escalar(210, escala),
                        (float)escalar(420, escala), (float)escalar(56, escala) };
    DrawRectangleRec(caixa, COR_BOTAO_FUNDO);
    DrawRectangleLinesEx(caixa, 3.0f, COR_DOURADO_BRILHO);

    /* O "_" pisca duas vezes por segundo para indicar onde o texto entra. */
    char texto_caixa[TAMANHO_NOME + 2];
    bool cursor_visivel = fmod(GetTime(), 1.0) < 0.5;
    snprintf(texto_caixa, sizeof(texto_caixa), "%s%s", nome_digitado, cursor_visivel ? "_" : " ");
    int fonte_nome = fonte(28, escala);
    desenhar_texto_centralizado(texto_caixa, centro_x, caixa.y + (caixa.height - fonte_nome) / 2.0f,
                                fonte_nome, WHITE);

    desenhar_texto_centralizado("[ENTER] salvar     [ESC] pular", centro_x, painel.y + escalar(292, escala),
                                fonte(18, escala), COR_DOURADO);
    desenhar_texto_centralizado("Ate 15 caracteres: letras, numeros, espaco, - ou _", centro_x,
                                painel.y + escalar(324, escala),
                                fonte_que_cabe("Ate 15 caracteres: letras, numeros, espaco, - ou _",
                                               fonte(15, escala), largura_texto),
                                COR_TEXTO_SUAVE);
}

/* ================= Resultados ================= */

static void desenhar_lista_ranking(Rectangle area, float escala, const Ranking *ranking, int posicao_destaque) {
    DrawText("RANKING DA MUSICA", (int)area.x, (int)area.y, fonte(20, escala), COR_DOURADO_BRILHO);
    int tamanho = fonte(17, escala);
    float altura_linha = (float)escalar(24, escala);
    float y = area.y + escalar(34, escala);

    if (ranking->quantidade == 0) {
        DrawText("Nenhuma pontuacao salva ainda.", (int)area.x, (int)y, tamanho, COR_TEXTO_SUAVE);
        return;
    }
    for (int i = 0; i < ranking->quantidade; i++) {
        const RegistroRanking *registro = &ranking->registros[i];
        float y_linha = y + i * altura_linha;
        Color cor = WHITE;
        if (i == posicao_destaque) {
            DrawRectangle((int)area.x - 4, (int)y_linha - 2, (int)area.width + 8, (int)altura_linha,
                          (Color){ 110, 40, 125, 250 });
            cor = COR_DOURADO_BRILHO;
        }
        char texto_nome[32];
        snprintf(texto_nome, sizeof(texto_nome), "%2d. %s", i + 1, registro->nome);
        DrawText(texto_nome, (int)area.x, (int)y_linha, tamanho, cor);
        char texto_pontos[16];
        snprintf(texto_pontos, sizeof(texto_pontos), "%d", registro->pontuacao);
        DrawText(texto_pontos, (int)(area.x + area.width - MeasureText(texto_pontos, tamanho)),
                 (int)y_linha, tamanho, cor);
    }
}

AcaoMenu desenhar_tela_resultados(int largura, int altura, Vector2 mouse,
                                  const Musica *musica, const Partida *partida,
                                  const Ranking *ranking, int posicao_ranking,
                                  const char *aviso_ranking, int *foco) {
    float escala = calcular_escala(largura, altura);
    navegar_menu(foco, 2);
    desenhar_fundo_teatro(largura, altura);

    float largura_painel = (float)escalar(900, escala);
    if (largura_painel > largura * 0.94f) largura_painel = largura * 0.94f;
    float altura_painel = (float)escalar(660, escala);
    if (altura_painel > altura * 0.94f) altura_painel = altura * 0.94f;
    Rectangle painel = { (largura - largura_painel) / 2.0f, (altura - altura_painel) / 2.0f,
                         largura_painel, altura_painel };
    desenhar_painel(painel, COR_DOURADO);
    float centro_x = largura / 2.0f;

    desenhar_texto_centralizado("VITORIA", centro_x, painel.y + escalar(18, escala), fonte(38, escala), COR_DOURADO_BRILHO);
    desenhar_texto_centralizado("PARTIDA CONCLUIDA", centro_x, painel.y + escalar(62, escala), fonte(18, escala), WHITE);
    int fonte_titulo = fonte_que_cabe(musica->titulo, fonte(24, escala), largura_painel - escalar(40, escala));
    desenhar_texto_centralizado(musica->titulo, centro_x, painel.y + escalar(86, escala), fonte_titulo, COR_DOURADO);
    float y_linha = painel.y + escalar(122, escala);
    DrawLine((int)(painel.x + escalar(30, escala)), (int)y_linha,
             (int)(painel.x + largura_painel - escalar(30, escala)), (int)y_linha, COR_LINHA_DOURADA);

    /* Duas colunas: desempenho a esquerda, ranking a direita. */
    float margem = (float)escalar(30, escala);
    float largura_coluna = (largura_painel - 3 * margem) / 2.0f;
    float x_esquerda = painel.x + margem;
    float x_direita = x_esquerda + largura_coluna + margem;
    float y_colunas = painel.y + escalar(136, escala);

    DrawText("SEU DESEMPENHO", (int)x_esquerda, (int)y_colunas, fonte(20, escala), COR_DOURADO_BRILHO);
    const char *rotulos[] = { "PONTUACAO", "ACERTOS", "NOTAS PERDIDAS", "MAIOR COMBO" };
    int valores[] = { partida->pontuacao, partida->notas_acertadas, partida->notas_perdidas, partida->maior_combo };
    for (int i = 0; i < 4; i++) {
        char texto[64];
        snprintf(texto, sizeof(texto), "%s: %d", rotulos[i], valores[i]);
        int tamanho = fonte_que_cabe(texto, fonte(20, escala), largura_coluna);
        DrawText(texto, (int)x_esquerda, (int)(y_colunas + escalar(36 + i * 32, escala)), tamanho, WHITE);
    }
    if (aviso_ranking != NULL && aviso_ranking[0] != '\0') {
        int tamanho = fonte_que_cabe(aviso_ranking, fonte(19, escala), largura_coluna);
        DrawText(aviso_ranking, (int)x_esquerda, (int)(y_colunas + escalar(184, escala)), tamanho, COR_DOURADO);
    }

    Rectangle area_ranking = { x_direita, y_colunas, largura_coluna, (float)escalar(280, escala) };
    desenhar_lista_ranking(area_ranking, escala, ranking, posicao_ranking);

    float altura_botao = (float)escalar(48, escala);
    float y_botoes = painel.y + altura_painel - altura_botao - escalar(18, escala);
    Rectangle jogar_novamente = { x_esquerda, y_botoes, largura_coluna, altura_botao };
    Rectangle voltar_catalogo = { x_direita, y_botoes, largura_coluna, altura_botao };
    AcaoMenu acao = ACAO_NENHUMA;
    if (desenhar_botao_navegavel(jogar_novamente, "JOGAR NOVAMENTE", mouse, fonte(22, escala), foco, 0)) {
        acao = ACAO_REINICIAR;
    }
    if (desenhar_botao_navegavel(voltar_catalogo, "VOLTAR AO CATALOGO", mouse, fonte(22, escala), foco, 1)) {
        acao = ACAO_VOLTAR_SELECAO;
    }
    desenhar_texto_centralizado("[ESC] Voltar ao catalogo", centro_x, altura - escalar(22, escala),
                                fonte(16, escala), (Color){ 190, 180, 200, 220 });
    return acao;
}

/* ================= Ranking pelo catalogo ================= */

AcaoMenu desenhar_tela_ranking_musica(int largura, int altura, Vector2 mouse,
                                      const Musica *musica, const Ranking *ranking,
                                      const char *aviso_ranking, int *foco) {
    float escala = calcular_escala(largura, altura);
    navegar_menu(foco, 1);
    desenhar_fundo_teatro(largura, altura);

    float largura_painel = (float)escalar(560, escala);
    if (largura_painel > largura * 0.94f) largura_painel = largura * 0.94f;
    float altura_painel = (float)escalar(520, escala);
    Rectangle painel = { (largura - largura_painel) / 2.0f, (altura - altura_painel) / 2.0f,
                         largura_painel, altura_painel };
    desenhar_painel(painel, COR_DOURADO);
    float centro_x = largura / 2.0f;
    float largura_texto = largura_painel - escalar(60, escala);

    int fonte_titulo = fonte_que_cabe(musica->titulo, fonte(34, escala), largura_texto);
    desenhar_texto_centralizado(musica->titulo, centro_x, painel.y + escalar(20, escala),
                                fonte_titulo, COR_DOURADO);
    int fonte_filme = fonte_que_cabe(musica->filme, fonte(18, escala), largura_texto);
    desenhar_texto_centralizado(musica->filme, centro_x, painel.y + escalar(62, escala),
                                fonte_filme, COR_TEXTO_SUAVE);
    float y_linha = painel.y + escalar(92, escala);
    DrawLine((int)(painel.x + escalar(30, escala)), (int)y_linha,
             (int)(painel.x + largura_painel - escalar(30, escala)), (int)y_linha, COR_LINHA_DOURADA);

    /* Sem destaque (-1): aqui so se consulta o ranking. */
    Rectangle area = { painel.x + escalar(30, escala), painel.y + escalar(106, escala),
                       largura_texto, (float)escalar(280, escala) };
    desenhar_lista_ranking(area, escala, ranking, -1);

    if (aviso_ranking != NULL && aviso_ranking[0] != '\0') {
        int tamanho = fonte_que_cabe(aviso_ranking, fonte(16, escala), largura_texto);
        desenhar_texto_centralizado(aviso_ranking, centro_x, painel.y + escalar(392, escala), tamanho, RED);
    }

    float largura_botao = (float)escalar(300, escala);
    float altura_botao = (float)escalar(48, escala);
    Rectangle voltar = { centro_x - largura_botao / 2.0f,
                         painel.y + altura_painel - altura_botao - escalar(20, escala),
                         largura_botao, altura_botao };
    if (desenhar_botao_navegavel(voltar, "VOLTAR AO CATALOGO", mouse, fonte(22, escala), foco, 0)) {
        return ACAO_VOLTAR_SELECAO;
    }
    desenhar_texto_centralizado("[ESC] Voltar ao catalogo", centro_x, altura - escalar(22, escala),
                                fonte(16, escala), (Color){ 190, 180, 200, 220 });
    return ACAO_NENHUMA;
}

/* ================= Confirmacao de saida ================= */

AcaoMenu desenhar_confirmacao_saida(int largura, int altura, Vector2 mouse, int *foco) {
    float escala = calcular_escala(largura, altura);
    navegar_menu(foco, 2);
    DrawRectangle(0, 0, largura, altura, (Color){ 0, 0, 0, 180 });

    float largura_painel = (float)escalar(380, escala);
    float altura_painel = (float)escalar(210, escala);
    Rectangle painel = { (largura - largura_painel) / 2.0f, (altura - altura_painel) / 2.0f,
                         largura_painel, altura_painel };
    desenhar_painel(painel, COR_DOURADO);
    desenhar_texto_centralizado("Deseja sair do jogo?", largura / 2.0f, painel.y + escalar(35, escala),
                                fonte(26, escala), COR_DOURADO);

    Rectangle botao_sim = { painel.x + escalar(25, escala), painel.y + escalar(110, escala),
                            (float)escalar(150, escala), (float)escalar(55, escala) };
    Rectangle botao_nao = { painel.x + escalar(205, escala), painel.y + escalar(110, escala),
                            (float)escalar(150, escala), (float)escalar(55, escala) };
    if (desenhar_botao_navegavel(botao_sim, "SIM", mouse, fonte(24, escala), foco, 0)) {
        return ACAO_CONFIRMAR_SAIDA;
    }
    if (desenhar_botao_navegavel(botao_nao, "NAO", mouse, fonte(24, escala), foco, 1)) {
        return ACAO_CANCELAR_SAIDA;
    }
    return ACAO_NENHUMA;
}

void desenhar_mensagem_erro(int largura, int altura, const char *mensagem) {
    if (mensagem == NULL || mensagem[0] == '\0') {
        return;
    }
    float escala = calcular_escala(largura, altura);
    int tamanho = fonte_que_cabe(mensagem, fonte(16, escala), largura - escalar(40, escala));
    int y = altura - escalar(58, escala);
    DrawRectangle(0, y - 4, largura, tamanho + 8, (Color){ 0, 0, 0, 200 });
    DrawText(mensagem, escalar(20, escala), y, tamanho, RED);
}