#include "raylib.h"
#include "catalogo_musicas.h"
#include "lista_notas.h"
#include "partida.h"
#include "midia_partida.h"
#include "ranking.h"
#include "interface.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define LARGURA_INICIAL 900
#define ALTURA_INICIAL 600
#define LARGURA_MINIMA_JANELA 640
#define ALTURA_MINIMA_JANELA 360

/* Telas do jogo. A tela atual decide o que e atualizado e desenhado. */
typedef enum {
    TELA_MENU_PRINCIPAL,
    TELA_SELECAO_MUSICAS,
    TELA_COMO_JOGAR,
    TELA_PARTIDA,
    TELA_REGISTRO_RANKING,
    TELA_RESULTADOS
} EstadoTela;

/*
 * Menus que aparecem por cima de uma tela. Os numeros comecam em 100
 * para nunca coincidir com os valores de EstadoTela. Servem apenas para
 * perceber quando o menu visivel mudou e reiniciar o foco do teclado.
 */
#define MENU_SEM_FOCO -1      /* partida em andamento, sem menu */
#define MENU_CONFIRMACAO 100
#define MENU_PAUSA 101
#define MENU_GAME_OVER 102

/*
 * Todo o estado do programa fica nesta struct, criada no main.
 * As funcoes recebem um ponteiro para ela, em vez de usar variaveis globais.
 */
typedef struct {
    EstadoTela tela;

    Musica catalogo[MAX_MUSICAS];
    int total_musicas;
    CriterioOrdenacao criterio;
    int indice_musica;              /* musica selecionada no catalogo */

    Partida partida;                /* lista de notas, pontos, medidor... */
    MidiaPartida midia;             /* audio e fundo da musica atual */

    Ranking ranking;                /* ranking da musica que acabou de ser jogada */
    int posicao_ranking;            /* posicao da ultima pontuacao (-1 = fora) */
    char nome_jogador[TAMANHO_NOME];
    char aviso_ranking[64];

    bool confirmar_saida;
    bool deve_sair;
    int foco_menu;                  /* item destacado pelo teclado */
    int menu_anterior;
    char mensagem_erro[160];
} Jogo;

/* Tabela fixa (somente leitura) que liga cada pista a uma tecla. */
static const int TECLAS_PISTAS[TOTAL_PISTAS] = { KEY_D, KEY_F, KEY_G, KEY_J, KEY_K };

static void iniciar_jogo(Jogo *jogo) {
    jogo->tela = TELA_MENU_PRINCIPAL;
    inicializar_catalogo(jogo->catalogo, &jogo->total_musicas);
    jogo->criterio = ORDEM_TITULO_AZ;
    ordenar_catalogo(jogo->catalogo, jogo->total_musicas, jogo->criterio);
    jogo->indice_musica = 0;

    iniciar_estrutura_partida(&jogo->partida);
    iniciar_midia_partida(&jogo->midia);

    iniciar_ranking(&jogo->ranking);
    jogo->posicao_ranking = -1;
    jogo->nome_jogador[0] = '\0';
    jogo->aviso_ranking[0] = '\0';

    jogo->confirmar_saida = false;
    jogo->deve_sair = false;
    jogo->foco_menu = 0;
    jogo->menu_anterior = -2;   /* valor que nenhum menu usa: forca o primeiro ajuste de foco */
    jogo->mensagem_erro[0] = '\0';
}

static void liberar_jogo(Jogo *jogo) {
    encerrar_partida(&jogo->partida);
    descarregar_midia_partida(&jogo->midia);
}

static Musica *musica_atual(Jogo *jogo) {
    return &jogo->catalogo[jogo->indice_musica];
}

/* ================= Inicio, reinicio e saida da partida ================= */

/* Carrega notas e midia. So muda para a partida se as notas forem validas. */
static void iniciar_partida(Jogo *jogo) {
    Musica *musica = musica_atual(jogo);
    if (!preparar_partida(&jogo->partida, musica->caminho_notas,
                          jogo->mensagem_erro, sizeof(jogo->mensagem_erro))) {
        TraceLog(LOG_WARNING, "%s: %s", musica->caminho_notas, jogo->mensagem_erro);
        return;
    }
    jogo->mensagem_erro[0] = '\0';
    carregar_midia_partida(&jogo->midia, musica);
    jogo->tela = TELA_PARTIDA;
}

