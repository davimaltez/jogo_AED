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
As cinco pastas já contêm `notas.txt` com sequências de demonstração.
Áudios e imagens ainda precisam ser adicionados. Execute o jogo a partir
da raiz do projeto para que os caminhos relativos sejam encontrados.

## Formato dos arquivos de notas

Cada linha contém `tempo pista`, separados por espaços ou tabulações:

```text
# tempo_em_segundos pista
4.00 2
2.00 0
2.00 4
3.25 1
```

O tempo é relativo ao início da partida, em segundos. Aceita zero e números
decimais finitos não negativos, com ponto decimal e expoente opcional
(`2e0` equivale a `2.0`). A conversão deve caber em `float`, até `FLT_MAX`
de `<float.h>`; erros de faixa, inclusive valores pequenos demais para a
conversão, são rejeitados. A precisão é a do `float` usado pelo jogo.
A pista é um inteiro decimal de 0 a 4: **0=D, 1=F, 2=G, 3=J, 4=K**.

São aceitos espaços antes/depois dos campos, linhas vazias e comentários
em linhas próprias iniciadas por `#`, com espaços opcionais antes dele.
Cada linha aceita até 255 caracteres, excluindo o final de linha LF ou CRLF.
A última linha pode terminar sem quebra. Arquivos sem eventos são inválidos.
Não há limite fixo de eventos: o carregamento depende da memória disponível.
Não são aceitos bytes nulos, campos extras ou comentários após um evento.

Exemplos inválidos, cada um em sua própria linha:

```text
-1.0 0
nan 1
inf 2
1e100 0
2,5 1
2.0 5
2.0 1.5
2.0
2.0 1 extra
```

Notas simultâneas usam linhas diferentes com o mesmo tempo. Cada linha é
um evento independente; registros repetidos não são eliminados. A ordem
do arquivo pode ser qualquer uma. O exemplo é ordenado para os tempos
`2.00, 2.00, 3.25, 4.00`, mantendo a pista 0 antes da pista 4 no empate.

`Musica.caminho_notas` determina o arquivo carregado, inclusive após ordenar
o catálogo. Para cadastrar outra música, informe seu caminho nesse campo;
o carregador não depende dos nomes das músicas. A capacidade atual do
catálogo continua sendo `MAX_MUSICAS`.

## Carregamento e ordenação

`carregar_notas_arquivo` cria nós com `criar_nota`, anexando-os ao final em
O(1) por evento. Uma lista temporária conserva a ordem de leitura. Só após
ler, validar e fechar o arquivo inteiro, o Merge Sort organiza seus nós e
substitui a lista anterior. O buffer de erro é opcional; quando fornecido,
recebe a causa e a linha, se aplicável, ou fica vazio no sucesso.

O Merge Sort divide a lista com dois ponteiros, ordena recursivamente as
metades e intercala ajustando `proximo`. Escolher a metade esquerda no
empate preserva a ordem das notas simultâneas. Os nós conservam seus
endereços, pistas, tempos e estados. A ordenação custa O(n log n), usa
O(log n) de pilha e não aloca novos nós. Ela acontece uma vez por carga.

Se houver falha de abertura, leitura, fechamento, formato ou alocação, os
nós temporários são liberados. A lista anterior e os indicadores são
preservados. O início permanece na seleção; o reinício permanece na pausa
ou no Game Over. A causa aparece na tela e o caminho é registrado no
terminal. Corrija o arquivo e tente novamente. Não há substituição
automática por notas geradas.

Os cinco arquivos de demonstração reproduzem os eventos do gerador antigo,
com precisão suficiente para recuperar os mesmos valores de `float`.
Grupos de tempos aparecem em ordem inversa para demonstrar a ordenação;
a ordem das notas simultâneas é mantida. `carregar_notas_iniciais` continua
disponível para testes. Essas sequências não são transcrições das músicas.

## Status atual

Em desenvolvimento. As notas são carregadas de arquivos e ordenadas antes
da partida. A conclusão normal leva à tela de vitória/resultados. O tempo
continua avançando por `GetFrameTime`; a reprodução de áudio e a sincronização
musical não foram implementadas.

## Conclusão normal e resultados

Uma partida ativa termina normalmente quando sua lista não vazia contém
somente notas acertadas ou perdidas. `todas_notas_resolvidas` percorre a
lista sem alterar seus nós. A verificação acontece depois dos acertos,
perdas e penalizações do quadro: se o medidor chegou a zero, o Game Over
tem prioridade, inclusive quando a última nota acabou de ser resolvida.
Durante a pausa, a conclusão não é verificada.

Uma nota ainda pendente impede a vitória. O acerto mantém sua tolerância
de 0,20 s; a perda só ocorre depois de ultrapassar `tempo_alvo + 0,22 s`.
No instante exato desse limite a nota continua pendente. Notas simultâneas
precisam ser resolvidas individualmente. Se a última nota for acertada
antecipadamente dentro da janela, a conclusão já pode ocorrer. Uma sequência
curta pode ser concluída mesmo com todas as notas perdidas, desde que o
medidor continue acima de zero.

A tela **VITÓRIA / PARTIDA CONCLUÍDA** mostra a música, pontuação, acertos,
notas perdidas e maior combo. O rótulo "Notas perdidas" usa o contador
existente, que não inclui teclas pressionadas sem acerto. Nos resultados,
o relógio, a pontuação e o medidor ficam congelados. Os botões só são
ativados no quadro seguinte à conclusão, evitando aproveitar um clique
feito durante a partida.

