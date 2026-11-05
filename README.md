# FINAL ACT

Projeto acadêmico de Algoritmos e Estruturas de Dados: um jogo de ritmo com tema de filmes musicais, desenvolvido em C com raylib.

## Como funciona

O jogador escolhe uma música e pressiona as teclas **D, F, G, J e K** quando as notas chegam à linha de acerto, acumulando pontos e combos.

## Organização

- `principal.c`: interface, menus e partida.
- `catalogo_musicas.c/.h`: catálogo e ordenação com Merge Sort.
- `lista_notas.c/.h`: gerenciamento das notas em uma lista simplesmente encadeada.
- `desempenho.c/.h`: regras do medidor e condição de Game Over.
- `instrucoes_projeto_jogo_musical/`: contexto e orientações do projeto.

## Recursos das músicas

Cada música tem uma pasta em `assets/musicas/`: `defying_gravity`,
`dancing_queen`, `city_of_stars`, `this_is_me` e `breaking_free`.
Coloque os recursos correspondentes dentro de cada pasta:

```text
assets/musicas/defying_gravity/
├── audio.mp3
├── fundo.png
└── notas.txt
```

Os caminhos ficam na struct `Musica`, no catálogo. Recursos compartilhados
podem ficar em `assets/interface/fontes/` e `assets/interface/imagens/`.
As pastas contêm apenas arquivos `.gitkeep` para serem preservadas no Git;
áudios, imagens e arquivos de notas ainda precisam ser adicionados.
O formato de `notas.txt` ainda será definido. Execute o jogo a partir da
raiz do projeto para que os caminhos relativos sejam encontrados.

## Status atual

Em desenvolvimento. As notas são geradas pelo código; a reprodução de áudio e a tela de resultados ainda não foram implementadas.

## Medidor de desempenho

O medidor semicircular aparece à esquerda das pistas. Começa em 75/100,
ganha 3 pontos por acerto e perde 8 por tecla pressionada sem acerto ou
por nota perdida. O valor fica entre 0 e 100: vermelho abaixo de 30,
amarelo de 30 a 59 e verde a partir de 60. Dez erros consecutivos desde
o início causam derrota; acertos permitem recuperar o desempenho antes disso.

Em zero, o Game Over congela a partida e oferece reinício, seleção de
músicas ou menu inicial. ESC volta à seleção. Reiniciar ou começar outra
partida restaura o medidor. Durante a pausa, o desempenho não muda.

## Compilação e testes (Linux/WSL)

Com GCC, Make e raylib instalados, compile com `make` e execute com `make run`.
Os testes abaixo não substituem a avaliação do equilíbrio em partidas reais.

```sh
gcc -Wall -Wextra -Werror -std=c99 -I. tests/teste_desempenho.c desempenho.c lista_notas.c catalogo_musicas.c -lm -o /tmp/teste_desempenho
/tmp/teste_desempenho

gcc -Wall -Wextra -Werror -std=c99 -I. tests/teste_partida.c desempenho.c lista_notas.c catalogo_musicas.c -Wl,--wrap=IsKeyPressed,--wrap=IsMouseButtonPressed,--wrap=GetFrameTime -o /tmp/teste_partida -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
/tmp/teste_partida
```

O segundo teste usa uma janela oculta e precisa de um ambiente gráfico
(por exemplo, WSLg). As entradas são simuladas apenas no executável de teste;
o jogo continua usando normalmente as entradas da raylib.

Este README foi criado apenas para teste.
