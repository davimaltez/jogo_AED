#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include "raylib.h"
#include "catalogo_musicas.h"
#include "partida.h"
#include "midia_partida.h"
#include "ranking.h"

/*
 * Acoes que o jogador pode escolher nos menus.
 * As telas apenas desenham e informam a acao escolhida;
 * quem muda de tela, carrega musica ou sai do jogo e o principal.c.
 */
typedef enum {
    ACAO_NENHUMA,
    ACAO_JOGAR,
    ACAO_COMO_JOGAR,
    ACAO_PEDIR_SAIDA,
    ACAO_CONFIRMAR_SAIDA,
    ACAO_CANCELAR_SAIDA,
    ACAO_VOLTAR_MENU,
    ACAO_ORDENAR_TITULO,
    ACAO_ORDENAR_DIFICULDADE_CRESCENTE,
    ACAO_ORDENAR_DIFICULDADE_DECRESCENTE,
    ACAO_INICIAR_PARTIDA,
    ACAO_VER_RANKING,
    ACAO_CONTINUAR,
    ACAO_REINICIAR,
    ACAO_VOLTAR_SELECAO
} AcaoMenu;

/*
 * A interface foi desenhada para 1280x720. A escala diz quanto a janela
 * atual e maior ou menor que isso (0.5 = metade). Tamanhos de fonte,
 * botoes e paineis sao multiplicados por ela.
 */
float calcular_escala(int largura, int altura);
int escalar(float valor, float escala);

/* Setas mudam o item em foco do menu atual. */
void navegar_menu(int *foco, int quantidade);

void desenhar_fundo_teatro(int largura, int altura);

AcaoMenu desenhar_menu_principal(int largura, int altura, Vector2 mouse, int *foco);

AcaoMenu desenhar_tela_como_jogar(int largura, int altura, Vector2 mouse, int *foco);

/*
 * Ordem do foco: 0-2 filtros, 3 ate total+2 musicas,
 * total+3 iniciar partida, total+4 ver ranking e total+5 voltar ao menu.
 * Clicar ou navegar ate uma musica altera *indice_selecionado.
 */
AcaoMenu desenhar_tela_selecao_musicas(int largura, int altura, Vector2 mouse,
                                       const Musica catalogo[], int total_musicas,
                                       int *indice_selecionado, CriterioOrdenacao criterio,
                                       const char *mensagem_erro, int *foco);

/* So desenha: as regras da partida ja foram aplicadas em atualizar_partida. */
void desenhar_tela_partida(int largura, int altura, const Partida *partida,
                           const Musica *musica, const MidiaPartida *midia,
                           const bool teclas_seguradas[TOTAL_PISTAS]);

AcaoMenu desenhar_menu_pausa(int largura, int altura, Vector2 mouse, bool game_over, int *foco);

void desenhar_tela_registro_ranking(int largura, int altura, const Musica *musica,
                                    int pontuacao, const char *nome_digitado);

AcaoMenu desenhar_tela_resultados(int largura, int altura, Vector2 mouse,
                                  const Musica *musica, const Partida *partida,
                                  const Ranking *ranking, int posicao_ranking,
                                  const char *aviso_ranking, int *foco);

/* Top 10 da musica escolhida no catalogo, sem precisar jogar. */
AcaoMenu desenhar_tela_ranking_musica(int largura, int altura, Vector2 mouse,
                                      const Musica *musica, const Ranking *ranking,
                                      const char *aviso_ranking, int *foco);

AcaoMenu desenhar_confirmacao_saida(int largura, int altura, Vector2 mouse, int *foco);

/* Mostra uma mensagem de erro em vermelho no rodape. */
void desenhar_mensagem_erro(int largura, int altura, const char *mensagem);

#endif /* INTERFACE_H */