/*
 * Usado por REINICIAR (pausa e Game Over) e JOGAR NOVAMENTE (resultados).
 * Recarrega as notas; se falhar, a tela atual e mantida com a mensagem.
 */
static void reiniciar_partida(Jogo *jogo) {
    Musica *musica = musica_atual(jogo);
    if (!preparar_partida(&jogo->partida, musica->caminho_notas,
                          jogo->mensagem_erro, sizeof(jogo->mensagem_erro))) {
        TraceLog(LOG_WARNING, "%s: %s", musica->caminho_notas, jogo->mensagem_erro);
        return;
    }
    jogo->mensagem_erro[0] = '\0';
    reiniciar_audio_partida(&jogo->midia);   /* audio e notas voltam juntos ao zero */
    jogo->tela = TELA_PARTIDA;
}

/* Libera a lista de notas e a midia, e vai para a tela de destino. */
static void sair_da_partida(Jogo *jogo, EstadoTela destino) {
    encerrar_partida(&jogo->partida);
    descarregar_midia_partida(&jogo->midia);
    jogo->mensagem_erro[0] = '\0';
    jogo->tela = destino;
}

static bool tela_de_partida(EstadoTela tela) {
    return tela == TELA_PARTIDA || tela == TELA_REGISTRO_RANKING || tela == TELA_RESULTADOS;
}

/* ================= Catalogo ================= */

/* Reordena o catalogo com Merge Sort mantendo a mesma musica selecionada. */
static void aplicar_ordenacao(Jogo *jogo, CriterioOrdenacao criterio) {
    char titulo_selecionado[64];
    strncpy(titulo_selecionado, musica_atual(jogo)->titulo, sizeof(titulo_selecionado) - 1);
    titulo_selecionado[sizeof(titulo_selecionado) - 1] = '\0';

    jogo->criterio = criterio;
    ordenar_catalogo(jogo->catalogo, jogo->total_musicas, criterio);

    int novo_indice = buscar_indice_por_titulo(jogo->catalogo, jogo->total_musicas, titulo_selecionado);
    if (novo_indice != -1) {
        jogo->indice_musica = novo_indice;
    }
}

/* ================= Ranking ================= */

static void abrir_registro_ranking(Jogo *jogo) {
    jogo->nome_jogador[0] = '\0';
    jogo->mensagem_erro[0] = '\0';
    jogo->tela = TELA_REGISTRO_RANKING;
}

/* Le as letras digitadas neste quadro e o Backspace. */
static void ler_nome_jogador(Jogo *jogo) {
    size_t tamanho = strlen(jogo->nome_jogador);
    int caractere = GetCharPressed();
    while (caractere > 0) {
        if (caractere_valido_nome(caractere) && tamanho < TAMANHO_NOME - 1) {
            jogo->nome_jogador[tamanho] = (char)caractere;
            tamanho++;
            jogo->nome_jogador[tamanho] = '\0';
        }
        caractere = GetCharPressed();
    }
    if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && tamanho > 0) {
        jogo->nome_jogador[tamanho - 1] = '\0';
    }
}

/*
 * Carrega o ranking da musica e, se "salvar" for true, insere a pontuacao
 * (Merge Sort acontece dentro de inserir_no_ranking) e grava o arquivo.
 * Depois vai para a tela de resultados.
 */
