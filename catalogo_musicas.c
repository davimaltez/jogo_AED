#include "catalogo_musicas.h"
#include <string.h>

/*
 * Preenche o vetor com as musicas iniciais baseadas em filmes musicais.
 * Cada musica possui sua pasta de audio, notas, fundo e ranking.
 */
void inicializar_catalogo(Musica catalogo[], int *quantidade) {
    *quantidade = 9;

    /* Musica 1 - Wicked */
    strncpy(catalogo[0].titulo, "Popular", sizeof(catalogo[0].titulo) - 1);
    catalogo[0].titulo[sizeof(catalogo[0].titulo) - 1] = '\0';
    strncpy(catalogo[0].filme, "Wicked", sizeof(catalogo[0].filme) - 1);
    catalogo[0].filme[sizeof(catalogo[0].filme) - 1] = '\0';
    catalogo[0].dificuldade = 4;
    strncpy(catalogo[0].caminho_audio, "assets/musicas/popular/popular-from-wicked-the-soundtrack-1_U0A7jBty.mp3", sizeof(catalogo[0].caminho_audio) - 1);
    catalogo[0].caminho_audio[sizeof(catalogo[0].caminho_audio) - 1] = '\0';
    strncpy(catalogo[0].caminho_notas, "assets/musicas/popular/notas.txt", sizeof(catalogo[0].caminho_notas) - 1);
    catalogo[0].caminho_notas[sizeof(catalogo[0].caminho_notas) - 1] = '\0';
    strncpy(catalogo[0].caminho_fundo, "assets/musicas/popular/fundo.png", sizeof(catalogo[0].caminho_fundo) - 1);
    catalogo[0].caminho_fundo[sizeof(catalogo[0].caminho_fundo) - 1] = '\0';
    strncpy(catalogo[0].caminho_ranking, "assets/musicas/popular/ranking.txt", sizeof(catalogo[0].caminho_ranking) - 1);
    catalogo[0].caminho_ranking[sizeof(catalogo[0].caminho_ranking) - 1] = '\0';

    /* Musica 2 - Mamma Mia! */
    strncpy(catalogo[1].titulo, "Dancing Queen", sizeof(catalogo[1].titulo) - 1);
    catalogo[1].titulo[sizeof(catalogo[1].titulo) - 1] = '\0';
    strncpy(catalogo[1].filme, "Mamma Mia!", sizeof(catalogo[1].filme) - 1);
    catalogo[1].filme[sizeof(catalogo[1].filme) - 1] = '\0';
    catalogo[1].dificuldade = 5;
    strncpy(catalogo[1].caminho_audio, "assets/musicas/dancing_queen/audio.mp3", sizeof(catalogo[1].caminho_audio) - 1);
    catalogo[1].caminho_audio[sizeof(catalogo[1].caminho_audio) - 1] = '\0';
    strncpy(catalogo[1].caminho_notas, "assets/musicas/dancing_queen/notas.txt", sizeof(catalogo[1].caminho_notas) - 1);
    catalogo[1].caminho_notas[sizeof(catalogo[1].caminho_notas) - 1] = '\0';
    strncpy(catalogo[1].caminho_fundo, "assets/musicas/dancing_queen/fundo.png", sizeof(catalogo[1].caminho_fundo) - 1);
    catalogo[1].caminho_fundo[sizeof(catalogo[1].caminho_fundo) - 1] = '\0';
    strncpy(catalogo[1].caminho_ranking, "assets/musicas/dancing_queen/ranking.txt", sizeof(catalogo[1].caminho_ranking) - 1);
    catalogo[1].caminho_ranking[sizeof(catalogo[1].caminho_ranking) - 1] = '\0';

    /* Musica 3 - High School Musical */
    strncpy(catalogo[2].titulo, "Breaking Free", sizeof(catalogo[2].titulo) - 1);
    catalogo[2].titulo[sizeof(catalogo[2].titulo) - 1] = '\0';
    strncpy(catalogo[2].filme, "High School Musical", sizeof(catalogo[2].filme) - 1);
    catalogo[2].filme[sizeof(catalogo[2].filme) - 1] = '\0';
    catalogo[2].dificuldade = 2;
    strncpy(catalogo[2].caminho_audio, "assets/musicas/breaking_free/audio.mp3", sizeof(catalogo[2].caminho_audio) - 1);
    catalogo[2].caminho_audio[sizeof(catalogo[2].caminho_audio) - 1] = '\0';
    strncpy(catalogo[2].caminho_notas, "assets/musicas/breaking_free/notas.txt", sizeof(catalogo[2].caminho_notas) - 1);
    catalogo[2].caminho_notas[sizeof(catalogo[2].caminho_notas) - 1] = '\0';
    strncpy(catalogo[2].caminho_fundo, "assets/musicas/breaking_free/fundo.png", sizeof(catalogo[2].caminho_fundo) - 1);
    catalogo[2].caminho_fundo[sizeof(catalogo[2].caminho_fundo) - 1] = '\0';
    strncpy(catalogo[2].caminho_ranking, "assets/musicas/breaking_free/ranking.txt", sizeof(catalogo[2].caminho_ranking) - 1);
    catalogo[2].caminho_ranking[sizeof(catalogo[2].caminho_ranking) - 1] = '\0';

    /* Musica 4 - origem provisoria; nivel medio do gerador offline. */
    catalogo[3] = (Musica){
        .titulo = "Twist and Shout",
        .filme = "Twist and Shout",
        .dificuldade = 3,
        .caminho_audio = "assets/musicas/twist_and_shout/audio.mp3",
        .caminho_notas = "assets/musicas/twist_and_shout/notas.txt",
        .caminho_fundo = "assets/musicas/twist_and_shout/fundo_jogo.png",
        .caminho_ranking = "assets/musicas/twist_and_shout/ranking.txt"
    };
    /* Mapas pendentes: dificuldade ainda nao classificada. */
    catalogo[4] = (Musica){
        .titulo = "I'm Still Standing",
        .filme = "Sing: Quem Canta Seus Males Espanta",
        .dificuldade = 0,
        .caminho_audio = "assets/musicas/im_still_standing/audio.wav",
        .caminho_notas = "assets/musicas/im_still_standing/notas.txt",
        .caminho_fundo = "assets/musicas/im_still_standing/fundo.png",
        .caminho_ranking = "assets/musicas/im_still_standing/ranking.txt"
    };
    catalogo[5] = (Musica){
        .titulo = "Um Sonho Eu Tenho",
        .filme = "Enrolados",
        .dificuldade = 0,
        .caminho_audio = "assets/musicas/um_sonho_eu_tenho/audio.wav",
        .caminho_notas = "assets/musicas/um_sonho_eu_tenho/notas.txt",
        .caminho_fundo = "assets/musicas/um_sonho_eu_tenho/fundo.png",
        .caminho_ranking = "assets/musicas/um_sonho_eu_tenho/ranking.txt"
    };
    /* Musicas 7 a 9: mapas do gerador offline no nivel de cada uma. */
    catalogo[6] = (Musica){
        .titulo = "Quase Lá",
        .filme = "A Princesa e o Sapo",
        .dificuldade = 2,
        .caminho_audio = "assets/musicas/Quase_la/audio.wav",
        .caminho_notas = "assets/musicas/Quase_la/notas.txt",
        .caminho_fundo = "assets/musicas/Quase_la/fundo.png",
        .caminho_ranking = "assets/musicas/Quase_la/ranking.txt"
    };
    catalogo[7] = (Musica){
        .titulo = "Um Mundo Ideal",
        .filme = "Aladdin",
        .dificuldade = 2,
        .caminho_audio = "assets/musicas/Um_mundo_ideal/audio.wav",
        .caminho_notas = "assets/musicas/Um_mundo_ideal/notas.txt",
        .caminho_fundo = "assets/musicas/Um_mundo_ideal/fundo.png",
        .caminho_ranking = "assets/musicas/Um_mundo_ideal/ranking.txt"
    };
    catalogo[8] = (Musica){
        .titulo = "De Nada",
        .filme = "Moana",
        .dificuldade = 3,
        .caminho_audio = "assets/musicas/De_nada/audio.wav",
        .caminho_notas = "assets/musicas/De_nada/notas.txt",
        .caminho_fundo = "assets/musicas/De_nada/fundo.png",
        .caminho_ranking = "assets/musicas/De_nada/ranking.txt"
    };
}

