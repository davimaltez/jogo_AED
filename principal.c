#include "raylib.h"
#include "catalogo_musicas.h"
#include <stdbool.h>
#include <stdio.h>

/*
 * Estados possiveis para a navegacao entre telas.
 * Cada valor representa uma tela visivel do jogo.
 */
typedef enum {
    TELA_MENU_PRINCIPAL,
    TELA_SELECAO_MUSICAS,
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

#define COR_PAINEL_FUNDO      (Color){ 28, 14, 38, 235 }

/*
 * Desenha o fundo visual do jogo com elementos de um palco de teatro:
 * cortinas sutis nas laterais e feixes de luz cruzando a tela.
 */
void desenhar_fundo_teatro(int largura, int altura) {
    ClearBackground(COR_FUNDO_ESCURO);

    /* Faixas laterais simulando cortinas de palco */
    int largura_cortina = largura * 0.08f;
    DrawRectangle(0, 0, largura_cortina, altura, COR_PALCO_LATERAL);
    DrawRectangle(largura - largura_cortina, 0, largura_cortina, altura, COR_PALCO_LATERAL);

    /* Linhas divisorias verticais douradas discretas */
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
bool desenhar_botao(Rectangle retangulo, const char *texto, Vector2 mouse, int tamanho_fonte) {
    bool mouse_em_cima = CheckCollisionPointRec(mouse, retangulo);
    bool clicado = mouse_em_cima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color cor_fundo = mouse_em_cima ? COR_BOTAO_FOCO_FUNDO : COR_BOTAO_FUNDO;
    Color cor_borda = mouse_em_cima ? COR_BOTAO_FOCO_BORDA : COR_BOTAO_BORDA;
    Color cor_texto = mouse_em_cima ? COR_BOTAO_FOCO_TEXTO : COR_BOTAO_TEXTO;

    DrawRectangleRec(retangulo, cor_fundo);

    float espessura_borda = mouse_em_cima ? 3.0f : 2.0f;
    DrawRectangleLinesEx(retangulo, espessura_borda, cor_borda);

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

    /* Titulo principal: FINAL ACT cinematografico */
    const char *titulo = "FINAL ACT";
    int tamanho_titulo = altura * 0.13f;
    if (tamanho_titulo < 64) tamanho_titulo = 64;
    if (tamanho_titulo > 110) tamanho_titulo = 110;
    int largura_titulo = MeasureText(titulo, tamanho_titulo);
    int posicao_x_titulo = (largura - largura_titulo) / 2;
    int posicao_y_titulo = altura * 0.16f;

    DrawText(titulo, posicao_x_titulo + 4, posicao_y_titulo + 4, tamanho_titulo, COR_SOMBRA_TEXTO);
    DrawText(titulo, posicao_x_titulo, posicao_y_titulo, tamanho_titulo, COR_DOURADO);

    /* Subtitulo */
    const char *subtitulo = "UM JOGO DE RITMO EM CENA";
    int tamanho_subtitulo = 22;
    int largura_subtitulo = MeasureText(subtitulo, tamanho_subtitulo);
    int posicao_x_subtitulo = (largura - largura_subtitulo) / 2;
    int posicao_y_subtitulo = posicao_y_titulo + tamanho_titulo + 14;
    DrawText(subtitulo, posicao_x_subtitulo, posicao_y_subtitulo, tamanho_subtitulo, COR_DOURADO_BRILHO);

    /* Medidas proporcionais dos botoes */
    float largura_botao = largura * 0.38f;
    if (largura_botao < 280.0f) largura_botao = 280.0f;
    if (largura_botao > 440.0f) largura_botao = 440.0f;
    float altura_botao = 60.0f;
    float posicao_x_botao = (largura - largura_botao) / 2;
    float inicio_y_botoes = altura * 0.44f;
    float espaco_entre_botoes = 22.0f;

    /* Botao JOGAR */
    Rectangle ret_jogar = { posicao_x_botao, inicio_y_botoes, largura_botao, altura_botao };
    if (desenhar_botao(ret_jogar, "JOGAR", mouse, 26)) {
        *estado = TELA_SELECAO_MUSICAS;
    }

    /* Botao COMO JOGAR */
    Rectangle ret_como_jogar = { posicao_x_botao, inicio_y_botoes + (altura_botao + espaco_entre_botoes), largura_botao, altura_botao };
    if (desenhar_botao(ret_como_jogar, "COMO JOGAR", mouse, 26)) {
        *estado = TELA_COMO_JOGAR;
    }

    /* Botao SAIR */
    Rectangle ret_sair = { posicao_x_botao, inicio_y_botoes + 2 * (altura_botao + espaco_entre_botoes), largura_botao, altura_botao };
    if (desenhar_botao(ret_sair, "SAIR", mouse, 26)) {
        *deve_sair = true;
    }
}

/*
 * Desenha um botao compacto para a selecao de filtros de ordenacao.
 * Retorna true caso o botao tenha sido clicado.
 */
bool desenhar_botao_filtro(Rectangle retangulo, const char *texto, bool ativo, Vector2 mouse) {
    bool mouse_em_cima = CheckCollisionPointRec(mouse, retangulo);
    bool clicado = mouse_em_cima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color cor_fundo = ativo ? (Color){ 95, 35, 110, 240 } : (mouse_em_cima ? COR_BOTAO_FOCO_FUNDO : COR_BOTAO_FUNDO);
    Color cor_borda = ativo ? COR_DOURADO : (mouse_em_cima ? COR_BOTAO_FOCO_BORDA : COR_BOTAO_BORDA);
    Color cor_texto = ativo ? COR_DOURADO_BRILHO : (mouse_em_cima ? COR_BOTAO_FOCO_TEXTO : COR_BOTAO_TEXTO);

    DrawRectangleRec(retangulo, cor_fundo);
    DrawRectangleLinesEx(retangulo, ativo ? 2.5f : 1.5f, cor_borda);

    int tamanho_fonte = 19;
    int largura_texto = MeasureText(texto, tamanho_fonte);
    int pos_x = retangulo.x + (retangulo.width - largura_texto) / 2;
    int pos_y = retangulo.y + (retangulo.height - tamanho_fonte) / 2;

    DrawText(texto, pos_x, pos_y, tamanho_fonte, cor_texto);

    return clicado;
}

/*
 * Desenha a tela de selecao de musicas do catalogo.
 * Exibe a lista de musicas, opcoes de ordenacao por Merge Sort,
 * detalhes da faixa selecionada e validacao de escolha.
 */
void desenhar_tela_selecao_musicas(int largura, int altura, Vector2 mouse, EstadoTela *estado,
                                   Musica catalogo[], int total_musicas,
                                   int *indice_selecionado, bool *confirmado,
                                   CriterioOrdenacao *criterio_atual) {
    desenhar_fundo_teatro(largura, altura);

    /* Titulo da secao */
    const char *titulo = "SELECAO DE MUSICAS";
    int tamanho_titulo = 36;
    int largura_titulo = MeasureText(titulo, tamanho_titulo);
    DrawText(titulo, (largura - largura_titulo) / 2, altura * 0.04f, tamanho_titulo, COR_DOURADO);

    /* Area principal dividida em duas colunas */
    float margem_x = largura * 0.06f;
    float topo_y = altura * 0.11f;
    float largura_util = largura - (2 * margem_x);
    float altura_paineis = altura * 0.76f;

    float largura_coluna_esquerda = largura_util * 0.485f;
    float largura_coluna_direita = largura_util * 0.485f;
    float espaco_colunas = largura_util * 0.03f;

    float x_coluna_esquerda = margem_x;
    float x_coluna_direita = margem_x + largura_coluna_esquerda + espaco_colunas;

    /* ---- COLUNA ESQUERDA: LISTA DE MUSICAS E FILTROS ---- */
    Rectangle painel_lista = { x_coluna_esquerda, topo_y, largura_coluna_esquerda, altura_paineis };
    DrawRectangleRec(painel_lista, COR_PAINEL_FUNDO);
    DrawRectangleLinesEx(painel_lista, 2.0f, (Color){ 140, 100, 50, 200 });

    DrawText("ORDENAR CATALOGO (MERGE SORT)", x_coluna_esquerda + 18, topo_y + 12, 20, COR_DOURADO_BRILHO);

    /* Botoes de selecao do criterio de ordenacao */
    float largura_botao_filtro = (largura_coluna_esquerda - 40.0f) / 3.0f;
    float altura_botao_filtro = 46.0f;
    float pos_y_filtros = topo_y + 40.0f;

    Rectangle ret_filtro_az = { x_coluna_esquerda + 15, pos_y_filtros, largura_botao_filtro, altura_botao_filtro };
    Rectangle ret_filtro_cresc = { x_coluna_esquerda + 15 + largura_botao_filtro + 5, pos_y_filtros, largura_botao_filtro, altura_botao_filtro };
    Rectangle ret_filtro_decr = { x_coluna_esquerda + 15 + 2 * (largura_botao_filtro + 5), pos_y_filtros, largura_botao_filtro, altura_botao_filtro };

    CriterioOrdenacao novo_criterio = *criterio_atual;
    bool trocou_criterio = false;

    if (desenhar_botao_filtro(ret_filtro_az, "Titulo A-Z", (*criterio_atual == ORDEM_TITULO_AZ), mouse)) {
        if (*criterio_atual != ORDEM_TITULO_AZ) {
            novo_criterio = ORDEM_TITULO_AZ;
            trocou_criterio = true;
        }
    }

    if (desenhar_botao_filtro(ret_filtro_cresc, "Dif. Cresc.", (*criterio_atual == ORDEM_DIFICULDADE_CRESCENTE), mouse)) {
        if (*criterio_atual != ORDEM_DIFICULDADE_CRESCENTE) {
            novo_criterio = ORDEM_DIFICULDADE_CRESCENTE;
            trocou_criterio = true;
        }
    }

    if (desenhar_botao_filtro(ret_filtro_decr, "Dif. Decr.", (*criterio_atual == ORDEM_DIFICULDADE_DECRESCENTE), mouse)) {
        if (*criterio_atual != ORDEM_DIFICULDADE_DECRESCENTE) {
            novo_criterio = ORDEM_DIFICULDADE_DECRESCENTE;
            trocou_criterio = true;
        }
    }

    /* Se o usuario clicou em um novo filtro, reordena preservando a musica selecionada */
    if (trocou_criterio) {
        char titulo_selecionado[64] = "";
        if (*indice_selecionado >= 0 && *indice_selecionado < total_musicas) {
            strncpy(titulo_selecionado, catalogo[*indice_selecionado].titulo, sizeof(titulo_selecionado) - 1);
            titulo_selecionado[sizeof(titulo_selecionado) - 1] = '\0';
        }

        *criterio_atual = novo_criterio;
        ordenar_catalogo(catalogo, total_musicas, *criterio_atual);

        /* Localiza a nova posicao da musica no vetor ordenado */
        if (titulo_selecionado[0] != '\0') {
            int novo_indice = buscar_indice_por_titulo(catalogo, total_musicas, titulo_selecionado);
            if (novo_indice != -1) {
                *indice_selecionado = novo_indice;
            }
        }
    }

    DrawLine(x_coluna_esquerda + 15, pos_y_filtros + altura_botao_filtro + 10, x_coluna_esquerda + largura_coluna_esquerda - 15, pos_y_filtros + altura_botao_filtro + 10, (Color){ 120, 90, 40, 150 });

    /* Distribuicao dinamica e equilibrada dos 5 cartoes de musica */
    float inicio_y_itens = pos_y_filtros + altura_botao_filtro + 18.0f;
    float espaco_disponivel_itens = altura_paineis - (inicio_y_itens - topo_y) - 15.0f;
    
    float altura_item = espaco_disponivel_itens * 0.165f;
    if (altura_item < 64.0f) altura_item = 64.0f;
    if (altura_item > 92.0f) altura_item = 92.0f;

    float espaco_entre_itens = (espaco_disponivel_itens - (total_musicas * altura_item)) / (total_musicas - 1);
    if (espaco_entre_itens < 6.0f) espaco_entre_itens = 6.0f;
    if (espaco_entre_itens > 18.0f) espaco_entre_itens = 18.0f;

    for (int i = 0; i < total_musicas; i++) {
        Rectangle ret_item = {
            x_coluna_esquerda + 15,
            inicio_y_itens + (i * (altura_item + espaco_entre_itens)),
            largura_coluna_esquerda - 30,
            altura_item
        };

        bool mouse_em_cima = CheckCollisionPointRec(mouse, ret_item);
        bool esta_selecionada = (*indice_selecionado == i);

        /* Se o usuario clicar na musica da lista, atualiza a selecao */
        if (mouse_em_cima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            *indice_selecionado = i;
            *confirmado = false;
        }

        /* Cores do item de acordo com o estado */
        Color cor_fundo_item = COR_BOTAO_FUNDO;
        Color cor_borda_item = COR_BOTAO_BORDA;

        if (esta_selecionada) {
            cor_fundo_item = (Color){ 110, 40, 125, 250 };
            cor_borda_item = COR_DOURADO;
        } else if (mouse_em_cima) {
            cor_fundo_item = COR_BOTAO_FOCO_FUNDO;
            cor_borda_item = COR_BOTAO_FOCO_BORDA;
        }

        DrawRectangleRec(ret_item, cor_fundo_item);
        DrawRectangleLinesEx(ret_item, esta_selecionada ? 3.5f : 1.5f, cor_borda_item);

        /* Barra vertical dourada marcante na lateral esquerda do item selecionado */
        float offset_texto_x = 14.0f;
        if (esta_selecionada) {
            DrawRectangle(ret_item.x, ret_item.y, 8, ret_item.height, COR_DOURADO);
            offset_texto_x = 22.0f;
        }

        /* Titulo da musica com fonte expressiva (26px) */
        float pos_y_titulo = ret_item.y + (altura_item * 0.16f);
        DrawText(catalogo[i].titulo, ret_item.x + offset_texto_x, pos_y_titulo, 26, (esta_selecionada ? COR_DOURADO : WHITE));

        /* Filme de origem e dificuldade (19px) */
        char info_sub[96];
        snprintf(info_sub, sizeof(info_sub), "%s   *   Dificuldade: Nivel %d", catalogo[i].filme, catalogo[i].dificuldade);
        float pos_y_sub = ret_item.y + (altura_item * 0.54f);
        DrawText(info_sub, ret_item.x + offset_texto_x, pos_y_sub, 19, (Color){ 215, 205, 225, 230 });
    }

    /* ---- COLUNA DIREITA: DETALHES DA MUSICA SELECIONADA ---- */
    Rectangle painel_detalhes = { x_coluna_direita, topo_y, largura_coluna_direita, altura_paineis };
    DrawRectangleRec(painel_detalhes, COR_PAINEL_FUNDO);
    DrawRectangleLinesEx(painel_detalhes, 2.0f, (Color){ 140, 100, 50, 200 });

    DrawText("DETALHES DA FAIXA", x_coluna_direita + 20, topo_y + 12, 22, COR_DOURADO_BRILHO);
    DrawLine(x_coluna_direita + 20, topo_y + 42, x_coluna_direita + largura_coluna_direita - 20, topo_y + 42, (Color){ 120, 90, 40, 150 });

    /* Validacao da selecao */
    if (*indice_selecionado >= 0 && *indice_selecionado < total_musicas) {
        Musica musica = catalogo[*indice_selecionado];

        /* Título */
        DrawText("Titulo:", x_coluna_direita + 25, topo_y + 54, 19, (Color){ 195, 185, 205, 255 });
        DrawText(musica.titulo, x_coluna_direita + 25, topo_y + 78, 36, COR_DOURADO);

        /* Filme */
        DrawText("Filme Musical:", x_coluna_direita + 25, topo_y + 130, 19, (Color){ 195, 185, 205, 255 });
        DrawText(musica.filme, x_coluna_direita + 25, topo_y + 154, 25, WHITE);

        /* Nivel de Dificuldade */
        DrawText("Dificuldade:", x_coluna_direita + 25, topo_y + 200, 19, (Color){ 195, 185, 205, 255 });

        char texto_nivel[32];
        snprintf(texto_nivel, sizeof(texto_nivel), "Nivel %d de 5", musica.dificuldade);
        DrawText(texto_nivel, x_coluna_direita + 25, topo_y + 224, 22, COR_DOURADO_BRILHO);

        /* Indicador visual com blocos para a dificuldade */
        for (int d = 1; d <= 5; d++) {
            Rectangle ret_dif = { x_coluna_direita + 25 + (d - 1) * 44, topo_y + 258, 36, 20 };
            if (d <= musica.dificuldade) {
                DrawRectangleRec(ret_dif, COR_DOURADO);
            } else {
                DrawRectangleRec(ret_dif, (Color){ 60, 40, 70, 200 });
                DrawRectangleLinesEx(ret_dif, 1.0f, (Color){ 120, 90, 130, 200 });
            }
        }

        /* Botao de confirmacao SELECIONAR */
        float largura_botao_sel = largura_coluna_direita - 50;
        Rectangle ret_selecionar = { x_coluna_direita + 25, topo_y + 300, largura_botao_sel, 56 };

        if (desenhar_botao(ret_selecionar, "SELECIONAR", mouse, 24)) {
            *confirmado = true;
        }

        /* Mensagem apos confirmacao */
        if (*confirmado) {
            Rectangle ret_msg = { x_coluna_direita + 20, topo_y + 375, largura_coluna_direita - 40, 86 };
            DrawRectangleRec(ret_msg, (Color){ 20, 45, 30, 240 });
            DrawRectangleLinesEx(ret_msg, 2.0f, (Color){ 80, 200, 120, 255 });

            DrawText("Musica escolhida com sucesso!", ret_msg.x + 16, ret_msg.y + 16, 21, (Color){ 140, 255, 170, 255 });
            DrawText("A partida sera iniciada na proxima etapa.", ret_msg.x + 16, ret_msg.y + 46, 18, WHITE);
        }
    } else {
        /* Mensagem orientando o jogador a selecionar */
        const char *aviso = "Clique em uma musica da lista ao lado\npara visualizar seus detalhes.";
        DrawText(aviso, x_coluna_direita + 25, topo_y + 130, 20, (Color){ 200, 190, 210, 200 });
    }

    /* ---- BOTAO VOLTAR AO MENU ---- */
    float largura_voltar = 260.0f;
    float altura_voltar = 50.0f;
    Rectangle ret_voltar = { (largura - largura_voltar) / 2, altura * 0.89f, largura_voltar, altura_voltar };

    if (desenhar_botao(ret_voltar, "VOLTAR AO MENU", mouse, 24)) {
        *estado = TELA_MENU_PRINCIPAL;
    }
}

/*
 * Renderiza a tela explicativa de instrucoes do jogo.
 */
void desenhar_tela_como_jogar(int largura, int altura, Vector2 mouse, EstadoTela *estado) {
    desenhar_fundo_teatro(largura, altura);

    /* Painel centralizado amplo */
    float largura_quadro = largura * 0.82f;
    float altura_quadro = altura * 0.72f;
    float quadro_x = (largura - largura_quadro) / 2;
    float quadro_y = altura * 0.12f;

    Rectangle quadro = { quadro_x, quadro_y, largura_quadro, altura_quadro };
    DrawRectangleRec(quadro, (Color){ 28, 14, 38, 235 });
    DrawRectangleLinesEx(quadro, 2.0f, COR_DOURADO);

    /* Titulo */
    const char *titulo = "COMO JOGAR";
    int tamanho_titulo = 36;
    int largura_titulo = MeasureText(titulo, tamanho_titulo);
    DrawText(titulo, quadro_x + (largura_quadro - largura_titulo) / 2, quadro_y + 24, tamanho_titulo, COR_DOURADO);

    DrawLine(quadro_x + 40, quadro_y + 68, quadro_x + largura_quadro - 40, quadro_y + 68, (Color){ 180, 140, 60, 160 });

    /* Instrucoes */
    const char *instrucao1 = "As notas musicais descerao pelas pistas da tela.";
    const char *instrucao2 = "Pressione a tecla correspondente quando a nota cruzar a zona de acerto:";
    int tamanho_texto = 23;

    DrawText(instrucao1, quadro_x + (largura_quadro - MeasureText(instrucao1, 24)) / 2, quadro_y + 88, 24, (Color){ 230, 220, 220, 255 });
    DrawText(instrucao2, quadro_x + (largura_quadro - MeasureText(instrucao2, tamanho_texto)) / 2, quadro_y + 122, tamanho_texto, (Color){ 230, 220, 220, 255 });

    /* Indicadores visuais das teclas D, F, J, K */
    const char *teclas[] = { "D", "F", "J", "K" };
    Color cores_teclas[] = {
        (Color){ 190, 60, 100, 255 },  /* Pista 1: Tom rosa/vinho */
        (Color){ 70, 120, 210, 255 },  /* Pista 2: Azul */
        (Color){ 220, 160, 40, 255 },  /* Pista 3: Dourado */
        (Color){ 60, 170, 120, 255 }   /* Pista 4: Verde */
    };

    float tamanho_tecla = 74.0f;
    float espaco_teclas = 24.0f;
    float largura_total_teclas = (4 * tamanho_tecla) + (3 * espaco_teclas);
    float inicio_x_teclas = quadro_x + (largura_quadro - largura_total_teclas) / 2;
    float posicao_y_teclas = quadro_y + 168;

    for (int i = 0; i < 4; i++) {
        Rectangle ret_tecla = { inicio_x_teclas + i * (tamanho_tecla + espaco_teclas), posicao_y_teclas, tamanho_tecla, tamanho_tecla };
        DrawRectangleRec(ret_tecla, cores_teclas[i]);
        DrawRectangleLinesEx(ret_tecla, 2.5f, (Color){ 255, 255, 255, 220 });

        int largura_letra = MeasureText(teclas[i], 36);
        int letra_x = ret_tecla.x + (tamanho_tecla - largura_letra) / 2;
        int letra_y = ret_tecla.y + (tamanho_tecla - 36) / 2;
        DrawText(teclas[i], letra_x, letra_y, 36, WHITE);
    }

    /* Informacoes adicionais sobre pontuacao e combo */
    const char *dica1 = "Acertos aumentam sua pontuacao e constroem sequencias de combo.";
    const char *dica2 = "Erros ou notas perdidas interrompem a contagem do combo!";
    int tamanho_dica = 20;
    DrawText(dica1, quadro_x + (largura_quadro - MeasureText(dica1, tamanho_dica)) / 2, quadro_y + 265, tamanho_dica, (Color){ 210, 200, 215, 255 });
    DrawText(dica2, quadro_x + (largura_quadro - MeasureText(dica2, tamanho_dica)) / 2, quadro_y + 295, tamanho_dica, (Color){ 210, 200, 215, 255 });

    /* Botao para voltar ao menu */
    float largura_botao = 260.0f;
    float altura_botao = 50.0f;
    Rectangle ret_voltar = { quadro_x + (largura_quadro - largura_botao) / 2, quadro_y + altura_quadro - 68, largura_botao, altura_botao };

    if (desenhar_botao(ret_voltar, "VOLTAR AO MENU", mouse, 24)) {
        *estado = TELA_MENU_PRINCIPAL;
    }
}

int main(void) {
    const int largura_inicial = 900;
    const int altura_inicial = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(largura_inicial, altura_inicial, "FINAL ACT - Jogo de Ritmo");
    SetTargetFPS(60);

    /* Inicia o jogo diretamente em tela cheia */
    ToggleFullscreen();

    /* Inicializacao do catalogo de musicas (vetor de structs) */
    Musica catalogo[MAX_MUSICAS];
    int total_musicas = 0;
    inicializar_catalogo(catalogo, &total_musicas);

    /* Ordena inicialmente o catalogo por Titulo A-Z */
    CriterioOrdenacao criterio_atual = ORDEM_TITULO_AZ;
    ordenar_catalogo(catalogo, total_musicas, criterio_atual);

    EstadoTela estado_atual = TELA_MENU_PRINCIPAL;
    int indice_musica_selecionada = 0;   /* Comeca com a primeira musica selecionada */
    bool musica_confirmada = false;
    bool deve_sair = false;

    /* Loop principal do jogo */
    while (!WindowShouldClose() && !deve_sair) {
        /* Alterna entre tela cheia e modo janela ao pressionar F11 */
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        int largura_atual = GetScreenWidth();
        int altura_atual = GetScreenHeight();
        Vector2 posicao_mouse = GetMousePosition();

        BeginDrawing();

        /* Controle de telas atraves do estado atual */
        switch (estado_atual) {
            case TELA_MENU_PRINCIPAL:
                desenhar_menu_principal(largura_atual, altura_atual, posicao_mouse, &estado_atual, &deve_sair);
                break;

            case TELA_SELECAO_MUSICAS:
                desenhar_tela_selecao_musicas(largura_atual, altura_atual, posicao_mouse, &estado_atual,
                                              catalogo, total_musicas,
                                              &indice_musica_selecionada, &musica_confirmada,
                                              &criterio_atual);
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