static void concluir_registro_ranking(Jogo *jogo, bool salvar) {
    Musica *musica = musica_atual(jogo);
    jogo->posicao_ranking = -1;
    bool leu = carregar_ranking(&jogo->ranking, musica->caminho_ranking);

    if (!salvar) {
        snprintf(jogo->aviso_ranking, sizeof(jogo->aviso_ranking), "Pontuacao nao registrada.");
    } else if (!leu) {
        /* Gravar agora apagaria o ranking antigo que nao foi lido. */
        snprintf(jogo->aviso_ranking, sizeof(jogo->aviso_ranking), "Erro ao ler o ranking; nada foi salvo.");
    } else {
        int posicao = inserir_no_ranking(&jogo->ranking, jogo->nome_jogador, jogo->partida.pontuacao);
        if (!salvar_ranking(&jogo->ranking, musica->caminho_ranking)) {
            carregar_ranking(&jogo->ranking, musica->caminho_ranking);
            snprintf(jogo->aviso_ranking, sizeof(jogo->aviso_ranking), "Nao foi possivel salvar o ranking.");
            TraceLog(LOG_WARNING, "Falha ao gravar %s", musica->caminho_ranking);
        } else if (posicao >= 0) {
            jogo->posicao_ranking = posicao;
            snprintf(jogo->aviso_ranking, sizeof(jogo->aviso_ranking), "Voce ficou em %do lugar!", posicao + 1);
        } else {
            snprintf(jogo->aviso_ranking, sizeof(jogo->aviso_ranking), "Pontuacao fora do top %d.", MAX_RANKING);
        }
    }
    jogo->tela = TELA_RESULTADOS;
}

/* ================= Atualizacao (sem desenhar) ================= */

static void ler_teclas_pressionadas(bool pressionadas[TOTAL_PISTAS]) {
    for (int pista = 0; pista < TOTAL_PISTAS; pista++) {
        pressionadas[pista] = IsKeyPressed(TECLAS_PISTAS[pista]);
    }
}

static void ler_teclas_seguradas(bool seguradas[TOTAL_PISTAS]) {
    for (int pista = 0; pista < TOTAL_PISTAS; pista++) {
        seguradas[pista] = IsKeyDown(TECLAS_PISTAS[pista]);
    }
}

/* O ESC e tratado uma unica vez por quadro, conforme a tela atual. */
static void tratar_tecla_esc(Jogo *jogo) {
    if (!IsKeyPressed(KEY_ESCAPE)) {
        return;
    }
    jogo->mensagem_erro[0] = '\0';
    if (jogo->confirmar_saida) {
        jogo->confirmar_saida = false;
        return;
    }
    switch (jogo->tela) {
        case TELA_RESULTADOS:
            sair_da_partida(jogo, TELA_SELECAO_MUSICAS);
            break;
        case TELA_REGISTRO_RANKING:
            concluir_registro_ranking(jogo, false);
            break;
        case TELA_PARTIDA:
            if (jogo->partida.desempenho.game_over) {
                sair_da_partida(jogo, TELA_SELECAO_MUSICAS);
            } else {
                jogo->partida.pausada = !jogo->partida.pausada;
            }
            break;
        case TELA_MENU_PRINCIPAL:
            jogo->confirmar_saida = true;
            break;
        default:
            jogo->tela = TELA_MENU_PRINCIPAL;
            break;
    }
}

/*
 * Um quadro da partida: o audio anda (ou pausa), o relogio e calculado
 * a partir dele e as regras sao aplicadas. Nada e desenhado aqui.
 */
static void atualizar_tela_partida(Jogo *jogo) {
    Partida *partida = &jogo->partida;
    bool bloqueada = partida_bloqueada(partida);

    atualizar_audio_partida(&jogo->midia, bloqueada);
    float novo_tempo = calcular_tempo_partida(partida->tempo, GetFrameTime(), bloqueada,
                                              audio_esta_tocando(&jogo->midia),
                                              tempo_audio_partida(&jogo->midia));
    bool teclas[TOTAL_PISTAS];
    ler_teclas_pressionadas(teclas);
    atualizar_partida(partida, novo_tempo, teclas);

    /* A vitoria e decidida depois das perdas do quadro: o Game Over tem prioridade. */
    if (partida_concluida(partida)) {
        abrir_registro_ranking(jogo);
    }
}

static void atualizar_jogo(Jogo *jogo) {
    switch (jogo->tela) {
        case TELA_PARTIDA:
            atualizar_tela_partida(jogo);
            break;
        case TELA_REGISTRO_RANKING:
            atualizar_audio_partida(&jogo->midia, false);   /* a musica continua ao fundo */
            ler_nome_jogador(jogo);
            if (IsKeyPressed(KEY_ENTER)) {
                concluir_registro_ranking(jogo, true);
            }
            break;
        case TELA_RESULTADOS:
            atualizar_audio_partida(&jogo->midia, false);
            break;
        default:
            break;
    }
}

