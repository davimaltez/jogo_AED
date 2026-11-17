# FINAL ACT

Twist and Shout foi adicionada ao catalogo com nivel inicial 3/5,
329 notas e oito sustentacoes. Veja os recursos, testes e limites em
[assets/musicas/twist_and_shout/README.md](assets/musicas/twist_and_shout/README.md).

Geracao automatica offline de mapas: veja
[ferramentas/gerador_mapas/README.md](ferramentas/gerador_mapas/README.md)
para compilar com `make gerador`, gerar cinco dificuldades a partir de MP3/WAV
e ajustar os parametros. O teste sintetico roda com `make test-gerador`
e tambem faz parte de `make test`. A ferramenta nao substitui mapas existentes.

Projeto acadêmico de Algoritmos e Estruturas de Dados: um jogo de ritmo com tema de filmes musicais, desenvolvido em C com raylib.

## Como funciona

O jogador escolhe uma música e pressiona as teclas **D, F, G, J e K** quando as notas chegam à linha de acerto, acumulando pontos e combos. Nas notas longas, mantém a tecla até a cauda chegar à linha.

## Navegação dos menus

- Use as setas para destacar uma opção e **Enter** para confirmar.
- No catálogo, **cima/baixo** percorrem as músicas e os demais controles;
  **esquerda/direita** percorrem os filtros. Com uma música em foco,
  **Enter** inicia a partida e **direita** leva ao botão de iniciar.
- **ESC** mantém o retorno entre telas e a pausa durante a partida.
- Pausa, Game Over, resultados e confirmação de saída também aceitam o
  teclado. A confirmação começa em **Não**.
- O mouse continua disponível; a opção em foco tem uma borda dourada clara.

## Organização dos arquivos

- `principal.c`: laço principal, entradas e transições entre telas.
- `partida.c/.h`: regras e relógio da partida, sem dependência de raylib.
- `midia_partida.c/.h`: carregamento e estados de reprodução do áudio.
- `interface.c/.h`: desenho das telas, notas e sustentações.
- `catalogo_musicas.c/.h`: catálogo e ordenação com Merge Sort.
- `lista_notas.c/.h`: gerenciamento das notas em uma lista simplesmente encadeada.
- `desempenho.c/.h`: regras do medidor e condição de Game Over.
- `instrucoes_projeto_jogo_musical/`: contexto e orientações do projeto.

## Recursos das músicas

Cada música tem uma pasta em `assets/musicas/`. O catálogo atual usa `popular`,
`dancing_queen`, `city_of_stars`, `this_is_me` e `breaking_free`.
Coloque os recursos correspondentes dentro de cada pasta:

```text
assets/musicas/dancing_queen/
├── audio.mp3
├── fundo.png
└── notas.txt
```

Os caminhos ficam na struct `Musica`, no catálogo. Recursos compartilhados
podem ficar em `assets/interface/fontes/` e `assets/interface/imagens/`.
Os demais mapas disponíveis são sequências de demonstração; Dancing Queen
tem uma adaptação difícil completa, descrita abaixo. Os recursos podem
estar incompletos. Popular usa `popular.wav`, conforme o catálogo. Execute o jogo a partir
da raiz do projeto para que os caminhos relativos sejam encontrados.

## Formato dos arquivos de notas

Cada linha contém `tempo pista [duracao]`, separados por espaços ou tabulações.
O terceiro campo é opcional; omitido ou zero representa uma nota comum:

```text
# tempo_em_segundos pista duracao_opcional
4.00 2
2.00 0
2.00 4
3.25 1
5.00 3 2.00
```

