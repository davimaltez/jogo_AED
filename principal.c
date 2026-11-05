#include "raylib.h"
#include "catalogo_musicas.h"
#include "lista_notas.h"
#include "desempenho.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/*
 * Estados possiveis para a navegacao entre telas.
 * Cada valor representa uma tela visivel do jogo.
 */
typedef enum {
    TELA_MENU_PRINCIPAL,
    TELA_SELECAO_MUSICAS,
    TELA_COMO_JOGAR,
    TELA_PARTIDA,
    TELA_RESULTADOS
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
void desenhar_menu_principal(int largura, int altura, Vector2 mouse, EstadoTela *estado, bool *confirmar_saida) {
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
        *confirmar_saida = true;
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
 * detalhes da faixa selecionada e botao para iniciar a partida.
 */
void desenhar_tela_selecao_musicas(int largura, int altura, Vector2 mouse, EstadoTela *estado,
                                   Musica catalogo[], int total_musicas,
                                   int *indice_selecionado, bool *confirmado,
                                   CriterioOrdenacao *criterio_atual,
                                   Nota **cabeca_notas, float *tempo_partida,
                                   int *pontuacao, int *combo, int *maior_combo,
                                   int *notas_acertadas, int *notas_perdidas,
                                   char *erro_carregamento, size_t tamanho_erro) {
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
            erro_carregamento[0] = '\0';
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

        /* Botao INICIAR PARTIDA */
        float largura_botao_sel = largura_coluna_direita - 50;
        Rectangle ret_selecionar = { x_coluna_direita + 25, topo_y + 300, largura_botao_sel, 56 };

        if (desenhar_botao(ret_selecionar, "INICIAR PARTIDA", mouse, 24)) {
            if (carregar_notas_arquivo(cabeca_notas, musica.caminho_notas,
                                      erro_carregamento, tamanho_erro)) {
                *confirmado = true;

                /* Zera cronometro e pontuacoes somente apos carregar tudo. */
                *tempo_partida = 0.0f;
                *pontuacao = 0;
                *combo = 0;
                *maior_combo = 0;
                *notas_acertadas = 0;
                *notas_perdidas = 0;

                /* Transita para a tela de partida */
                *estado = TELA_PARTIDA;
            } else {
                TraceLog(LOG_WARNING, "%s: %s", musica.caminho_notas, erro_carregamento);
            }
        }

        /* Dica explicativa */
        if (erro_carregamento[0] != '\0') {
            DrawText(erro_carregamento, x_coluna_direita + 25, topo_y + 365, 14, RED);
        } else {
            DrawText("Pressione INICIAR PARTIDA para jogar com a lista encadeada.", x_coluna_direita + 25, topo_y + 375, 16, (Color){ 200, 190, 210, 220 });
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

/* O semicirculo cresce do vermelho (esquerda) ao verde (direita). */
void desenhar_medidor(int largura, int altura, int valor) {
    float raio = largura * 0.085f;
    if (raio > 110.0f) raio = 110.0f;
    Vector2 centro = { largura * 0.11f, altura * 0.48f };
    const int segmentos = 100;
    for (int i = 0; i < segmentos; i++) {
        float angulo_inicio = PI + PI * i / segmentos;
        float angulo_fim = PI + PI * (i + 1) / segmentos;
        Vector2 inicio = { centro.x + cosf(angulo_inicio) * raio,
                           centro.y + sinf(angulo_inicio) * raio };
        Vector2 fim = { centro.x + cosf(angulo_fim) * raio,
                        centro.y + sinf(angulo_fim) * raio };
        Color cor = i < LIMITE_VERMELHO ? (Color){ 225, 65, 80, 255 } :
                    (i < LIMITE_VERDE ? COR_DOURADO : (Color){ 45, 210, 125, 255 });
        DrawLineEx(inicio, fim, raio * 0.15f, cor);
    }
    float angulo = PI + PI * valor / DESEMPENHO_MAXIMO;
    Vector2 ponta = { centro.x + cosf(angulo) * raio * 0.8f,
                      centro.y + sinf(angulo) * raio * 0.8f };
    DrawLineEx(centro, ponta, 3.0f, WHITE);
    DrawCircleV(centro, 5.0f, COR_DOURADO);
    int fonte = raio < 75.0f ? 12 : 16;
    const char *titulo = "DESEMPENHO";
    DrawText(titulo, centro.x - MeasureText(titulo, fonte) / 2, centro.y - raio - 32,
             fonte, COR_DOURADO);
    char texto[32];
    snprintf(texto, sizeof(texto), "%d / 100", valor);
    DrawText(texto, centro.x - MeasureText(texto, fonte) / 2, centro.y + 15, fonte, WHITE);
}

/* Gerencia a partida usando a lista de notas e a posicao calculada pelo tempo. */
void desenhar_tela_partida(int largura, int altura, bool pausada, Desempenho *desempenho,
                           Musica musica_atual, Nota **cabeca_notas,
                           float *tempo_partida, int *pontuacao, int *combo,
                           int *maior_combo, int *notas_acertadas, int *notas_perdidas,
                           float *tempo_feedback, int *pista_feedback) {
    desenhar_fundo_teatro(largura, altura);

    /* Atualiza o tempo decorrido da partida em segundos */
    bool bloqueada = pausada || desempenho->game_over;
    float delta = bloqueada ? 0.0f : GetFrameTime();
    *tempo_partida += delta;

    /* Atualiza feedback de acerto */
    if (*tempo_feedback > 0.0f) {
        *tempo_feedback -= delta;
    }

    /* Leitura das teclas de acerto D, F, G, J, K */
    int teclas[] = { KEY_D, KEY_F, KEY_G, KEY_J, KEY_K };

    for (int p = 0; p < TOTAL_PISTAS; p++) {
        if (!bloqueada && !desempenho->game_over && IsKeyPressed(teclas[p])) {
            /* Verifica se ha acerto na lista encadeada */
            if (verificar_acerto(*cabeca_notas, p, *tempo_partida, 0.20f)) {
                *pontuacao += 100 + (*combo * 10);
                (*combo)++;
                if (*combo > *maior_combo) {
                    *maior_combo = *combo;
                }
                (*notas_acertadas)++;
                registrar_desempenho(desempenho, true);
                *pista_feedback = p;
                *tempo_feedback = 0.30f;
            } else {
                *combo = 0;
                registrar_desempenho(desempenho, false);
            }
        }
    }

    /* Atualiza notas que ultrapassaram a linha sem acerto */
    if (!bloqueada && !desempenho->game_over) {
        int perdas_anteriores = *notas_perdidas;
        atualizar_notas(*cabeca_notas, *tempo_partida, 0.22f, combo, notas_perdidas);
        /* Cada nota perdida e penalizada uma unica vez, inclusive em acordes. */
        for (int i = perdas_anteriores; i < *notas_perdidas; i++) {
            registrar_desempenho(desempenho, false);
        }
    }

    /* Cores das 5 pistas (Guitar Hero) */
    Color cores_pistas[] = {
        (Color){ 225, 50, 110, 255 },  /* Pista 0: D (Rosa / Vinho) */
        (Color){ 50, 140, 245, 255 },  /* Pista 1: F (Azul Vibrante) */
        (Color){ 245, 195, 65, 255 },  /* Pista 2: G (Dourado Ouro) */
        (Color){ 45, 210, 125, 255 },  /* Pista 3: J (Verde Esmeralda) */
        (Color){ 180, 90, 245, 255 }   /* Pista 4: K (Roxo) */
    };

    /* 
     * Geometria em Perspectiva 3D (Guitar Hero):
     * A pista forma um trapezio que converge para o horizonte no topo da tela.
     */
    float centro_x = largura / 2.0f;
    float y_topo = altura * 0.16f;        /* Linha do horizonte da pista */
    float y_base = altura * 0.94f;        /* Fim da pista perto do jogador */
    float y_alvo = altura * 0.82f;        /* Linha exata de acerto */

    float largura_topo = largura * 0.20f; /* Pista estreita no horizonte */
    float largura_base = largura * 0.56f; /* Pista larga perto do jogador */

    /* Quatro vertices do trapezio da pista */
    Vector2 p_topo_esq = { centro_x - (largura_topo / 2.0f), y_topo };
    Vector2 p_topo_dir = { centro_x + (largura_topo / 2.0f), y_topo };
    Vector2 p_base_dir = { centro_x + (largura_base / 2.0f), y_base };
    Vector2 p_base_esq = { centro_x - (largura_base / 2.0f), y_base };

    /* Desenha o corpo da pista com dois triangulos */
    DrawTriangle(p_topo_esq, p_base_esq, p_base_dir, (Color){ 16, 9, 24, 235 });
    DrawTriangle(p_topo_esq, p_base_dir, p_topo_dir, (Color){ 16, 9, 24, 235 });

    /* Brilho suave no horizonte da pista */
    Vector2 centro_horizonte = { centro_x, y_topo };
    /* A raylib 5.5 recebe coordenadas inteiras; a 6 recebe Vector2. */
#if RAYLIB_VERSION_MAJOR >= 6
    DrawCircleGradient(centro_horizonte, largura_topo * 0.6f, (Color){ 255, 220, 130, 45 }, (Color){ 0, 0, 0, 0 });
#else
    DrawCircleGradient((int)centro_horizonte.x, (int)centro_horizonte.y, largura_topo * 0.6f,
                       (Color){ 255, 220, 130, 45 }, (Color){ 0, 0, 0, 0 });
#endif

    /* Linhas divisórias das pistas convergindo para a perspectiva */
    for (int p = 0; p <= TOTAL_PISTAS; p++) {
        float fracao = p / (float)TOTAL_PISTAS;
        Vector2 inicio_div = { (centro_x - largura_topo / 2.0f) + (fracao * largura_topo), y_topo };
        Vector2 fim_div    = { (centro_x - largura_base / 2.0f) + (fracao * largura_base), y_base };

        if (p == 0 || p == TOTAL_PISTAS) {
            /* Bordas externas da pista em dourado iluminado */
            DrawLineEx(inicio_div, fim_div, 3.0f, COR_DOURADO);
        } else {
            /* Linhas internas divisórias sutis */
            DrawLineEx(inicio_div, fim_div, 1.5f, (Color){ 110, 70, 120, 160 });
        }
    }

    /* Linha de acerto em perspectiva no plano y_alvo */
    float fator_alvo = (y_alvo - y_topo) / (y_base - y_topo);
    float largura_alvo = largura_topo + (largura_base - largura_topo) * fator_alvo;
    float x_alvo_esq = centro_x - (largura_alvo / 2.0f);
    float x_alvo_dir = centro_x + (largura_alvo / 2.0f);

    /* Faixa de acerto dourada luminosa */
    Vector2 linha_alvo_inicio = { x_alvo_esq, y_alvo };
    Vector2 linha_alvo_fim = { x_alvo_dir, y_alvo };
    DrawLineEx(linha_alvo_inicio, linha_alvo_fim, 6.0f, COR_DOURADO);
    DrawLineEx(linha_alvo_inicio, linha_alvo_fim, 2.0f, WHITE);

    /* Teclas receptoras na base da linha de acerto */
    const char *nomes_teclas[] = { "D", "F", "G", "J", "K" };
    float largura_coluna_alvo = largura_alvo / (float)TOTAL_PISTAS;

    for (int p = 0; p < TOTAL_PISTAS; p++) {
        float cx_rec = x_alvo_esq + (p + 0.5f) * largura_coluna_alvo;
        float larg_rec = largura_coluna_alvo * 0.85f;
        float alt_rec = 34.0f;

        Rectangle ret_rec = { cx_rec - (larg_rec / 2.0f), y_alvo - (alt_rec / 2.0f), larg_rec, alt_rec };
        bool ativa = IsKeyDown(teclas[p]);

        Color cor_fundo_rec = ativa ? cores_pistas[p] : (Color){ 35, 18, 48, 230 };
        Color cor_borda_rec = ativa ? WHITE : cores_pistas[p];

        DrawRectangleRec(ret_rec, cor_fundo_rec);
        DrawRectangleLinesEx(ret_rec, ativa ? 3.5f : 2.0f, cor_borda_rec);

        /* Se ativa, desenha feixe de luz ascendente na pista */
        if (ativa) {
            float fracao_esq = (float)p / (float)TOTAL_PISTAS;
            float fracao_dir = (float)(p + 1) / (float)TOTAL_PISTAS;
            Vector2 t_esq = { (centro_x - largura_topo / 2.0f) + (fracao_esq * largura_topo), y_topo };
            Vector2 t_dir = { (centro_x - largura_topo / 2.0f) + (fracao_dir * largura_topo), y_topo };
            Vector2 b_esq = { (centro_x - largura_base / 2.0f) + (fracao_esq * largura_base), y_alvo };
            Vector2 b_dir = { (centro_x - largura_base / 2.0f) + (fracao_dir * largura_base), y_alvo };

            Color cor_feixe = cores_pistas[p];
            cor_feixe.a = 40;
            DrawTriangle(t_esq, b_esq, b_dir, cor_feixe);
            DrawTriangle(t_esq, b_dir, t_dir, cor_feixe);
        }

        /* Letra indicativa da tecla */
        int larg_letra = MeasureText(nomes_teclas[p], 24);
        DrawText(nomes_teclas[p], cx_rec - (larg_letra / 2), y_alvo - 12, 24, WHITE);
    }

    /* 
     * Desenho das notas da lista encadeada com perspectiva e aceleracao optica:
     * O tempo de antecedencia define a visibilidade da nota a partir do horizonte.
     */
    float tempo_antecedencia = 1.9f;

    Nota *atual = *cabeca_notas;
    while (atual != NULL) {
        if (atual->estado == NOTA_PENDENTE) {
            float tempo_restante = atual->tempo_alvo - *tempo_partida;
            float progresso_tempo = 1.0f - (tempo_restante / tempo_antecedencia);

            /* A nota e visivel desde o horizonte (0.0) ate um pouco apos o acerto (1.15) */
            if (progresso_tempo >= 0.0f && progresso_tempo <= 1.15f) {
                /* Curva exponencial de perspectiva: acelera suavemente conforme se aproxima */
                float fator_curva = (float)pow(progresso_tempo, 1.55);
                float pos_y_nota = y_topo + (y_alvo - y_topo) * fator_curva;

                /* Largura da pista e da nota na coordenada Y atual */
                float fator_largura = (pos_y_nota - y_topo) / (y_base - y_topo);
                if (fator_largura < 0.0f) fator_largura = 0.0f;
                if (fator_largura > 1.0f) fator_largura = 1.0f;

                float largura_pista_y = largura_topo + (largura_base - largura_topo) * fator_largura;
                float x_esq_y = centro_x - (largura_pista_y / 2.0f);
                float largura_coluna_y = largura_pista_y / (float)TOTAL_PISTAS;

                float cx_nota = x_esq_y + (atual->pista + 0.5f) * largura_coluna_y;
                float larg_nota = largura_coluna_y * 0.85f;
                float alt_nota = 14.0f + (18.0f * fator_largura);

                Rectangle ret_nota = {
                    cx_nota - (larg_nota / 2.0f),
                    pos_y_nota - (alt_nota / 2.0f),
                    larg_nota,
                    alt_nota
                };

                /* Desenho da nota com corpo colorido, borda branca e reflexo superior */
                DrawRectangleRec(ret_nota, cores_pistas[atual->pista]);
                DrawRectangleLinesEx(ret_nota, 2.0f, WHITE);
                DrawRectangle(ret_nota.x + 3, ret_nota.y + 2, ret_nota.width - 6, alt_nota * 0.35f, (Color){ 255, 255, 255, 170 });
            }
        }
        atual = atual->proximo;
    }

    /* Painel HUD Superior: Informacoes da musica e pontuacao */
    desenhar_medidor(largura, altura, desempenho->valor);
    DrawText(musica_atual.titulo, 35, 25, 26, COR_DOURADO);
    DrawText(musica_atual.filme, 35, 58, 19, (Color){ 215, 205, 225, 230 });

    /* Canto Superior Direito: Pontuacao e Combo */
    char texto_pontos[64];
    snprintf(texto_pontos, sizeof(texto_pontos), "PONTOS: %d", *pontuacao);
    int larg_pontos = MeasureText(texto_pontos, 26);
    DrawText(texto_pontos, largura - larg_pontos - 35, 25, 26, COR_DOURADO);

    if (*combo > 0) {
        char texto_combo[32];
        snprintf(texto_combo, sizeof(texto_combo), "COMBO: %dx", *combo);
        int larg_combo = MeasureText(texto_combo, 24);
        DrawText(texto_combo, largura - larg_combo - 35, 58, 24, (Color){ 140, 255, 170, 255 });
    }

    /* Feedback visual de acerto com efeito de impacto */
    if (*tempo_feedback > 0.0f) {
        const char *txt_feedback = "PERFEITO!";
        int larg_fb = MeasureText(txt_feedback, 30);
        DrawText(txt_feedback, (largura - larg_fb) / 2, y_alvo - 75, 30, COR_DOURADO_BRILHO);
    }

    const char *aviso_esc = "Pressione [ESC] para pausar";
    int larg_esc = MeasureText(aviso_esc, 18);
    DrawText(aviso_esc, (largura - larg_esc) / 2, altura - 28, 18, (Color){ 190, 180, 200, 220 });

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

    /* Indicadores visuais das teclas D, F, G, J, K */
    const char *teclas[] = { "D", "F", "G", "J", "K" };
    Color cores_teclas[] = {
        (Color){ 190, 60, 100, 255 },  /* Pista 1: Tom rosa/vinho */
        (Color){ 70, 120, 210, 255 },  /* Pista 2: Azul */
        (Color){ 220, 160, 40, 255 },  /* Pista 3: Dourado */
        (Color){ 60, 170, 120, 255 },  /* Pista 4: Verde */
        (Color){ 180, 90, 245, 255 }   /* Pista 5: Roxo */
    };

    float tamanho_tecla = 74.0f;
    float espaco_teclas = 24.0f;
    float largura_total_teclas = (TOTAL_PISTAS * tamanho_tecla) + ((TOTAL_PISTAS - 1) * espaco_teclas);
    float inicio_x_teclas = quadro_x + (largura_quadro - largura_total_teclas) / 2;
    float posicao_y_teclas = quadro_y + 168;

    for (int i = 0; i < TOTAL_PISTAS; i++) {
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

/* Retorna a opcao escolhida: continuar, reiniciar, selecao ou menu. */
int desenhar_menu_pausa(int largura, int altura, Vector2 mouse, bool game_over) {
    DrawRectangle(0, 0, largura, altura, (Color){ 0, 0, 0, 180 });
    Rectangle painel = { (largura - 360) / 2.0f, (altura - 350) / 2.0f, 360, 350 };
    DrawRectangleRec(painel, COR_PAINEL_FUNDO);
    DrawRectangleLinesEx(painel, 2.0f, COR_DOURADO);
    const char *titulo = game_over ? "GAME OVER" : "PAUSADO";
    DrawText(titulo, (largura - MeasureText(titulo, 30)) / 2,
             painel.y + 20, 30, COR_DOURADO);
    if (game_over) {
        const char *motivo = "Voce perdeu por excesso de erros.";
        DrawText(motivo, (largura - MeasureText(motivo, 17)) / 2,
                 painel.y + 72, 17, WHITE);
    }

    const char *opcoes[] = { "CONTINUAR", "REINICIAR", "VOLTAR A SELECAO", "MENU INICIAL" };
    for (int i = game_over ? 1 : 0; i < 4; i++) {
        Rectangle botao = { painel.x + 20, painel.y + 75 + i * 62, 320, 50 };
        if (desenhar_botao(botao, opcoes[i], mouse, 22)) {
            return i + 1;
        }
    }
    return 0;
}

/* Retorna os mesmos codigos do menu de pausa: 2 = reiniciar, 3 = catalogo. */
int desenhar_tela_resultados(int largura, int altura, Vector2 mouse, Musica musica,
                            int pontuacao, int acertos, int perdas, int maior_combo) {
    desenhar_fundo_teatro(largura, altura);
    float largura_painel = largura * 0.82f;
    if (largura_painel > 640.0f) largura_painel = 640.0f;
    float altura_painel = altura * 0.86f;
    if (altura_painel > 520.0f) altura_painel = 520.0f;
    Rectangle painel = { (largura - largura_painel) / 2.0f,
                         (altura - altura_painel) / 2.0f, largura_painel, altura_painel };
    DrawRectangleRec(painel, COR_PAINEL_FUNDO);
    DrawRectangleLinesEx(painel, 2.0f, COR_DOURADO);
    DrawText("VITORIA", (largura - MeasureText("VITORIA", 38)) / 2,
             painel.y + 22, 38, COR_DOURADO_BRILHO);
    DrawText("PARTIDA CONCLUIDA", (largura - MeasureText("PARTIDA CONCLUIDA", 20)) / 2,
             painel.y + 68, 20, WHITE);
    DrawText(musica.titulo, (largura - MeasureText(musica.titulo, 24)) / 2,
             painel.y + 102, 24, COR_DOURADO);

    const char *rotulos[] = { "PONTUACAO", "ACERTOS", "NOTAS PERDIDAS", "MAIOR COMBO" };
    int valores[] = { pontuacao, acertos, perdas, maior_combo };
    for (int i = 0; i < 4; i++) {
        char texto[64];
        snprintf(texto, sizeof(texto), "%s: %d", rotulos[i], valores[i]);
        DrawText(texto, (largura - MeasureText(texto, 22)) / 2,
                 painel.y + 155 + i * 36, 22, WHITE);
    }

    Rectangle jogar_novamente = { painel.x + 30, painel.y + painel.height - 136,
                                  painel.width - 60, 48 };
    Rectangle voltar_catalogo = { painel.x + 30, painel.y + painel.height - 78,
                                  painel.width - 60, 48 };
    if (desenhar_botao(jogar_novamente, "JOGAR NOVAMENTE", mouse, 22)) return 2;
    if (desenhar_botao(voltar_catalogo, "VOLTAR AO CATALOGO", mouse, 22)) return 3;
    const char *aviso = "[ESC] Voltar ao catalogo";
    DrawText(aviso, (largura - MeasureText(aviso, 18)) / 2, altura - 24, 18,
             (Color){ 190, 180, 200, 220 });
    return 0;
}

void desenhar_confirmacao_saida(int largura, int altura, Vector2 mouse,
                                bool *confirmar_saida, bool *deve_sair) {
    DrawRectangle(0, 0, largura, altura, (Color){ 0, 0, 0, 180 });
    Rectangle painel = { (largura - 380) / 2.0f, (altura - 210) / 2.0f, 380, 210 };
    DrawRectangleRec(painel, COR_PAINEL_FUNDO);
    DrawRectangleLinesEx(painel, 2.0f, COR_DOURADO);
    const char *pergunta = "Deseja sair do jogo?";
    DrawText(pergunta, (largura - MeasureText(pergunta, 26)) / 2,
             painel.y + 35, 26, COR_DOURADO);

    Rectangle botao_sim = { painel.x + 25, painel.y + 110, 150, 55 };
    Rectangle botao_nao = { painel.x + 205, painel.y + 110, 150, 55 };
    if (desenhar_botao(botao_sim, "SIM", mouse, 24)) {
        *deve_sair = true;
    }
    if (desenhar_botao(botao_nao, "NAO", mouse, 24)) {
        *confirmar_saida = false;
    }
}

int main(void) {
    const int largura_inicial = 900;
    const int altura_inicial = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(largura_inicial, altura_inicial, "FINAL ACT - Jogo de Ritmo");
    SetExitKey(KEY_NULL);
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
    bool confirmar_saida = false;
    bool partida_pausada = false;
    char erro_carregamento[160] = "";
    Desempenho desempenho;
    iniciar_desempenho(&desempenho);

    /* Variaveis da partida e lista encadeada */
    Nota *cabeca_notas = NULL;
    float tempo_partida = 0.0f;
    int pontuacao = 0;
    int combo = 0;
    int maior_combo = 0;
    int notas_acertadas = 0;
    int notas_perdidas = 0;
    float tempo_feedback = 0.0f;
    int pista_feedback = -1;

    /* Loop principal do jogo */
    while (!WindowShouldClose() && !deve_sair) {
        /* Alterna entre tela cheia e modo janela ao pressionar F11 */
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        int largura_atual = GetScreenWidth();
        int altura_atual = GetScreenHeight();
        Vector2 posicao_mouse = GetMousePosition();

        /* O ESC e tratado uma unica vez por quadro, conforme a tela atual. */
        if (IsKeyPressed(KEY_ESCAPE)) {
            erro_carregamento[0] = '\0';
            if (confirmar_saida) {
                confirmar_saida = false;
            } else if (estado_atual == TELA_RESULTADOS) {
                liberar_lista_notas(&cabeca_notas);
                partida_pausada = false;
                tempo_feedback = 0.0f;
                pista_feedback = -1;
                estado_atual = TELA_SELECAO_MUSICAS;
            } else if (estado_atual == TELA_PARTIDA) {
                if (desempenho.game_over) {
                    liberar_lista_notas(&cabeca_notas);
                    partida_pausada = false;
                    estado_atual = TELA_SELECAO_MUSICAS;
                } else {
                    partida_pausada = !partida_pausada;
                }
            } else if (estado_atual == TELA_MENU_PRINCIPAL) {
                confirmar_saida = true;
            } else {
                estado_atual = TELA_MENU_PRINCIPAL;
            }
        }

        BeginDrawing();

        /* Controle de telas atraves do estado atual */
        switch (estado_atual) {
            case TELA_MENU_PRINCIPAL:
                /* Impede cliques no menu atras da confirmacao de saida. */
                desenhar_menu_principal(largura_atual, altura_atual,
                                        confirmar_saida ? (Vector2){ -1, -1 } : posicao_mouse,
                                        &estado_atual, &confirmar_saida);
                break;

            case TELA_SELECAO_MUSICAS:
                desenhar_tela_selecao_musicas(largura_atual, altura_atual, posicao_mouse, &estado_atual,
                                              catalogo, total_musicas,
                                              &indice_musica_selecionada, &musica_confirmada,
                                              &criterio_atual,
                                              &cabeca_notas, &tempo_partida,
                                              &pontuacao, &combo, &maior_combo,
                                              &notas_acertadas, &notas_perdidas,
                                              erro_carregamento, sizeof(erro_carregamento));
                if (estado_atual == TELA_PARTIDA) {
                    iniciar_desempenho(&desempenho);
                    partida_pausada = false;
                    tempo_feedback = 0.0f;
                    pista_feedback = -1;
                }
                break;

            case TELA_COMO_JOGAR:
                desenhar_tela_como_jogar(largura_atual, altura_atual, posicao_mouse, &estado_atual);
                break;

            case TELA_PARTIDA:
            case TELA_RESULTADOS:
                if (estado_atual == TELA_PARTIDA) {
                    desenhar_tela_partida(largura_atual, altura_atual, partida_pausada, &desempenho,
                                          catalogo[indice_musica_selecionada], &cabeca_notas,
                                          &tempo_partida, &pontuacao, &combo,
                                          &maior_combo, &notas_acertadas, &notas_perdidas,
                                          &tempo_feedback, &pista_feedback);
                    /* As penalizacoes do quadro sao aplicadas antes de decidir a vitoria. */
                    if (!partida_pausada && !desempenho.game_over && todas_notas_resolvidas(cabeca_notas)) {
                        estado_atual = TELA_RESULTADOS;
                        erro_carregamento[0] = '\0';
                        /* Exibe os botoes no proximo quadro, sem aproveitar um clique da partida. */
                        break;
                    }
                }
                if (estado_atual == TELA_RESULTADOS || partida_pausada || desempenho.game_over) {
                    int opcao;
                    if (estado_atual == TELA_RESULTADOS) {
                        opcao = desenhar_tela_resultados(largura_atual, altura_atual, posicao_mouse,
                                                        catalogo[indice_musica_selecionada], pontuacao,
                                                        notas_acertadas, notas_perdidas, maior_combo);
                    } else {
                        opcao = desenhar_menu_pausa(largura_atual, altura_atual, posicao_mouse,
                                                    desempenho.game_over);
                    }
                    if (opcao == 1) {
                        partida_pausada = false;
                        erro_carregamento[0] = '\0';
                    } else if (opcao == 2) {
                        if (carregar_notas_arquivo(&cabeca_notas,
                                                  catalogo[indice_musica_selecionada].caminho_notas,
                                                  erro_carregamento, sizeof(erro_carregamento))) {
                            iniciar_desempenho(&desempenho);
                            tempo_partida = 0.0f;
                            pontuacao = 0;
                            combo = 0;
                            maior_combo = 0;
                            notas_acertadas = 0;
                            notas_perdidas = 0;
                            tempo_feedback = 0.0f;
                            pista_feedback = -1;
                            partida_pausada = false;
                            estado_atual = TELA_PARTIDA;
                        } else {
                            TraceLog(LOG_WARNING, "%s: %s",
                                     catalogo[indice_musica_selecionada].caminho_notas, erro_carregamento);
                        }
                    } else if (opcao == 3 || opcao == 4) {
                        liberar_lista_notas(&cabeca_notas);
                        partida_pausada = false;
                        tempo_feedback = 0.0f;
                        pista_feedback = -1;
                        estado_atual = opcao == 3 ? TELA_SELECAO_MUSICAS : TELA_MENU_PRINCIPAL;
                        erro_carregamento[0] = '\0';
                    }
                    if (erro_carregamento[0] != '\0') {
                        DrawText(erro_carregamento, 20, altura_atual - 55, 16, RED);
                    }
                }
                break;
        }

        if (confirmar_saida) {
            desenhar_confirmacao_saida(largura_atual, altura_atual, posicao_mouse,
                                       &confirmar_saida, &deve_sair);
        }

        EndDrawing();
    }

    /* Libera a memoria alocada para a lista encadeada antes de sair */
    liberar_lista_notas(&cabeca_notas);

    CloseWindow();
    return 0;
}