static int identificar_menu(const Jogo *jogo) {
    if (jogo->confirmar_saida) return MENU_CONFIRMACAO;
    if (jogo->tela == TELA_PARTIDA) {
        if (jogo->partida.desempenho.game_over) return MENU_GAME_OVER;
        if (jogo->partida.pausada) return MENU_PAUSA;
        return MENU_SEM_FOCO;
    }
    return (int)jogo->tela;
}

/* Quando o menu visivel muda, o foco volta para o item inicial dele. */
static void atualizar_foco_menu(Jogo *jogo) {
    int menu = identificar_menu(jogo);
    if (menu == jogo->menu_anterior) {
        return;
    }
    if (menu == MENU_CONFIRMACAO) {
        jogo->foco_menu = 1;   /* a confirmacao comeca em NAO */
    } else if (jogo->tela == TELA_SELECAO_MUSICAS) {
        jogo->foco_menu = jogo->indice_musica + 3;
    } else {
        jogo->foco_menu = 0;
    }
    jogo->menu_anterior = menu;
}

/* ================= Renderizacao ================= */

/*
 * Desenha a tela atual e devolve a acao escolhida nos menus.
 * "entrada_liberada" e false no quadro em que a tela acabou de mudar,
 * para que o mesmo Enter ou clique nao acione a tela nova.
 */
static AcaoMenu desenhar_jogo(Jogo *jogo, int largura, int altura, Vector2 mouse,
                              bool entrada_liberada, const bool teclas_seguradas[TOTAL_PISTAS]) {
    Vector2 sem_mouse = { -1, -1 };
    /* Com a confirmacao aberta, a tela de tras nao recebe cliques nem teclas. */
    bool tela_recebe_entrada = entrada_liberada && !jogo->confirmar_saida;
    Vector2 mouse_tela = tela_recebe_entrada ? mouse : sem_mouse;
    int *foco_tela = tela_recebe_entrada ? &jogo->foco_menu : NULL;
    Musica *musica = musica_atual(jogo);
    AcaoMenu acao = ACAO_NENHUMA;

    switch (jogo->tela) {
        case TELA_MENU_PRINCIPAL:
            acao = desenhar_menu_principal(largura, altura, mouse_tela, foco_tela);
            break;
        case TELA_COMO_JOGAR:
            acao = desenhar_tela_como_jogar(largura, altura, mouse_tela, foco_tela);
            break;
        case TELA_SELECAO_MUSICAS: {
            int indice_antes = jogo->indice_musica;
            acao = desenhar_tela_selecao_musicas(largura, altura, mouse_tela, jogo->catalogo,
                                                 jogo->total_musicas, &jogo->indice_musica,
                                                 jogo->criterio, jogo->mensagem_erro, foco_tela);
            if (jogo->indice_musica != indice_antes) {
                jogo->mensagem_erro[0] = '\0';
            }
            break;
        }
        case TELA_PARTIDA:
            desenhar_tela_partida(largura, altura, &jogo->partida, musica, &jogo->midia, teclas_seguradas);
            if (partida_bloqueada(&jogo->partida)) {
                acao = desenhar_menu_pausa(largura, altura, mouse_tela,
                                           jogo->partida.desempenho.game_over, foco_tela);
            }
            desenhar_mensagem_erro(largura, altura, jogo->mensagem_erro);
            break;
        case TELA_REGISTRO_RANKING:
            desenhar_tela_registro_ranking(largura, altura, musica, jogo->partida.pontuacao,
                                           jogo->nome_jogador);
            break;
        case TELA_RESULTADOS:
            acao = desenhar_tela_resultados(largura, altura, mouse_tela, musica, &jogo->partida,
                                            &jogo->ranking, jogo->posicao_ranking,
                                            jogo->aviso_ranking, foco_tela);
            desenhar_mensagem_erro(largura, altura, jogo->mensagem_erro);
            break;
    }

    if (jogo->confirmar_saida) {
        AcaoMenu resposta = desenhar_confirmacao_saida(largura, altura,
                                                       entrada_liberada ? mouse : sem_mouse,
                                                       entrada_liberada ? &jogo->foco_menu : NULL);
        if (resposta != ACAO_NENHUMA) {
            acao = resposta;
        }
    }
    return acao;
}