O tempo marca o acerto relativo ao início do arquivo de áudio, em segundos,
incluindo seus silêncios. Não é o instante de aparição da nota. Aceita zero e números
decimais finitos não negativos, com ponto decimal e expoente opcional
(`2e0` equivale a `2.0`). A conversão deve caber em `float`, até `FLT_MAX`
de `<float.h>`; erros de faixa, inclusive valores pequenos demais para a
conversão, são rejeitados. A precisão é a do `float` usado pelo jogo.
A pista é um inteiro decimal de 0 a 4: **0=D, 1=F, 2=G, 3=J, 4=K**.
A duração segue as mesmas regras numéricas. Uma duração positiva define uma
sustentação; `5.00 3 2.00` começa aos 5 s e termina aos 7 s. A soma precisa
ser finita e representar um instante maior que o início. Com áudio, nenhum
evento pode terminar depois da duração da faixa; essa validação ocorre antes
de substituir a partida anterior.

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

Notas simultâneas em pistas diferentes usam linhas com o mesmo tempo.
Duplicatas e sobreposições na mesma pista são rejeitadas. Uma nota pode
começar exatamente quando a sustentação anterior termina. A ordem do arquivo
pode ser qualquer uma. O Merge Sort mantém a pista 0 antes da pista 4 no
empate do exemplo, preservando a ordem relativa original.

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

Parte dos arquivos de demonstração reproduz os eventos do gerador antigo.
Grupos de tempos aparecem fora de ordem para demonstrar a ordenação;
a ordem das notas simultâneas é mantida. `carregar_notas_iniciais` continua
disponível para testes. Essas sequências não são transcrições das músicas.

## Status atual

Em desenvolvimento. O jogo inicia em janela; F11 alterna a tela cheia.
As notas são carregadas e ordenadas antes da partida. Há notas comuns e
longas, contagem inicial de 1,9 s e retomada de pausa com 1 s de preparação.
O relógio durante a reprodução vem de `GetMusicTimePlayed`, consultado uma
vez por atualização após alimentar o stream. Julgamento e desenho usam essa
mesma amostra. Pequenos recuos da leitura não fazem o jogo voltar no tempo.

O áudio possui estados separados: ausente, pronto, tocando, pausado,
finalizado e falha. Uma parada inesperada pausa a partida com aviso, sem
passar silenciosamente para o relógio de quadros. O fim natural é registrado
durante a atualização do stream sem looping; o relógio é ancorado na duração
antes da resolução das últimas janelas. Um áudio finalizado não é retomado
ao entrar no ranking. Ausência de áudio mantém um modo de demonstração,
identificado na tela, que usa `GetFrameTime` e termina pela lista.

Com exceção da adaptação de Dancing Queen, os mapas atuais são de demonstração;
o relógio correto não os transforma em transcrições das gravações. É necessário conferir
os timestamps na versão exata do áudio. BPM ajuda a preparar a grade, mas
não é necessário na execução. A precisão percebida ainda depende de
latência de áudio, tela e teclado; não há calibração de hardware implementada.

## Conclusão normal e resultados

### Dancing Queen: mapa difícil

Dificuldade 5/5, revisada para **somente teclado**, com 535 notas,
105 sustentações e 189 ataques em acorde. O mapa usa apenas os canais de
piano do MIDI, com ajustes pelos ataques no áudio. Voz e ataques genéricos
do fechamento foram removidos. Até duas teclas são exigidas simultaneamente.

O último evento termina em 227,064 s; o restante fica sem notas por falta
de referência de teclado suficientemente confirmada. A vitória ainda aguarda
o fim dos 244,227 s de áudio e a resolução das notas.
**A conferência auditiva humana ainda está pendente**; o teste automático
não comprova sincronização perceptiva perfeita.

### Condição de conclusão

Com áudio, uma partida ativa termina normalmente somente após o fim natural
da faixa e a resolução da lista não vazia. Notas em sustentação ainda não
estão resolvidas; sustentações interrompidas estão. `todas_notas_resolvidas`
percorre a lista sem alterar seus nós. A verificação acontece depois dos acertos,
perdas e penalizações do quadro: se o medidor chegou a zero, o Game Over
tem prioridade, inclusive quando a última nota acabou de ser resolvida.
Durante a pausa, a conclusão não é verificada.

