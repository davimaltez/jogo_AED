#include "desempenho.h"
#include "lista_notas.h"
#include "catalogo_musicas.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    Desempenho desempenho;
    iniciar_desempenho(&desempenho);
    assert(desempenho.valor == 75 && !desempenho.game_over);
    assert(desempenho.valor >= LIMITE_VERDE);

    registrar_desempenho(&desempenho, false);
    assert(desempenho.valor == 67);
    registrar_desempenho(&desempenho, true);
    assert(desempenho.valor == 70);
    for (int i = 0; i < 50; i++) registrar_desempenho(&desempenho, true);
    assert(desempenho.valor == 100 && !desempenho.game_over);

    iniciar_desempenho(&desempenho);
    for (int i = 0; i < 9; i++) {
        registrar_desempenho(&desempenho, false);
        assert(!desempenho.game_over);
    }
    assert(desempenho.valor == 3);
    registrar_desempenho(&desempenho, false);
    assert(desempenho.valor == 0 && desempenho.game_over);
    registrar_desempenho(&desempenho, true);
    assert(desempenho.valor == 0 && desempenho.game_over);
    iniciar_desempenho(&desempenho);
    assert(desempenho.valor == 75 && !desempenho.game_over);

    /* Recuperacao de uma situacao vermelha sem reiniciar a partida. */
    for (int i = 0; i < 6; i++) registrar_desempenho(&desempenho, false);
    assert(desempenho.valor == 27 && desempenho.valor < LIMITE_VERMELHO);
    for (int i = 0; i < 11; i++) registrar_desempenho(&desempenho, true);
    assert(desempenho.valor == LIMITE_VERDE && !desempenho.game_over);

    Nota *notas = NULL;
    inserir_nota(&notas, 2.0f, 0);
    inserir_nota(&notas, 3.0f, 1);
    inserir_nota(&notas, 3.0f, 4);
    iniciar_desempenho(&desempenho);
    assert(verificar_acerto(notas, 0, 2.0f, 0.20f));
    registrar_desempenho(&desempenho, true);
    assert(!verificar_acerto(notas, 0, 2.0f, 0.20f));
    assert(desempenho.valor == 78);

    int combo = 1;
    int perdas = 0;
    atualizar_notas(notas, 3.23f, 0.22f, &combo, &perdas);
    assert(perdas == 2 && combo == 0);
    for (int i = 0; i < perdas; i++) registrar_desempenho(&desempenho, false);
    assert(desempenho.valor == 62);
    atualizar_notas(notas, 4.0f, 0.22f, &combo, &perdas);
    assert(perdas == 2); /* Nao conta novamente as notas ja perdidas. */
    liberar_lista_notas(&notas);
    assert(notas == NULL);


    /* Ordenar deve mover o registro completo, preservando todos os recursos. */
    Musica catalogo[MAX_MUSICAS], original[MAX_MUSICAS];
    int total;
    inicializar_catalogo(catalogo, &total);
    assert(MAX_MUSICAS == 9 && total == 9);
    memcpy(original, catalogo, sizeof(catalogo));
    assert(strcmp(catalogo[4].titulo, "I'm Still Standing") == 0);
    assert(strcmp(catalogo[4].filme, "Sing: Quem Canta Seus Males Espanta") == 0);
    assert(strcmp(catalogo[5].titulo, "Um Sonho Eu Tenho") == 0);
    assert(strcmp(catalogo[5].filme, "Enrolados") == 0);
    const char *pastas[] = {"im_still_standing", "um_sonho_eu_tenho"};
    for (int i = 4; i < 6; i++) {
        char caminho[128];
        assert(catalogo[i].dificuldade == (i == 4 ? 5 : 4));
        snprintf(caminho, sizeof(caminho), "assets/musicas/%s/audio.wav", pastas[i - 4]);
        assert(strcmp(catalogo[i].caminho_audio, caminho) == 0);
        snprintf(caminho, sizeof(caminho), "assets/musicas/%s/notas.txt", pastas[i - 4]);
        assert(strcmp(catalogo[i].caminho_notas, caminho) == 0);
        snprintf(caminho, sizeof(caminho), "assets/musicas/%s/fundo.png", pastas[i - 4]);
        assert(strcmp(catalogo[i].caminho_fundo, caminho) == 0);
        snprintf(caminho, sizeof(caminho), "assets/musicas/%s/ranking.txt", pastas[i - 4]);
        assert(strcmp(catalogo[i].caminho_ranking, caminho) == 0);
    }
    const CriterioOrdenacao criterios[] = {
        ORDEM_TITULO_AZ, ORDEM_DIFICULDADE_CRESCENTE, ORDEM_DIFICULDADE_DECRESCENTE
    };
    const char *esperadas[][9] = {
        {"Breaking Free", "Dancing Queen", "De Nada", "I'm Still Standing", "Popular",
         "Quase Lá", "Twist and Shout", "Um Mundo Ideal", "Um Sonho Eu Tenho"},
        {"Breaking Free", "Quase Lá", "Um Mundo Ideal", "De Nada", "Twist and Shout",
         "Popular", "Um Sonho Eu Tenho", "Dancing Queen", "I'm Still Standing"},
        {"Dancing Queen", "I'm Still Standing", "Popular", "Um Sonho Eu Tenho",
         "De Nada", "Twist and Shout", "Breaking Free", "Quase Lá", "Um Mundo Ideal"}
    };
    for (int criterio = 0; criterio < 3; criterio++) {
        ordenar_catalogo(catalogo, total, criterios[criterio]);
        for (int i = 0; i < total; i++) {
            assert(strcmp(catalogo[i].titulo, esperadas[criterio][i]) == 0);
            int indice = buscar_indice_por_titulo(original, total, catalogo[i].titulo);
            assert(indice >= 0);
            assert(strcmp(catalogo[i].filme, original[indice].filme) == 0);
            assert(catalogo[i].dificuldade == original[indice].dificuldade);
            assert(strcmp(catalogo[i].caminho_audio, original[indice].caminho_audio) == 0);
            assert(strcmp(catalogo[i].caminho_notas, original[indice].caminho_notas) == 0);
            assert(strcmp(catalogo[i].caminho_fundo, original[indice].caminho_fundo) == 0);
            assert(strcmp(catalogo[i].caminho_ranking, original[indice].caminho_ranking) == 0);
        }
    }

    puts("OK: limites, recuperacao, Game Over, reinicio, notas e catalogo.");
    return 0;
}
