# Arquivo de contexto do projeto --- Jogo de ritmo com tema de musicais

> Documento de referência para uma IA auxiliar no planejamento,
> desenvolvimento, documentação e revisão do projeto.
>
> **Status:** especificação inicial em construção. Diferencie decisões
> já tomadas de sugestões ainda não confirmadas.

## 1. Visão geral

O projeto é um jogo chamado "FINAL ACT", de ritmo inspirado na jogabilidade de **Guitar
Hero**, mas com temática centrada em **filmes musicais**. O jogador
escolhe músicas de diferentes filmes e precisa pressionar as teclas
correspondentes no momento correto, acompanhando notas que aparecem e se
deslocam pela tela em sincronia com a música.

A proposta é desenvolver o jogo em **linguagem C**, com foco acadêmico
em Algoritmos e Estruturas de Dados (AED), sem perder de vista uma
interface visual atraente e uma experiência de jogo funcional.

Exemplos de filmes que podem compor o catálogo: *Wicked*, *Mamma Mia!*,
*La La Land*, *High School Musical* e *The Greatest Showman*. São
referências iniciais, não uma lista fechada nem uma confirmação de que
todas as músicas serão incluídas.

## 2. Conceito e ciclo principal de jogo

O fluxo esperado é:

1.  O jogador abre o jogo e chega à tela inicial.
2.  A tela inicial apresenta um catálogo de músicas.
3.  O jogador pode escolher como ordenar o catálogo:
    -   ordem alfabética pelo título;
    -   dificuldade crescente;
    -   dificuldade decrescente.
4.  O jogador seleciona uma música.
5.  O jogo carrega os dados das notas dessa música e os prepara para a
    partida.
6.  Durante a música, as notas aparecem em pistas/colunas e se deslocam
    em direção à zona de acerto.
7.  O jogador pressiona a tecla correspondente no momento adequado.
8.  Acertos aumentam a pontuação e podem manter ou ampliar um combo;
    erros podem interromper o combo.
9.  Ao final, o jogo apresenta o resultado da partida.

A ideia inicial prevê quatro pistas/teclas (`D`, `F`, `J` e `K`), como
referência de implementação. Essa quantidade e esse mapeamento ainda
podem ser ajustados durante o desenvolvimento.

## 3. Decisões de arquitetura já estabelecidas

### 3.1 Linguagem e interface

-   **Linguagem principal:** C.
-   **Biblioteca gráfica planejada:** raylib.
-   A raylib será usada para desenhar menus, catálogo, pistas, notas,
    indicadores de pontuação/combos e tela de resultados.
-   A reprodução de áudio deverá ser sincronizada com o tempo das notas.

A raylib é a escolha de implementação planejada até o momento. Se houver
uma decisão posterior diferente, atualizar este documento.

### 3.2 Estrutura principal: lista simplesmente encadeada

A estrutura de dados central da jogabilidade será uma **lista
simplesmente encadeada de notas**.

Cada nó representará uma nota e deverá armazenar, no mínimo:

-   instante em que a nota deve ser tocada (tempo relativo ao início da
    música);
-   pista ou tecla correspondente;
-   posição vertical ou informação suficiente para calcular sua posição
    na tela;
-   estado da nota, se necessário (por exemplo, pendente, acertada ou
    perdida);
-   ponteiro para o próximo nó.

A lista será percorrida durante a partida. O jogo consulta as próximas
notas, determina quando devem aparecer, verifica as tentativas do
jogador e remove ou marca as notas que já foram resolvidas.

A lista precisa participar efetivamente da lógica central do jogo; não
deve ser usada apenas como armazenamento decorativo. As operações
previstas incluem criação, inserção, percurso, busca/verificação,
atualização, remoção e liberação de memória.

**Observação de implementação:** a posição na tela pode ser calculada a
partir do tempo da nota e do tempo atual da música. Não é obrigatório
armazenar `posicao_y` se isso gerar redundância. A representação final
da struct deve ser decidida durante a implementação.

### 3.3 Catálogo: vetor de structs de músicas

O catálogo de músicas será armazenado em um **vetor de structs
`Musica`**. Cada registro deverá conter informações como:

-   título da música;
-   filme de origem;
-   nível de dificuldade;
-   caminho do arquivo de áudio;
-   caminho do arquivo que contém os eventos/notas da música.

O vetor será ordenado de acordo com o filtro selecionado pelo jogador. O
vetor não substitui a lista encadeada como estrutura central da partida:
cada estrutura tem uma responsabilidade diferente.