Uma nota ainda pendente impede a vitória. O acerto mantém sua tolerância
de 0,20 s; a perda só ocorre depois de ultrapassar `tempo_alvo + 0,22 s`.
No instante exato desse limite a nota continua pendente. Notas simultâneas
precisam ser resolvidas individualmente. Acertar a última nota antecipadamente
não encerra o áudio. O silêncio final faz parte da duração. Após o término,
o relógio avança apenas para resolver janelas pendentes, sem novas notas
além da faixa. Uma sequência
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

## Notas longas e pontuação

Cada sustentação ocupa um único nó, com início, duração, estado e pontos já
creditados. O Merge Sort continua ordenando por início, sem copiar nós.
O pressionamento inicial conserva a janela de 0,20 s, os pontos
`100 + combo_anterior * 10`, o aumento do combo e o ganho de 3 no medidor.
Segurar antecipadamente sem um novo pressionamento não acerta a cabeça.

A sustentação soma **50 pontos por segundo**, sem multiplicador adicional,
limitados ao intervalo entre início efetivamente acertado e fim da nota.
O total é calculado a partir do tempo do áudio, não por quadro; frações
nunca são descartadas repetidamente a cada atualização. Acertar antes do
alvo não acrescenta tempo extra; acertar depois perde aquela fração inicial.

Soltar antes do fim interrompe a nota, zera o combo e retira **8 do medidor
uma única vez**. Os pontos já ganhos ficam; não há retomada da mesma nota.
A tolerância adicional de soltura é zero nesta versão. Soltar no instante
final é permitido. A leitura é por quadro: no quadro em que se observa uma
soltura, o intervalo desde a última amostra segurada não recebe pontos,
pois não sabemos o instante exato da soltura. Travamentos e entradas
inteiras entre quadros não podem ser reconstruídos com precisão.

Pausa e preparação não pontuam nem penalizam. Ao continuar, há 1 s para
reposicionar as teclas das sustentações antes de o áudio voltar. Reiniciar
recarrega notas e zera todos os estados. Os resultados mostram sustentações
completas e interrompidas separadamente: "Acertos" inclui as cabeças
acertadas; "Notas perdidas" inclui apenas cabeças não acertadas.
O ranking mantém seu formato e seus registros; pontuações de mapas ou
regras diferentes não são diretamente comparáveis.

## Verificação desta implementação

Para preservar um executável existente, compile em outro destino:

```sh
make TARGET=/tmp/final_act_teste_sustain
/tmp/final_act_teste_sustain
make test
make test-grafico
# Teste opcional de toda a faixa Dancing Queen, com volume zero:
/tmp/final_act_teste_partida audio-completo
```

Execute na raiz do projeto no Linux/WSL. Variáveis locais de GPU podem ser
aplicadas ao comando de execução, mas não fazem parte do código/Makefile.
Os testes usam notas, ranking e WAV temporários em `/tmp`; não editam os
recursos do catálogo. O teste gráfico exige janela e dispositivo de áudio.

`teste_sustentacao.c` cobre pontos em diferentes divisões de quadros, acerto
antecipado/tardio, interrupção, acordes mistos, pausa, preparação e conclusão.
O teste gráfico percorre uma faixa WAV de 4 s com silêncio final, verifica
sustentação com pausa, erro de mapa no reinício, última janela de julgamento,
parada inesperada e layouts de 640x360 até 1920x1080.
Esses testes não substituem ouvir e jogar os mapas reais, avaliar latência,
balanceamento e desempenho em tela cheia.

