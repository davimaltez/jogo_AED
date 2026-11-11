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
-   organização dos módulos e arquivos `.c`/`.h`;
-   formato de armazenamento dos melhores resultados;
-   funcionalidades extras após a primeira versão.

## Organização dos recursos por música (decisão confirmada)

Cada música possui uma pasta própria em `assets/musicas/<nome_da_musica>/`,
com os caminhos previstos para `audio.mp3`, `fundo.png` e `notas.txt`.
A struct `Musica` guarda esses caminhos nos campos `caminho_audio`,
`caminho_fundo` e `caminho_notas`. Recursos compartilhados ficam em
`assets/interface/fontes/` e `assets/interface/imagens/`.

Essa organização não altera a lista encadeada nem o Merge Sort.
As pastas contêm arquivos `notas.txt` de demonstração. O carregamento das
notas está implementado; áudio e imagens ainda não são carregados.

## Arquivos de notas e preparação da partida (decisão confirmada)

Cada linha de `notas.txt` contém `tempo pista`: tempo decimal em segundos,
finito, não negativo e representável em `float`; pista inteira de 0 a 4,
correspondente a D, F, G, J e K. Espaços e tabulações separam os campos.
Linhas vazias e comentários em linhas próprias iniciadas por `#` são aceitos.
O limite é 255 caracteres por linha, sem contar LF/CRLF. Arquivos sem notas,
campos extras, bytes nulos e conversões numéricas com erro são rejeitados.
O README documenta limites e exemplos completos.

`carregar_notas_arquivo` usa `Musica.caminho_notas` para montar uma lista
temporária, valida a leitura completa e aplica Merge Sort estável por tempo,
reorganizando os ponteiros dos nós. Só depois substitui a lista anterior.
Falhas liberam a lista temporária e preservam a lista e os indicadores da
partida. Início e reinício só avançam em caso de sucesso; erros aparecem na
tela e no terminal. Não existe fallback automático para notas geradas.

Os cinco arquivos demonstram eventos fora de ordem, reproduzindo os tempos
e pistas do gerador anterior. O gerador foi mantido para testes. A ordenação
ocorre uma vez por carregamento, antes da utilização da lista na partida.
O relógio continua baseado em `GetFrameTime`, sem reprodução ou sincronização
de áudio. Pontuação, combos, medidor e controles permanecem com suas regras.

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

A partida é concluída quando a lista não vazia tem todas as notas acertadas
ou perdidas e o medidor não acionou Game Over. A consulta
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
permanecem com suas implementações anteriores. Áudio e sincronização não
foram implementados.

## Navegação por teclado (implementação confirmada)

Os menus aceitam setas para mover o foco e Enter para confirmar, mantendo
o mouse e o ESC. O foco recebe uma borda dourada clara e é reiniciado na
troca de menu. No catálogo, cima/baixo percorrem os controles e músicas,
esquerda/direita percorrem os filtros e direita numa música acessa iniciar.
Enter numa música também inicia a partida usando o carregamento validado.
Pausa, Game Over e resultados reutilizam as ações existentes. A confirmação
de saída começa em Não e bloqueia as entradas do menu que fica atrás dela.

## 11. Resumo rápido da arquitetura

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