/*
 * Compara duas musicas com base no criterio escolhido.
 * Retorna true se a musica 'a' deve preceder ou ser igual a musica 'b'.
 * O uso de '<=' no empate preserva a estabilidade do Merge Sort.
 */
static bool comparar_musicas(Musica a, Musica b, CriterioOrdenacao criterio) {
    if (criterio == ORDEM_TITULO_AZ) {
        return strcmp(a.titulo, b.titulo) <= 0;
    }

    /* Sem classificacao fica ao final nos dois sentidos de dificuldade. */
    if (a.dificuldade == 0 || b.dificuldade == 0) {
        if (a.dificuldade == b.dificuldade) return strcmp(a.titulo, b.titulo) <= 0;
        return b.dificuldade == 0;
    }
    if (criterio == ORDEM_DIFICULDADE_CRESCENTE) {
        if (a.dificuldade < b.dificuldade) {
            return true;
        }
        if (a.dificuldade > b.dificuldade) {
            return false;
        }
        /* Desempate por titulo para manter ordem estavel e previsivel */
        return strcmp(a.titulo, b.titulo) <= 0;
    }

    if (criterio == ORDEM_DIFICULDADE_DECRESCENTE) {
        if (a.dificuldade > b.dificuldade) {
            return true;
        }
        if (a.dificuldade < b.dificuldade) {
            return false;
        }
        /* Desempate por titulo para manter ordem estavel e previsivel */
        return strcmp(a.titulo, b.titulo) <= 0;
    }

    return true;
}