Verificado no WSL Ubuntu em 13/11/2026: compilação, `make test`,
`make test-grafico` e teste integral do MP3 Dancing Queen (244,227 s), com
volume zero e mapa temporário. O relógio não retrocedeu e a vitória aguardou
o fim da faixa. A suíte de sustentação também passou com AddressSanitizer e
UndefinedBehaviorSanitizer. A captura de sustentação em 900x600 foi inspecionada.
O teste de catálogo avisa e pula `breaking_free/notas.txt`, que não está
disponível. Não foram alterados mapas, áudios ou rankings do catálogo.

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
Esse registro histórico antecede o controle de áudio e as notas longas
descritos nas seções de status e verificação acima.

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

## Integracao de duas musicas com mapas pendentes (17/11/2026)

O catalogo agora tem oito musicas, preservando as seis anteriores:

| Titulo | Artista | Filme confirmado | Pasta |
| --- | --- | --- | --- |
| I'm Still Standing | Taron Egerton | Sing: Quem Canta Seus Males Espanta | assets/musicas/im_still_standing/ |
| Um Sonho Eu Tenho | Mauro Ramos | Enrolados | assets/musicas/um_sonho_eu_tenho/ |

Os WAV foram copiados de /mnt/c/Users/gabri/OneDrive/Desktop/musicasjogodepif/,
mantendo os nomes originais na origem e usando audio.wav nas pastas do jogo.
As copias tiveram SHA-256 identico aos originais. Nenhum arquivo de origem
foi modificado ou removido. Ambos sao PCM stereo, 44100 Hz, 16 bits;
duracoes informadas pela raylib: 187,951 s e 192,235 s, respectivamente.

A dificuldade interna 0 significa **A definir**, sem representar um nivel
jogavel. Nas duas ordenacoes por dificuldade, entradas pendentes ficam ao
final, em ordem de titulo. O Merge Sort e o vetor de Musica foram mantidos.
Os artistas ficam documentados aqui, sem acrescentar campos a struct.

Os caminhos de notas.txt, fundo.png e ranking.txt estao cadastrados,
mas esses arquivos ainda nao existem nas duas pastas. A interface avisa
"Mapa de notas pendente"; Enter e INICIAR PARTIDA continuam usando o
carregamento validado, que rejeita a ausencia e conserva a tela e o estado.
Nao foram criados mapas vazios, ficticios ou candidatos nesta integracao.
Fundo e opcional no sistema atual. Ranking ausente aparece vazio e sera
gravado no caminho individual quando houver uma vitoria registrada.

Para tornar cada faixa jogavel, preparar um notas.txt real, nao vazio,
no formato tempo pista [duracao], revisado contra o audio exato. O mapa
deve passar pelo carregador e pela validacao da duracao, incluindo caudas.
Somente apos revisao atribuir dificuldade de 1 a 5. Adicionar uma imagem
PNG valida em fundo.png quando disponivel.

A ferramenta offline existente aceita WAV e pode gerar candidatos numa
etapa posterior, sem alterar a reproducao ou os mapas publicados:

    make gerador
    /tmp/final_act_gerador_mapas assets/musicas/im_still_standing/audio.wav /tmp/mapas_im_still_standing
    /tmp/final_act_gerador_mapas assets/musicas/um_sonho_eu_tenho/audio.wav /tmp/mapas_um_sonho_eu_tenho

Cada destino precisa ser uma pasta nova. Os cinco candidatos exigem revisao
auditiva e escolha manual antes de publicar notas.txt; a geracao nao
garante sincronizacao musical nem isolamento de instrumentos.

Validacao executada no WSL Ubuntu nesta integracao:

- make TARGET=/tmp/final_act_integracao: compilacao aprovada, preservando
  o executavel preexistente final_act.
- make test: aprovado, incluindo catalogo, carregador, estruturas, regras,
  ranking, sustentacoes, Dancing Queen e sinais sinteticos do gerador.
  Os mapas ausentes de Breaking Free e das duas novas musicas tiveram a
  rejeicao verificada; isso nao valida mapas que ainda nao existem.
