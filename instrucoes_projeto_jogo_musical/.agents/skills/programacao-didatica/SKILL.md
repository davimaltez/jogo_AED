---
description: Desenvolve e revisa codigo em C de forma simples e didatica
  para este projeto academico. Use ao criar, modificar, explicar, testar
  ou revisar codigo, estruturas de dados e algoritmos.
name: programacao-didatica
---

# Skill de programacao didatica

## 1. Quando usar

Use estas orientacoes sempre que trabalhar com codigo deste jogo:
planejamento tecnico, criacao de arquivos, implementacao de funcoes,
depuracao, testes, revisao ou explicacao.

Antes de comecar, leia: - `AGENTS.md`, para as regras gerais do
projeto; - `contexto_projeto_jogo_musicais.md`, para entender o jogo e
as decisoes tomadas.

## 2. Nivel e objetivo de aprendizagem

Considere que a estudante esta no terceiro periodo de Ciencia da
Computacao e esta aprendendo a desenvolver este projeto.

O objetivo nao e produzir o codigo mais sofisticado. O objetivo e
construir um jogo funcional cujo codigo a estudante consiga compreender,
explicar e defender academicamente.

Ensine de forma gradual. Nao presuma conhecimento de conceitos que ainda
nao foram explicados.

## 3. Linguagem, bibliotecas e compatibilidade

-   Escreva em C, nao em C++.
-   Utilize raylib para recursos graficos, conforme o contexto do
    projeto.
-   Prefira recursos comuns da linguagem C.
-   Nao acrescente dependencias sem necessidade.
-   Antes de usar uma funcao ou recurso de biblioteca, explique sua
    finalidade quando ela for relevante para a compreensao do projeto.

## 4. Padrao de nomenclatura

Use nomes em portugues, sem acentos, sempre que possivel.

### Variaveis e campos de estruturas

Use `snake_case` e nomes que indiquem o significado: - `tempo_musica` -
`pontuacao` - `quantidade_notas` - `nota_atual` - `proxima_nota`

Evite nomes vagos como `x`, `aux`, `tmp`, `n1` ou `valor`, exceto em
casos simples em que o significado esteja evidente pelo contexto. Em
lacos curtos, `i` e aceitavel como contador.

### Funcoes

Use `snake_case` com verbo e complemento: - `criar_nota` -
`inserir_nota` - `remover_nota` - `carregar_notas` - `ordenar_musicas` -
`calcular_pontuacao` - `desenhar_pistas`

O nome deve indicar o que a funcao faz. Cada funcao deve ter uma
responsabilidade principal.

### Estruturas e tipos

Use nomes claros em portugues, com inicial maiuscula, como `Musica`,
`Nota` e `Jogador`. Os campos seguem `snake_case`.

### Arquivos

Use nomes curtos e descritivos, em minusculas e com palavras separadas
por underline quando necessario. Exemplos: `lista_notas.c`,
`lista_notas.h`, `catalogo_musicas.c`.

## 5. Estilo e simplicidade

-   Prefira `if`, `else`, `switch`, `for` e `while` quando forem
    suficientes.
-   Evite macros, ponteiros de funcao, genericos, abstracoes e padroes
    avancados sem necessidade.
-   Nao crie uma funcao para uma unica linha trivial se isso tornar o
    fluxo mais dificil de acompanhar.
-   Tambem nao concentre muitas responsabilidades em uma funcao grande.
-   Evite variaveis globais. Se forem realmente necessarias, explique a
    razao.
-   Use ponteiros quando forem necessarios, explicando o que apontam e
    como sao usados.
-   Verifique retornos de funcoes que podem falhar, como alocacao de
    memoria e abertura de arquivos.
-   Libere corretamente a memoria alocada.
-   Nao use recursos de C++.

## 6. Comentarios e documentacao

-   Escreva comentarios em portugues, preferencialmente sem acentos para
    manter consistencia com os identificadores.
-   Comente a finalidade de funcoes importantes e decisoes que nao sejam
    obvias.
-   Nao comente cada linha.
-   Evite comentarios que apenas repetem literalmente o comando.
-   Explique invariantes e cuidados importantes em operacoes com
    ponteiros e listas.

## 7. Estruturas de dados e AED

Respeite o contexto do projeto: - A lista simplesmente encadeada de
notas e a estrutura central da logica da partida. - O catalogo de
musicas fica em um vetor de estruturas `Musica`. - O Merge Sort deve ter
aplicacao real na ordenacao do catalogo e, se os dados de entrada
justificarem, na ordenacao cronologica das notas. - Nao substitua essas
escolhas por outras estruturas sem apresentar justificativa e pedir
aprovacao.

Ao implementar listas, explique: - o papel do primeiro ponteiro da
lista; - o que cada no guarda; - como o campo `proximo` liga os nos; -
como inserir, percorrer, buscar, atualizar, remover e liberar os nos; -
como evitar perder o restante da lista ao alterar ponteiros.

Ao implementar Merge Sort, explique: - a ideia de dividir o conjunto; -
como as partes menores sao ordenadas; - como a intercalação funciona; -
por que o algoritmo tem complexidade `O(n log n)`; - a diferenca entre
ordenar vetor e lista encadeada.

Nao afirme que um requisito academico foi atendido sem conferir o
contexto e, quando necessario, os materiais da disciplina.

## 8. Como apresentar uma implementacao

Ao criar ou alterar codigo, organize a explicacao nesta ordem:

1.  **Objetivo:** o que esta etapa vai resolver.
2.  **Arquivos:** quais arquivos serao criados ou modificados.
3.  **Conceitos:** quais conceitos novos aparecem.
4.  **Implementacao:** explique as partes importantes do codigo em
    linguagem simples.
5.  **Decisoes:** explique por que essa abordagem foi escolhida.
6.  **Teste:** mostre como compilar e executar, e quais resultados devem
    ser observados.
7.  **Proximo passo:** sugira apenas a proxima etapa necessaria, sem
    avancar automaticamente.

Nao despeje explicacoes enormes de uma vez. Divida conceitos dificeis em
partes pequenas e pergunte se a estudante quer esclarecer algo quando
houver risco de confusao.

## 9. Processo de trabalho

-   Comece cada tarefa entendendo o pedido e consultando os arquivos de
    referencia.
-   Para mudancas relevantes, apresente um plano curto antes de editar.
-   Aguarde aprovacao antes de mudancas grandes ou que alterem a
    arquitetura.
-   Trabalhe em etapas pequenas e testaveis.
-   Ao terminar, liste os arquivos alterados e os testes realizados.
-   Se nao conseguir executar um teste, diga isso claramente e forneca o
    comando para a estudante executar.
-   Ao encontrar um erro, explique primeiro a causa provavel e depois
    apresente uma correcao incremental.
-   Nao invente saidas de terminal nem declare sucesso sem verificacao.

## 10. Regra de qualidade

Antes de considerar uma etapa concluida, verifique: - O codigo esta em
C? - Os nomes sao claros e consistentes? - A solucao e simples o
suficiente para a estudante explicar? - As funcoes tem responsabilidades
compreensiveis? - Os erros e recursos de memoria foram tratados? - A
etapa foi compilada ou testada, ou a limitacao foi informada? - A
implementacao respeita o contexto do projeto?

Se uma resposta for "nao", explique o que falta antes de avancar.
