#include "partida.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

/* Testa o mapa real; nunca escreve no ranking nem modifica os recursos. */
int main(int argc, char **argv) {
    const char *caminho = argc > 1 ? argv[1] : "assets/musicas/dancing_queen/notas.txt";
    Partida partida;
    iniciar_estrutura_partida(&partida);
    char erro[160];
    if (!preparar_partida(&partida, caminho, erro, sizeof(erro))) {
        fprintf(stderr, "%s\n", erro);
        return 1;
    }
    assert(validar_duracao_notas(partida.notas, 244.227f, erro, sizeof(erro)));
    int quantidade = 0, longas = 0, pistas[TOTAL_PISTAS] = { 0 };
    float ultimo_fim = 0;
    for (const Nota *nota = partida.notas; nota != NULL; nota = nota->proximo) {
        quantidade++;
        longas += nota->duracao > 0;
        pistas[nota->pista]++;
        if (nota->tempo_alvo + nota->duracao > ultimo_fim) ultimo_fim = nota->tempo_alvo + nota->duracao;
        /* No maximo duas teclas exigidas simultaneamente, contando sustains. */
        int simultaneas = 0;
        for (const Nota *outra = partida.notas; outra != NULL; outra = outra->proximo) {
            if (outra->tempo_alvo == nota->tempo_alvo ||
                (outra->tempo_alvo < nota->tempo_alvo && outra->tempo_alvo + outra->duracao > nota->tempo_alvo)) {
                simultaneas++;
            }
        }
        assert(simultaneas <= 2);
    }
    /* A selecao segue apenas o teclado; nao exige preencher trechos sem referencia. */
    assert(quantidade > 0 && longas > 0);
    assert(partida.notas->tempo_alvo < 1 && ultimo_fim > 200);
    for (int pista = 0; pista < TOTAL_PISTAS; pista++) assert(pistas[pista] > 0);
    const int taxas[] = { 30, 60, 144, 240 };
    for (unsigned int i = 0; i < sizeof(taxas) / sizeof(taxas[0]); i++) {
        assert(preparar_partida(&partida, caminho, erro, sizeof(erro)));
        partida.usa_audio = true;
        partida.duracao_audio = 244.227f;
        clock_t inicio = clock();
        int quadros = 0;
        for (int quadro = 0; quadro <= 244.6 * taxas[i]; quadro++) {
            float tempo = (float)quadro / taxas[i];
            bool pressionadas[TOTAL_PISTAS] = { false };
            bool seguradas[TOTAL_PISTAS] = { false };
            for (const Nota *nota = partida.notas; nota != NULL; nota = nota->proximo) {
                if (nota->estado == NOTA_PENDENTE && nota->tempo_alvo <= tempo) {
                    pressionadas[nota->pista] = true;
                    seguradas[nota->pista] = true;
                }
                if (nota->estado == NOTA_SUSTENTANDO) seguradas[nota->pista] = true;
            }
            partida.audio_finalizado = tempo >= partida.duracao_audio;
            atualizar_partida(&partida, tempo, pressionadas, seguradas);
            quadros++;
            assert(!partida.desempenho.game_over);
            if (!partida.audio_finalizado) assert(!partida_concluida(&partida));
        }
        double milissegundos = 1000.0 * (clock() - inicio) / CLOCKS_PER_SEC;
        assert(partida_concluida(&partida));
        assert(partida.notas_acertadas == quantidade && partida.notas_perdidas == 0);
        assert(partida.sustentacoes_concluidas == longas && partida.sustentacoes_interrompidas == 0);
        printf("OK a %d Hz: %d notas, %d sustains; %.4f ms/quadro de CPU (inclui jogador automatico).\n",
               taxas[i], quantidade, longas, milissegundos / quadros);
    }
    printf("OK: Dancing Queen, %d notas, %d sustains, %.3f s, todas as teclas e acordes jogaveis.\n",
           quantidade, longas, ultimo_fim);
    encerrar_partida(&partida);
    return 0;
}