- make test-grafico: aprovado. Teclado e mouse percorrem oito itens,
  filtros conservam a selecao e iniciar sem mapa conserva o estado.
  WAV reais abertos com o modulo existente, com reproducao breve em volume
  zero, relogio, pausa, retomada, reinicio e liberacao verificados.
  O teste WAV sintetico existente verifica fim integral e conclusao.
- /tmp/final_act_teste_partida catalogo: aprovado; gera capturas em
  /tmp/final_act_catalogo_*.png. Inspecionadas visualmente as capturas de
  640x360 (I'm Still Standing) e 1280x720 (Um Sonho Eu Tenho).
  Cartoes baixos mostram apenas o titulo; o painel direito conserva os
  detalhes, mantendo o estilo visual.
- A primeira tentativa de captura por TakeScreenshot nao gravou os arquivos
  por tratar o destino como relativo. O teste foi corrigido para
  LoadImageFromScreen/ExportImage, com verificacao do retorno, e reexecutado.

A raylib em execucao informou 6.1-dev (pkg-config informa 5.5).
Nao foi feita avaliacao auditiva, nem reproducao integral dos dois novos
WAV. Sem mapas, nao ha validacao de sincronizacao musical dessas faixas.
Audio, relogio, contagem regressiva, pontuacao, Game Over, sustentacoes,
conclusao, lista encadeada e rankings existentes nao tiveram suas regras
alteradas.

## Mapas automaticos integrados - 17/11/2026

Esta etapa substitui o estado anterior de mapas pendentes para I'm Still
Standing e Um Sonho Eu Tenho. Os filmes, artistas, oito registros do
catalogo e dificuldade 0 / A definir permanecem como cadastrados.

Foram gerados cinco candidatos para cada WAV com o gerador existente,
sem alteracoes em codigo ou parametros. Os candidatos ficam em
assets/musicas/im_still_standing/mapas_gerados/ e
assets/musicas/um_sonho_eu_tenho/mapas_gerados/.
Cada pasta contem notas_1.txt ate notas_5.txt; o notas.txt publicado fica
na pasta da musica, separado dos candidatos.

| Musica | Preset escolhido | Notas | Sustentacoes | Notas/s | Primeiro ataque | Ultimo fim | Duracao do WAV |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| I'm Still Standing | 3 | 414 | 1 | 2,203 | 1,927256 s | 181,394287 s | 187,951 s |
| Um Sonho Eu Tenho | 2 | 310 | 7 | 1,613 | 2,054966 s | 188,499588 s | 192,235 s |

A escolha considerou ataques detectados, densidade local, folga para
repetir teclas, sustentacoes e simultaneidade. Nao houve conflitos na
mesma pista. I'm Still Standing exige ate uma tecla por vez e quatro
ataques em uma janela de 1 s; Um Sonho Eu Tenho, ate duas teclas e tres
ataques em uma janela de 1 s. Todos usam as cinco pistas, com no maximo
duas repeticoes consecutivas. Isso indica viabilidade estrutural, nao
comprova conforto humano ou sincronizacao musical perfeita.

O corte inicial de 1,5 s do gerador foi preservado. Ha sinal nos dois
audios: em I'm Still Standing nao houve picos filtrados nesse trecho;
em Um Sonho Eu Tenho houve seis picos harmonicos/percussivos, alguns
possivelmente do mesmo evento, entre 0,232 e 1,115 s. Nenhum evento foi
acrescentado artificialmente. Cada mapa escolhido tem tres eventos
provenientes exclusivamente da grade estimada. Os instantes, as tabelas
dos cinco presets, a distribuicao por pista, as lacunas, as sustentacoes,
a energia das bordas e os hashes estao nos README.md de cada musica.
Esses pontos precisam de revisao auditiva; o fim de Um Sonho Eu Tenho
ainda tem energia e um ataque filtrado a 188,639 s, proximo demais do
anterior para o preset escolhido.

Validacoes executadas:
- Os dez candidatos e os dois notas.txt instalados passaram pelo parser
  real e pela validacao de duracao usando os WAV.
- Simulacoes das regras reais a 30, 60, 144 e 240 Hz: todas as notas
  acertadas, todas as sustentacoes completas, nenhuma derrota nem
  conclusao antes do fim simulado.
- make TARGET=/tmp/final_act_mapas: aprovado, executavel antigo preservado.
- make test e make test-grafico: aprovados. Os mapas novos sao carregados
  automaticamente pela verificacao existente do catalogo.
- Breaking Free continua com mapa ausente; a rejeicao e verificada, sem
  afirmar jogabilidade para um recurso ausente. Outros recursos ausentes
  tambem nao foram corrigidos.

Nao foi necessario modificar testes existentes nem Makefile. Programas
temporarios de diagnostico, estatisticas e validacao ficam em
/tmp/final_act_analise_novas/. Para o teste integral, foi compilada uma
copia temporaria do teste existente, adaptando a selecao da musica
por variavel de ambiente e o caminho de inclusao, com diagnosticos e
prazo de seguranca descritos no resultado abaixo. As regras executadas
sao os mesmos modulos de producao. Nenhum ranking real e gravado.

Para avaliar manualmente, na raiz do projeto pelo WSL:

    make TARGET=/tmp/final_act_mapas
    /tmp/final_act_mapas

Escolha JOGAR, selecione a musica e pressione Enter. Use D/F/G/J/K,
segurando nas notas longas; ESC pausa. O jogo aguarda o fim do WAV mesmo
depois da ultima nota. A dificuldade continua A definir. Os fundos
especificos estao ausentes; isso nao impede a partida.

Nenhuma escuta humana foi realizada nesta etapa. Lista, Merge Sort,
audio, relogio, pontuacao, pausa, reinicio, Game Over, conclusao e mapas
anteriores foram preservados. Nao houve commit ou push.

### Resultado dos testes integrais com audio real

As repeticoes isoladas passaram pelo fluxo real de inicio e reproducao,
usando os notas.txt publicados e entradas automatizadas em volume zero:

- I'm Still Standing: 414/414 notas, 1/1 sustentacao completa, zero perdas,
  zero interrupcoes; estado AUDIO_FINALIZADO e tela de registro de ranking
  somente depois de 187,951 s de audio.
- Um Sonho Eu Tenho: 310/310 notas, 7/7 sustentacoes completas, zero perdas,
  zero interrupcoes; estado AUDIO_FINALIZADO e tela de registro de ranking
  somente depois de 192,235 s de audio. Tempo real observado: 205,272 s.

As primeiras execucoes, simultaneas, falharam na verificacao final de
estado. Nao houve diagnostico suficiente nelas para afirmar a causa exata.
Na repeticao isolada, foram acrescentadas mensagens de estado ao auxiliar
temporario. Observou-se diferenca entre tempo real e relogio de reproducao
no WSL. Para a segunda faixa, ampliou-se apenas o prazo de seguranca do
teste de duracao+15 s para 2*duracao+30 s; todas as verificacoes de estado,
notas, sustentacoes e ausencia de vitoria antecipada foram mantidas.
O relogio e os modulos do jogo nao foram modificados. O teste nao comprova
ausencia de artefatos audiveis nem mede latencia do equipamento.

Uma compilacao inicial do auxiliar de analise falhou por omitir bibliotecas
de ligacao da raylib. Foi repetida com as mesmas bibliotecas do Makefile,
sem instalar dependencias nem modificar o gerador.

Os 71 arquivos preexistentes de codigo, testes, Makefile, executavel e
recursos incluidos na verificacao SHA-256 permaneceram identicos.
Nao foram gravados rankings reais. Nenhum arquivo de origem foi alterado.
A avaliacao auditiva e de conforto humano permanece pendente.