/* ================= Acoes dos menus ================= */

/* Unico lugar onde as escolhas dos menus mudam o estado do jogo. */
static void aplicar_acao(Jogo *jogo, AcaoMenu acao) {
    switch (acao) {
        case ACAO_JOGAR:
            jogo->tela = TELA_SELECAO_MUSICAS;
            break;
        case ACAO_COMO_JOGAR:
            jogo->tela = TELA_COMO_JOGAR;
            break;
        case ACAO_PEDIR_SAIDA:
            jogo->confirmar_saida = true;
            break;
        case ACAO_CONFIRMAR_SAIDA:
            jogo->deve_sair = true;
            break;
        case ACAO_CANCELAR_SAIDA:
            jogo->confirmar_saida = false;
            break;
        case ACAO_VOLTAR_MENU:
            if (tela_de_partida(jogo->tela)) {
                sair_da_partida(jogo, TELA_MENU_PRINCIPAL);
            } else {
                jogo->tela = TELA_MENU_PRINCIPAL;
            }
            break;
        case ACAO_ORDENAR_TITULO:
            aplicar_ordenacao(jogo, ORDEM_TITULO_AZ);
            break;
        case ACAO_ORDENAR_DIFICULDADE_CRESCENTE:
            aplicar_ordenacao(jogo, ORDEM_DIFICULDADE_CRESCENTE);
            break;
        case ACAO_ORDENAR_DIFICULDADE_DECRESCENTE:
            aplicar_ordenacao(jogo, ORDEM_DIFICULDADE_DECRESCENTE);
            break;
        case ACAO_INICIAR_PARTIDA:
            iniciar_partida(jogo);
            break;
        case ACAO_CONTINUAR:
            jogo->partida.pausada = false;
            jogo->mensagem_erro[0] = '\0';
            break;
        case ACAO_REINICIAR:
            reiniciar_partida(jogo);
            break;
        case ACAO_VOLTAR_SELECAO:
            sair_da_partida(jogo, TELA_SELECAO_MUSICAS);
            break;
        case ACAO_NENHUMA:
            break;
    }
}

/*
 * Um quadro completo do jogo, sempre na mesma ordem:
 * 1) atualizar (entrada, tempo e regras), 2) desenhar, 3) aplicar a acao do menu.
 */
static void executar_quadro(Jogo *jogo) {
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }
    int largura = GetScreenWidth();
    int altura = GetScreenHeight();
    Vector2 mouse = GetMousePosition();
    EstadoTela tela_no_inicio = jogo->tela;

    /* 1) Atualizacao: entrada, tempo e regras, sem desenhar nada. */
    tratar_tecla_esc(jogo);
    atualizar_jogo(jogo);
    atualizar_foco_menu(jogo);

    /* 2) Renderizacao: desenha o estado atual e recolhe a escolha do menu. */
    bool teclas_seguradas[TOTAL_PISTAS];
    ler_teclas_seguradas(teclas_seguradas);
    BeginDrawing();
    AcaoMenu acao = desenhar_jogo(jogo, largura, altura, mouse,
                                  jogo->tela == tela_no_inicio, teclas_seguradas);
    EndDrawing();

    /* 3) A escolha do menu so muda o estado depois do quadro desenhado. */
    aplicar_acao(jogo, acao);
}

/* ================= Programa ================= */

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(LARGURA_INICIAL, ALTURA_INICIAL, "FINAL ACT - Jogo de Ritmo");
    SetWindowMinSize(LARGURA_MINIMA_JANELA, ALTURA_MINIMA_JANELA);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitAudioDevice();

    /* Inicia o jogo em tela cheia; F11 alterna para janela. */
    ToggleFullscreen();

    Jogo jogo;
    iniciar_jogo(&jogo);

    while (!WindowShouldClose() && !jogo.deve_sair) {
        executar_quadro(&jogo);
    }

    liberar_jogo(&jogo);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