/*
 * Intercala duas partes ordenadas do vetor: [inicio .. meio] e [meio+1 .. fim].
 * Utiliza um vetor auxiliar simples de tamanho estatico para evitar alocacao dinamica.
 */
static void intercalar_vetor_musicas(Musica catalogo[], int inicio, int meio, int fim, CriterioOrdenacao criterio) {
    Musica auxiliar[MAX_MUSICAS];

    int i = inicio;      /* Indice da primeira metade */
    int j = meio + 1;    /* Indice da segunda metade */
    int k = 0;           /* Indice do vetor auxiliar */

    /* Compara os elementos das duas metades e insere o menor no auxiliar */
    while (i <= meio && j <= fim) {
        if (comparar_musicas(catalogo[i], catalogo[j], criterio)) {
            auxiliar[k++] = catalogo[i++];
        } else {
            auxiliar[k++] = catalogo[j++];
        }
    }

    /* Copia os elementos restantes da primeira metade, se houver */
    while (i <= meio) {
        auxiliar[k++] = catalogo[i++];
    }

    /* Copia os elementos restantes da segunda metade, se houver */
    while (j <= fim) {
        auxiliar[k++] = catalogo[j++];
    }

    /* Transfere os dados intercalados de volta para o vetor original */
    for (int p = 0; p < k; p++) {
        catalogo[inicio + p] = auxiliar[p];
    }
}

/*
 * Implementacao recursiva do Merge Sort para vetor.
 * Divide o intervalo ao meio sucessivamente e depois intercala as partes.
 */
static void merge_sort_vetor_musicas(Musica catalogo[], int inicio, int fim, CriterioOrdenacao criterio) {
    if (inicio >= fim) {
        return; /* Caso base: vetor com 0 ou 1 elemento ja esta ordenado */
    }

    int meio = inicio + (fim - inicio) / 2;

    /* Divide: ordena a metade esquerda e a metade direita */
    merge_sort_vetor_musicas(catalogo, inicio, meio, criterio);
    merge_sort_vetor_musicas(catalogo, meio + 1, fim, criterio);

    /* Conquista: intercala as duas metades ordenadas */
    intercalar_vetor_musicas(catalogo, inicio, meio, fim, criterio);
}

/*
 * Funcao publica de ordenacao do catalogo.
 */
void ordenar_catalogo(Musica catalogo[], int total_musicas, CriterioOrdenacao criterio) {
    if (total_musicas > 1) {
        merge_sort_vetor_musicas(catalogo, 0, total_musicas - 1, criterio);
    }
}

/*
 * Busca o indice de uma musica no catalogo pelo seu titulo.
 */
int buscar_indice_por_titulo(Musica catalogo[], int total_musicas, const char *titulo) {
    if (titulo == NULL) {
        return -1;
    }

    for (int i = 0; i < total_musicas; i++) {
        if (strcmp(catalogo[i].titulo, titulo) == 0) {
            return i;
        }
    }

    return -1;
}