### 3.4 Algoritmo de ordenação: Merge Sort

O algoritmo de ordenação escolhido é o **Merge Sort**. Ele terá duas
aplicações planejadas:

**A. Ordenação do catálogo de músicas**

O jogador escolhe um critério na tela inicial: - título em ordem
alfabética (A--Z); - dificuldade crescente; - dificuldade decrescente.

O Merge Sort ordena o vetor de músicas de acordo com o critério
escolhido. Para comparar títulos, pode-se utilizar `strcmp`; para
dificuldade, compara-se o valor numérico do nível.

**B. Ordenação das notas da música**

Ao carregar uma música, os eventos de notas serão organizados pelo
instante de execução, em ordem cronológica. Para isso, será implementada
uma versão de Merge Sort adaptada à lista simplesmente encadeada,
reorganizando os ponteiros dos nós.

Essa segunda aplicação prepara a sequência que será consumida pela
lógica da partida. O algoritmo deve ter uma função real: ordenar eventos
carregados, que não precisam estar previamente em ordem. Não se deve
alegar que o Merge Sort é indispensável se os arquivos forem sempre
gravados e garantidos em ordem; nesse caso, a necessidade da ordenação
precisa ser reavaliada ou os dados de entrada devem permitir demonstrar
a operação de forma legítima.

**Importante:** a ordenação do vetor e a ordenação da lista são
implementações distintas do mesmo algoritmo. Não presumir que uma única
função genérica em C resolverá ambos os tipos de estrutura. O princípio
divide-e-conquista é o mesmo, mas a manipulação dos dados muda.

Complexidade esperada do Merge Sort: `O(n log n)` em tempo. Na
implementação em lista encadeada, a divisão e intercalação devem
manipular os ponteiros; na implementação em vetor, será necessário
espaço auxiliar para a intercalação.

## 4. Dificuldade das músicas

Para a primeira versão, a dificuldade pode ser atribuída manualmente a
cada música em uma escala de **1 a 5**. A classificação deverá ser
definida pelo grupo com base em fatores como:

-   quantidade de notas;
-   densidade de notas por segundo;
-   velocidade e frequência das sequências;
-   presença de trechos mais exigentes.

A escala manual é uma decisão simples e viável para um catálogo pequeno.
Uma possível evolução seria calcular um indicador de dificuldade
automaticamente a partir dos dados das notas, mas isso não é requisito
inicial e não deve ser tratado como funcionalidade já implementada.

## 5. Funcionalidades previstas

### Essenciais para a primeira versão

-   Menu/tela inicial.
-   Catálogo de músicas.
-   Filtro de ordenação alfabética.
-   Filtro de dificuldade crescente e decrescente.
-   Seleção de música.
-   Carregamento dos dados de notas.
-   Preparação/ordenação cronológica das notas com Merge Sort.
-   Pistas e notas visuais.
-   Leitura das teclas do jogador.
-   Verificação de acertos e erros.
-   Pontuação e combo.
-   Tela de resultado.
-   Liberação correta da memória alocada.

### Escopo recomendado

Manter a primeira versão controlada: poucas músicas, quatro pistas e
notas simples. Evitar adicionar antes de concluir o ciclo principal
funcionalidades como editor de chart, modo de treino com navegação,
multiplayer, loja, sistema online ou inteligência artificial.

## 6. Funções previstas para a lista encadeada

As funções podem ser nomeadas e organizadas de acordo com o padrão do
código, mas o projeto deverá ter operações claras sobre a estrutura. Uma
lista inicial:

1.  `criar_nota`: aloca e inicializa um nó.
2.  `inserir_nota`: insere uma nota na lista.
3.  `carregar_notas`: lê os dados de uma música e monta a lista.
4.  `dividir_lista`: divide a lista em duas partes para o Merge Sort.
5.  `intercalar_listas`: intercala duas listas ordenadas.
6.  `merge_sort_notas`: ordena as notas pelo instante de execução.
7.  `atualizar_notas`: percorre as notas e atualiza sua situação em
    relação ao tempo da música.
8.  `verificar_acerto`: verifica se a tecla pressionada corresponde a
    uma nota dentro da janela de acerto.
9.  `remover_nota`: remove uma nota resolvida, se essa for a estratégia
    escolhida.
10. `liberar_lista`: libera todos os nós restantes.

Também existirão funções para ordenar o vetor de músicas com Merge Sort,
comparar músicas por título/dificuldade, desenhar a interface,
reproduzir/controlar áudio e atualizar pontuação.

