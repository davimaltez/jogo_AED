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

/*
 * Estado das animacoes da partida (faiscas, brilhos, agulha do medidor).
 * E so visual: nada aqui interfere nas regras nem no tempo da musica.
 * Tudo tem tamanho fixo, entao nenhuma memoria e alocada durante o jogo.
 */
#define MAX_FAISCAS 96

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    float vida;             /* segundos restantes; <= 0 significa livre */
    float vida_total;
    int pista;              /* define a cor da faisca */
} Faisca;

typedef struct {
    bool sincronizado;                  /* false ate ver a partida pela primeira vez */
    float relogio;                      /* tempo das animacoes (congela na pausa) */
    float tempo_partida;                /* tempo da partida no quadro anterior */
    int acertos_pista[TOTAL_PISTAS];    /* notas acertadas ja vistas em cada pista */
    int perdas_pista[TOTAL_PISTAS];     /* notas perdidas ja vistas em cada pista */
    float impacto[TOTAL_PISTAS];        /* 1 no acerto, cai ate 0 */
    float erro[TOTAL_PISTAS];           /* 1 na nota perdida, cai ate 0 */
    float brilho_tecla[TOTAL_PISTAS];   /* 1 com a tecla segurada, apaga ao soltar */
    float agulha;                       /* posicao suavizada do medidor */
    float velocidade_agulha;
    float pulso_medidor;                /* +1 quando sobe, -1 quando desce, volta a 0 */
    int desempenho_anterior;
    int pontuacao_anterior;
    int combo_anterior;
    float pulso_pontos;
    float pulso_combo;
    Faisca faiscas[MAX_FAISCAS];
    int proxima_faisca;                 /* as faiscas sao reaproveitadas em circulo */
    unsigned int semente;
} EfeitosPartida;

void iniciar_efeitos_partida(EfeitosPartida *efeitos);

/*
 * As regras da partida ja foram aplicadas em atualizar_partida; aqui a
 * partida e apenas lida. So "efeitos" e alterado, para animar o desenho.
 */
void desenhar_tela_partida(int largura, int altura, const Partida *partida,
                           const Musica *musica, const MidiaPartida *midia,
                           const bool teclas_seguradas[TOTAL_PISTAS], EfeitosPartida *efeitos);

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