- **JOGAR NOVAMENTE:** recarrega o arquivo e, após sucesso, restaura relógio,
  pontuação, combos, contadores, desempenho e feedback.
- **VOLTAR AO CATÁLOGO** ou **ESC:** libera a lista e retorna à seleção,
  preservando a música selecionada.

Se o recarregamento falhar, os resultados e a lista anterior permanecem
preservados, com a causa exibida na tela. Fechar o jogo também libera a lista.

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
make clean
make
make test
make test-grafico
```

`make test` executa carregamento, Merge Sort, conclusão e o teste preexistente
de desempenho, sem janela e sem raylib. Os testes compilam com `-Werror` e
exibem `OK` após as verificações. Os executáveis ficam em `/tmp/final_act_teste_*`.

`make test-grafico` executa o teste preexistente da partida e o fluxo real
de início/reinício com arquivos, usando janelas ocultas; precisa de ambiente
gráfico (por exemplo, WSLg). Entradas, relógio e falhas são simulados somente
no executável de teste, com `--wrap` do linker GNU. Nenhum arquivo do catálogo
é alterado pelos testes. Os quatro avisos de arquivo sem notas são esperados
nas tentativas de início/reinício com erro, incluindo após vitória.

Os testes cobrem arquivos inválidos e incompletos, limites de linha,
CRLF, tempos repetidos, preservação da lista anterior, falhas de alocação,
leitura e fechamento, estabilidade, estados, endereços, ausência de ciclos
e equivalência entre os cinco arquivos e o gerador original. O teste gráfico
também verifica que falhas não zeram pontuação/desempenho nem retomam uma
partida pausada ou perdida.

Os testes de conclusão cobrem última nota, ausência de vitória antecipada,
limite exato da perda, notas simultâneas parcialmente resolvidas e sequência
com todas as notas perdidas sem Game Over. O teste gráfico percorre o `main`
real para conferir resultados, prioridade da derrota, congelamento, recarga,
falha de recarga e retorno ao catálogo por botão e ESC.

Para verificar a memória das estruturas e do carregador:

```sh
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=1 /tmp/final_act_teste_carregamento_notas
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=1 /tmp/final_act_teste_merge_sort_notas
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=1 /tmp/final_act_teste_desempenho
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=1 /tmp/final_act_teste_conclusao_partida
```

Na verificação manual, execute `make run`, escolha uma música, jogue com
D/F/G/J/K e teste pausa, reinício e Game Over. Ao resolver todas as notas
sem perder a partida, confira os resultados, JOGAR NOVAMENTE e o retorno
ao catálogo por botão e ESC. Para conferir mensagens,
faça uma cópia de segurança de um `notas.txt`, introduza uma linha inválida
e tente iniciar/reiniciar; a partida deve permanecer na tela atual. Restaure
o arquivo e tente novamente. O relógio e os indicadores só devem ser zerados
após um carregamento bem-sucedido.

O desenho do brilho da pista contém uma adaptação localizada para a assinatura
de `DrawCircleGradient` da raylib 5.5 e para a versão 6 presente neste ambiente.
Consulte a [declaração oficial da raylib 5.5](https://github.com/raysan5/raylib/blob/5.5/src/raylib.h).

### Validação executada nesta implementação

- `make clean`, `make`, `make test` e `make test-grafico`: aprovados no Ubuntu 24.04/WSL.
- Compilação com `-Werror` e teste gráfico com a distribuição oficial da
  raylib 5.5: aprovados, usando cabeçalho e biblioteca estática em `/tmp`.
- Compilação e testes gráficos com a raylib 6.1-dev instalada: aprovados.
  O `pkg-config` local informa 5.5, mas seu cabeçalho/biblioteca são 6.1-dev;
  a instalação do sistema foi preservada.
- Valgrind nos quatro testes sem janela: zero erros e zero bytes em uso na saída.
- Os testes existentes de desempenho e partida continuam aprovados.

A avaliação manual de jogabilidade em uma partida completa não foi realizada.
Áudio e sincronização musical continuam fora deste escopo.

### Limitação identificada no teste gráfico com Valgrind

O fluxo gráfico completo também foi executado no Valgrind, com raylib 5.5
e `GALLIUM_DRIVER=softpipe`. Todas as verificações funcionais passaram e
não foram apontados acessos inválidos. Entretanto, o relatório de memória
não ficou zerado: foram 336 bytes diretamente perdidos e 336 indiretamente
perdidos, além de memória ainda alcançável das bibliotecas gráficas.

A mesma pilha de alocação foi reproduzida em `libgallium` (Mesa
25.2.8-0ubuntu0.24.04.4), com 56 bytes diretos e 56 indiretos por janela,
usando somente este programa, sem qualquer módulo do jogo:

```c
#include "raylib.h"
int main(void) {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(100, 100, "Janela minima");
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Teste", 10, 10, 20, WHITE);
    EndDrawing();
    CloseWindow();
    return 0;
}
```

O teste completo abre seis janelas, correspondendo aos 672 bytes reportados.
Abrir e fechar uma janela sem desenhar não reproduziu o vazamento. Os testes
de estruturas, carregamento e conclusão ficaram com zero bytes em uso na
saída. O problema no driver gráfico foi documentado e não foi alterado nesta
tarefa. Relatórios desta execução: `/tmp/final_act_valgrind_vitoria_grafico.log`
e `/tmp/final_act_valgrind_janela_renderizada.log`.