O roteiro acadêmico solicita pelo menos seis funções que envolvam a
estrutura escolhida. As funções acima são uma proposta para demonstrar
operações reais da lista, não uma exigência de manter exatamente esses
nomes.

## 7. Sincronização de áudio e notas

A posição das notas na tela deve ser calculada com base no tempo atual
da música, e não apenas na quantidade de frames ou na velocidade do loop
principal. Isso evita que as notas percam sincronia quando o FPS variar.

Cada nota possui um tempo-alvo. A lógica do jogo compara esse tempo com
o tempo atual da reprodução para decidir: - quando a nota entra na área
visível; - quando chega à zona de acerto; - se o jogador acertou dentro
da janela permitida; - quando a nota deve ser considerada perdida.

Os valores de velocidade de deslocamento, antecedência de aparição e
janela de acerto ainda precisam ser definidos e testados.

## 8. Requisitos acadêmicos de AED

O projeto está sendo planejado para uma atividade de Algoritmos e
Estruturas de Dados. As decisões tomadas procuram atender aos seguintes
pontos do roteiro da atividade:

-   utilizar uma estrutura de dados estudada na disciplina, com listas
    e/ou árvores entre as categorias mencionadas;
-   fazer a estrutura participar da lógica central do jogo;
-   utilizar pelo menos um algoritmo de ordenação estudado;
-   não restringir a ordenação ao ranking;
-   implementar pelo menos seis funções que envolvam a estrutura de
    dados;
-   desenvolver uma interface gráfica.

A proposta escolhida utiliza lista simplesmente encadeada como estrutura
central e Merge Sort como algoritmo de ordenação, aplicado ao catálogo e
à sequência de notas.

**Atenção:** antes de afirmar que a implementação está totalmente
aderente ao conteúdo ministrado, confirmar nos materiais de aula se
Merge Sort e a variação específica de lista escolhida foram efetivamente
abordados/aceitos pelo professor. O roteiro, por si só, não confirma
quais algoritmos específicos foram ensinados.

## 9. Restrições e princípios para orientar a IA

Ao ajudar neste projeto, a IA deve:

-   considerar que o desenvolvedor é estudante de Ciência da Computação
    no terceiro período;
-   explicar conceitos de forma didática, natural e compatível com esse
    nível;
-   priorizar soluções que o estudante consiga entender, implementar e
    defender;
-   não introduzir estruturas ou algoritmos complexos sem justificar sua
    necessidade;
-   não sugerir adicionar uma estrutura apenas para aumentar
    artificialmente a complexidade;
-   manter C como linguagem principal e considerar a raylib para
    gráficos;
-   distinguir claramente o que já foi decidido, o que é sugestão e o
    que ainda precisa ser definido;
-   não inventar requisitos do professor nem afirmar que algo está no
    PDF sem confirmação;
-   ao revisar código, explicar o raciocínio e propor mudanças
    incrementais;
-   não entregar uma solução completa quando o estudante estiver
    tentando aprender; preferir orientar por etapas, pistas e exemplos
    pequenos;
-   preservar a arquitetura acordada, mas apontar problemas técnicos
    quando houver justificativa.

## 10. Decisões ainda em aberto

Não tratar os itens abaixo como decisões já tomadas:

-   nome definitivo do jogo;
-   identidade visual, paleta de cores e estilo da interface;
-   quantidade final de músicas;
-   músicas e arquivos de áudio que serão utilizados;
-   formato de futuras extensões dos arquivos de notas (o formato atual está definido abaixo);
-   valores dos níveis de dificuldade para cada música;
-   quantidade definitiva de pistas/teclas;
-   janela de acerto e regras exatas de pontuação;
-   se notas serão removidas da lista ou apenas marcadas como
    resolvidas;
-   funcionalidades extras após a primeira versão.

## Organização dos recursos por música (decisão confirmada)

Cada música possui uma pasta própria em `assets/musicas/<nome_da_musica>/`,
com os caminhos previstos para `audio.mp3`, `fundo.png` e `notas.txt`.
A struct `Musica` guarda esses caminhos nos campos `caminho_audio`,
`caminho_fundo` e `caminho_notas`. Recursos compartilhados ficam em
`assets/interface/fontes/` e `assets/interface/imagens/`.

