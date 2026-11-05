#include "catalogo_musicas.h"
#include <string.h>

/*
 * Preenche o vetor com as musicas iniciais baseadas em filmes musicais.
 * Os caminhos de audio e notas sao provisorios, para utilizacao futura.
 */
void inicializar_catalogo(Musica catalogo[], int *quantidade) {
    *quantidade = 5;

    /* Musica 1 - Wicked */
    strncpy(catalogo[0].titulo, "Defying Gravity", sizeof(catalogo[0].titulo) - 1);
    catalogo[0].titulo[sizeof(catalogo[0].titulo) - 1] = '\0';
    strncpy(catalogo[0].filme, "Wicked", sizeof(catalogo[0].filme) - 1);
    catalogo[0].filme[sizeof(catalogo[0].filme) - 1] = '\0';
    catalogo[0].dificuldade = 4;
    strncpy(catalogo[0].caminho_audio, "assets/audio/defying_gravity.mp3", sizeof(catalogo[0].caminho_audio) - 1);
    catalogo[0].caminho_audio[sizeof(catalogo[0].caminho_audio) - 1] = '\0';
    strncpy(catalogo[0].caminho_notas, "assets/notas/defying_gravity.txt", sizeof(catalogo[0].caminho_notas) - 1);
    catalogo[0].caminho_notas[sizeof(catalogo[0].caminho_notas) - 1] = '\0';

    /* Musica 2 - Mamma Mia! */
    strncpy(catalogo[1].titulo, "Dancing Queen", sizeof(catalogo[1].titulo) - 1);
    catalogo[1].titulo[sizeof(catalogo[1].titulo) - 1] = '\0';
    strncpy(catalogo[1].filme, "Mamma Mia!", sizeof(catalogo[1].filme) - 1);
    catalogo[1].filme[sizeof(catalogo[1].filme) - 1] = '\0';
    catalogo[1].dificuldade = 2;
    strncpy(catalogo[1].caminho_audio, "assets/audio/dancing_queen.mp3", sizeof(catalogo[1].caminho_audio) - 1);
    catalogo[1].caminho_audio[sizeof(catalogo[1].caminho_audio) - 1] = '\0';
    strncpy(catalogo[1].caminho_notas, "assets/notas/dancing_queen.txt", sizeof(catalogo[1].caminho_notas) - 1);
    catalogo[1].caminho_notas[sizeof(catalogo[1].caminho_notas) - 1] = '\0';

    /* Musica 3 - La La Land */
    strncpy(catalogo[2].titulo, "City of Stars", sizeof(catalogo[2].titulo) - 1);
    catalogo[2].titulo[sizeof(catalogo[2].titulo) - 1] = '\0';
    strncpy(catalogo[2].filme, "La La Land", sizeof(catalogo[2].filme) - 1);
    catalogo[2].filme[sizeof(catalogo[2].filme) - 1] = '\0';
    catalogo[2].dificuldade = 1;
    strncpy(catalogo[2].caminho_audio, "assets/audio/city_of_stars.mp3", sizeof(catalogo[2].caminho_audio) - 1);
    catalogo[2].caminho_audio[sizeof(catalogo[2].caminho_audio) - 1] = '\0';
    strncpy(catalogo[2].caminho_notas, "assets/notas/city_of_stars.txt", sizeof(catalogo[2].caminho_notas) - 1);
    catalogo[2].caminho_notas[sizeof(catalogo[2].caminho_notas) - 1] = '\0';

    /* Musica 4 - The Greatest Showman */
    strncpy(catalogo[3].titulo, "This Is Me", sizeof(catalogo[3].titulo) - 1);
    catalogo[3].titulo[sizeof(catalogo[3].titulo) - 1] = '\0';
    strncpy(catalogo[3].filme, "The Greatest Showman", sizeof(catalogo[3].filme) - 1);
    catalogo[3].filme[sizeof(catalogo[3].filme) - 1] = '\0';
    catalogo[3].dificuldade = 3;
    strncpy(catalogo[3].caminho_audio, "assets/audio/this_is_me.mp3", sizeof(catalogo[3].caminho_audio) - 1);
    catalogo[3].caminho_audio[sizeof(catalogo[3].caminho_audio) - 1] = '\0';
    strncpy(catalogo[3].caminho_notas, "assets/notas/this_is_me.txt", sizeof(catalogo[3].caminho_notas) - 1);
    catalogo[3].caminho_notas[sizeof(catalogo[3].caminho_notas) - 1] = '\0';

    /* Musica 5 - High School Musical */
    strncpy(catalogo[4].titulo, "Breaking Free", sizeof(catalogo[4].titulo) - 1);
    catalogo[4].titulo[sizeof(catalogo[4].titulo) - 1] = '\0';
    strncpy(catalogo[4].filme, "High School Musical", sizeof(catalogo[4].filme) - 1);
    catalogo[4].filme[sizeof(catalogo[4].filme) - 1] = '\0';
    catalogo[4].dificuldade = 2;
    strncpy(catalogo[4].caminho_audio, "assets/audio/breaking_free.mp3", sizeof(catalogo[4].caminho_audio) - 1);
    catalogo[4].caminho_audio[sizeof(catalogo[4].caminho_audio) - 1] = '\0';
    strncpy(catalogo[4].caminho_notas, "assets/notas/breaking_free.txt", sizeof(catalogo[4].caminho_notas) - 1);
    catalogo[4].caminho_notas[sizeof(catalogo[4].caminho_notas) - 1] = '\0';
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