Essa organização não altera a lista encadeada nem o Merge Sort.
As pastas contêm arquivos `notas.txt` de demonstração. O carregamento das
notas está implementado; áudio e fundo são carregados por `midia_partida.c`
quando os arquivos existem (ver a seção de organização e áudio abaixo).

## Arquivos de notas e preparação da partida (decisão confirmada)

Cada linha de `notas.txt` contém `tempo pista [duracao]`: tempo decimal em segundos,
finito, não negativo e representável em `float`; pista inteira de 0 a 4,
correspondente a D, F, G, J e K. Espaços e tabulações separam os campos.
Linhas vazias e comentários em linhas próprias iniciadas por `#` são aceitos.
O terceiro campo opcional indica a duração da sustentação; omitido ou zero
mantém a nota comum. Duração e instante final precisam ser finitos e válidos.
Sobreposições e duplicatas na mesma pista são rejeitadas após a ordenação;
acordes em pistas diferentes são aceitos. Com áudio, o início/reinício também
valida se o fim de cada evento cabe na faixa antes de trocar a partida.
O limite é 255 caracteres por linha, sem contar LF/CRLF. Arquivos sem notas,
campos extras, bytes nulos e conversões numéricas com erro são rejeitados.
O README documenta limites e exemplos completos.

`carregar_notas_arquivo` usa `Musica.caminho_notas` para montar uma lista
temporária, valida a leitura completa e aplica Merge Sort estável por tempo,
reorganizando os ponteiros dos nós. Só depois substitui a lista anterior.
Falhas liberam a lista temporária e preservam a lista e os indicadores da
partida. Início e reinício só avançam em caso de sucesso; erros aparecem na
tela e no terminal. Não existe fallback automático para notas geradas.

Parte dos arquivos de demonstração contém eventos fora de ordem, reproduzindo
tempos e pistas do gerador anterior. Dancing Queen agora tem uma adaptação
difícil específica, descrita abaixo. O gerador foi mantido para testes. A ordenação
ocorre uma vez por carregamento, antes da utilização da lista na partida.
O relógio segue o áudio quando carregado; uma parada inesperada bloqueia a
partida com aviso, sem trocar o relógio para `GetFrameTime`. O modo sem áudio
é uma demonstração identificada na tela. Após o fim natural, o relógio é
ancorado na duração e avança apenas para resolver janelas de julgamento finais.

## Medidor de desempenho (implementação solicitada)

O módulo `desempenho.c/.h` mantém um indicador de 0 a 100, inicialmente em
75. Cada acerto soma 3; cada tecla sem acerto ou nota perdida subtrai 8.
As regiões são vermelho abaixo de 30, amarelo de 30 a 59 e verde de 60 a
100. Ao atingir zero, a partida é encerrada por excesso de erros.
O Game Over reutiliza as ações de reinício e retorno da pausa, sem permitir
continuar a partida perdida. Nova partida e reinício restauram o medidor.
Esses valores iniciais podem ser ajustados após avaliação de jogabilidade.
Lista encadeada, catálogo em vetor e Merge Sort permanecem preservados.

## Conclusão normal e tela de resultados (implementação confirmada)

A partida com áudio é concluída somente depois do fim natural da faixa e
da resolução da lista não vazia, sem Game Over. Sem áudio, permanece a
conclusão pela lista. Sustentações ativas impedem a vitória; completas ou
interrompidas são resolvidas. A consulta
`todas_notas_resolvidas` não altera os nós. A decisão ocorre depois dos
acertos, perdas e penalizações do quadro, com prioridade para a derrota.
Durante a pausa, não ocorre conclusão. Uma nota pendente, inclusive em um
acorde, impede a vitória; as janelas existentes de acerto (0,20 s) e perda
(tempo estritamente maior que alvo + 0,22 s) são preservadas.

`TELA_RESULTADOS` congela a partida e apresenta vitória, música, pontuação,
acertos, notas perdidas e maior combo, usando o fundo teatral e as cores
existentes. "Notas perdidas" não inclui teclas pressionadas sem acerto.
Concluir significa sobreviver aos eventos: uma sequência curta com todas
as notas perdidas pode terminar normalmente se o medidor não chegar a zero.

JOGAR NOVAMENTE reutiliza o recarregamento validado e só restaura os dados
depois do sucesso. Falhas mantêm os resultados e exibem a causa. VOLTAR AO
CATÁLOGO e ESC liberam a lista e retornam à seleção. O fechamento do jogo
também libera os nós. Carregador, Merge Sort, regras de pontuação e medidor
permanecem com suas implementações anteriores. Depois da vitória, o jogador
passa pela tela de registro do nome no ranking antes dos resultados.

## Navegação por teclado (implementação confirmada)

Os menus aceitam setas para mover o foco e Enter para confirmar, mantendo
o mouse e o ESC. O foco recebe uma borda dourada clara e é reiniciado na
troca de menu. No catálogo, cima/baixo percorrem os controles e músicas,
esquerda/direita percorrem os filtros e direita numa música acessa iniciar.
Enter numa música também inicia a partida usando o carregamento validado.
Pausa, Game Over e resultados reutilizam as ações existentes. A confirmação
de saída começa em Não e bloqueia as entradas do menu que fica atrás dela.

## Organização, áudio, ranking e interface (implementação confirmada)

Decisões aprovadas pela estudante em 11/11/2026:

-   **Módulos:** `principal.c` (struct `Jogo`, laço e transições),
    `interface.c` (telas), `partida.c` (regras sem raylib),
    `midia_partida.c` (áudio e fundo) e `ranking.c`. Não há variáveis
    globais mutáveis; o estado fica nas structs `Jogo`, `Partida` e
    `MidiaPartida`.
-   **Reinicialização centralizada:** `reiniciar_estado_partida` é o único
    lugar que zera tempo, pontos, combos, contadores, medidor, pausa e
    feedback. `preparar_partida` carrega as notas e só reinicia em caso de
    sucesso; é usada para iniciar, reiniciar e jogar novamente.
-   **Atualização separada da renderização:** cada quadro executa
    atualizar (ESC, tempo, regras, digitação), desenhar e aplicar a ação
    escolhida no menu. A partida é atualizada por `atualizar_partida`,
    que não desenha; `desenhar_tela_partida` só desenha. Os menus seguem o
    padrão de interface imediata da raylib (o botão é desenhado e consultado
    no mesmo lugar), mas apenas devolvem uma `AcaoMenu`; a mudança de estado
    ocorre em `aplicar_acao`.
-   **Áudio:** o tempo da partida vem de `GetMusicTimePlayed` enquanto a
    música toca; sem áudio, de `GetFrameTime`. Pausa e Game Over pausam o
    áudio; reiniciar volta notas e áudio a zero juntos.
-   **Ranking:** por música, top 10 em `assets/musicas/<pasta>/ranking.txt`
    (`pontuacao;nome`), apenas vitórias. Ordenado com Merge Sort em vetor:
    pontuação decrescente, empate por nome A-Z, estável. Esta é a terceira
    aplicação do Merge Sort (catálogo, notas e ranking); a ordenação não
    fica restrita ao ranking.
-   **Janelas menores:** escala calculada sobre a referência 1280x720,
    tamanho mínimo de janela 640x360.

## 11. Resumo rápido da arquitetura

### Atualização de 13/11/2026: duração integral e sustentações

- A reprodução tem estados explícitos em `MidiaPartida`: ausente, pronta,
  tocando, pausada, finalizada e falha. O fim natural é registrado durante
  `UpdateMusicStream`; não depende de igualdade entre tempo e duração.
- Início/reinício têm preparação de 1,9 s, com notas se aproximando antes do
  áudio. Retomar a pausa oferece 1 s para reposicionar as teclas. Nenhuma
  preparação pontua. Durante a reprodução, regras e desenho compartilham
  uma amostra de `GetMusicTimePlayed` por atualização.
- Cada nota longa continua sendo um nó da lista. A cabeça usa as regras
  anteriores; a sustentação vale 50 pontos por segundo efetivamente
  reconhecido, sem bônus de combo adicional. Soltar antes do fim interrompe
  a nota, zera combo e aplica uma única perda de 8 no medidor, sem retomada.
  Soltura no fim é válida; tolerância adicional de soltura: zero.
- Pontos são calculados pelo total de tempo, limitados ao início/fim, sem
  arredondamento por quadro. Um intervalo cuja soltura só foi observada no
  quadro seguinte não ganha pontos retroativos. A precisão de entrada é
  limitada pela leitura por quadro; não se promete reconstrução de entradas
  durante travamentos. Não há calibração de latência de hardware ainda.
- Resultados distinguem sustentações completas e interrompidas. Acertos
  contam cabeças acertadas; notas perdidas contam cabeças não acertadas.
- A lista, os Merge Sort de notas/catálogo/ranking e os arquivos de ranking
  são preservados. Os demais mapas continuam sendo demonstrações; Dancing
  Queen recebeu a adaptação descrita abaixo. Avaliar latência/jogabilidade
  ainda exige conferência musical.
- O jogo inicia em janela e mantém F11. O executável preexistente não é
  substituído por testes; destinos de compilação separados ficam em `/tmp`.

### Síntese

#### Dancing Queen — atualização em 14/11/2026

Catálogo em dificuldade 5/5; mapa revisado a pedido do usuário para somente
teclado: 535 notas, 105 sustentações e 189 ataques de duas teclas, do instante
0,120 até 227,064 s. O áudio original
permanece intacto e a vitória aguarda seu fim natural em aproximadamente
244,227 s. Lista encadeada, Merge Sort e regras da partida não foram alterados
nesta etapa. O trabalho de análise ocorre offline, sem custo adicional no jogo.

A adaptação agora utiliza exclusivamente os canais de piano 3 e 11 do MIDI
3381, com ajuste por canal e verificação dos ataques esperados no áudio.
Eventos vocais e ataques genéricos do fechamento foram removidos. Depois
do último evento, não são inventadas notas para preencher o final sem uma
referência de teclado suficientemente confirmada. A conferência auditiva
humana continua pendente: o teste de execução não prova sincronização musical
perfeita. Os arquivos intermediarios de analise foram removidos; o mapa publicado permanece em `assets/musicas/dancing_queen/notas.txt`.

  -----------------------------------------------------------------------
  Parte                               Decisão
  ----------------------------------- -----------------------------------
  Gênero/tema                         Jogo de ritmo inspirado em Guitar
                                      Hero, com músicas de filmes
                                      musicais

  Linguagem                           C

  Biblioteca gráfica planejada        raylib

  Estrutura central                   Lista simplesmente encadeada de
                                      notas

  Estrutura do catálogo               Vetor de structs `Musica`

  Algoritmo                           Merge Sort

  Ordenação do catálogo               Título A--Z ou dificuldade
                                      crescente/decrescente

  Ordenação da partida                Notas por tempo de execução

  Dificuldade inicial                 Classificação manual de 1 a 5

  Entrada do jogador                  Teclas associadas às pistas
                                      (proposta inicial: D, F, J, K)

  IA na dinâmica do jogo              Não prevista inicialmente
  -----------------------------------------------------------------------

**Diretriz central:** construir primeiro uma versão funcional e pequena
do jogo, garantindo que a lista encadeada controle as notas, que o Merge
Sort tenha uma aplicação demonstrável e que áudio, tempo e entrada do
jogador estejam sincronizados. Só depois ampliar o catálogo e adicionar
recursos extras.

## Gerador offline de mapas (17/11/2026)

Foi implementada a ferramenta solicitada em `ferramentas/gerador_mapas/`,
em C99, usando a raylib existente para decodificar MP3/WAV e converter
para mono a 11025 Hz. FFT radix-2 propria, STFT/Hann, HPSS, fluxo espectral,
BPM, estimativa de altura e sustentacoes geram cinco arquivos alternativos.
Os ajustes ficam em `configuracao.h`. Executar `make gerador` compila um
executavel separado; `make test-gerador` roda sinais sinteticos conhecidos.
Os mapas sao gravados somente em uma pasta nova indicada pelo usuario.

Nao houve alteracao no relogio, lista encadeada, Merge Sort do jogo,
catalogo, dificuldade publicada, audio ou mapa atual de Dancing Queen.
A selecao de um dos cinco arquivos para uso no jogo continua manual.
HPSS nao isola teclado de voz; qualidade musical em gravacoes reais requer
conferencia auditiva. Densidades por nivel sao presets iniciais ajustaveis,
nao uma classificacao definitiva. Consulte o README da ferramenta para
limites, resultados dos testes e orientacoes de ajuste.

## Integracao de Twist and Shout

O catalogo e sua capacidade passaram de cinco para seis musicas. A nova
musica usa os recursos em `assets/musicas/twist_and_shout/`, mapa do gerador
offline no nivel inicial 3/5 (329 notas, oito sustentacoes) e ranking proprio.
O filme/origem esta provisoriamente identificado como "Twist and Shout".
O MP3 e a imagem fornecidos foram preservados. Como `fundo.png` era JPEG,
foi criada a copia `fundo_jogo.png` em PNG valido para a raylib.
Lista encadeada, Merge Sort, regras e relogio da partida permanecem iguais.
Os cinco candidatos foram guardados em `mapas_gerados/`; a conferencia
auditiva do mapa publicado ainda esta pendente.

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
