# Dancing Queen ? revis?o somente teclado

## O que mudou em 14/11/2026

A pedido do usu?rio, o mapa misto de piano e voz foi substitu?do por um mapa
com **535 notas provenientes exclusivamente dos canais MIDI 3 e 11**, ambos
com programa de piano. H? 105 sustenta??es e 189 ataques com duas teclas.
O cat?logo continua em dificuldade 5/5; a prioridade desta revis?o ? a
correspond?ncia com o teclado, n?o preencher a m?sica com mais eventos.

Foram removidos todos os eventos da linha vocal e os 33 ataques gen?ricos
usados anteriormente no fechamento. O primeiro ataque passou de 0,285 para
0,120 s. O ?ltimo evento termina em 227,064 s. Depois disso n?o h? notas:
**n?o h? refer?ncia de teclado suficientemente confirmada para o restante**.
O ?udio continua at? seu fim natural, aproximadamente 244,227 s, e somente
ent?o a partida pode ser conclu?da. N?o foi cortado nem modificado o MP3.

O mapa anterior, sua auditoria, gerador e relat?rio foram guardados em
`historico_misto/`. O mapa de demonstra??o original continua preservado
em `notas_demonstracao_original.txt`.

## M?todo de alinhamento revisado

O m?todo anterior comparava a harmonia geral. Isso n?o comprova que as
cabe?as das notas coincidem com os ataques do teclado. Nesta revis?o:

1. Somente notas dos canais de piano entram na refer?ncia.
2. O ?udio ? analisado em passos de aproximadamente 10 ms, com janelas de
   aproximadamente 93 ms, procurando aumento de energia nas alturas esperadas.
3. Cada canal recebe seu pr?prio ajuste de andamento e deslocamento.
   Canal 3: `tempo_midi * 1,0039 - 6,480`;
   canal 11: `tempo_midi * 1,0041 - 6,350`.
4. Cada acorde candidato procura um ataque local em at? 45 ms dessa previs?o.
   Eventos sem evid?ncia suficiente de ataque s?o descartados.
5. A repeti??o final usa somente as partes de piano da refer?ncia e tamb?m
   passa pela verifica??o local. N?o h? preenchimento por voz, bateria ou
   ataques escolhidos sem uma nota de piano correspondente no MIDI.

Esses deslocamentos alinham o MIDI ao arquivo de ?udio; n?o s?o uma
calibra??o do equipamento do jogador. O detector trabalha com a mistura
completa, n?o com uma faixa de piano isolada. Outros instrumentos podem
compartilhar alturas e ataques. Portanto, **isso ? uma revis?o autom?tica
mais espec?fica, ainda sujeita ? confer?ncia auditiva**, n?o uma garantia
mensurada de sincroniza??o perfeita. A resolu??o da an?lise tamb?m n?o ?
uma medida do erro real de cada evento.

Acordes s?o adaptados a no m?ximo duas teclas, contando sustenta??es ativas.
H? espa?o para soltar e rearticular a mesma pista. O gerador pode simplificar
ornamentos muito r?pidos e encurtar caudas. Cada evento publicado tem seu
canal, altura, tempo de origem e medida de ataque no JSON de auditoria.

## Como conferir

Na raiz do projeto, dentro do WSL:

```sh
make TARGET=/tmp/final_act_dancing_queen_teclado
GALLIUM_DRIVER=d3d12 /tmp/final_act_dancing_queen_teclado
```

A vari?vel de GPU ? apenas uma op??o local de execu??o. Nenhuma configura??o
de GPU foi inclu?da no c?digo. O execut?vel `final_act` existente n?o foi
substitu?do.

Abra `previa_teclado_30s.ogg` para ouvir os primeiros 30 segundos da grava??o
com um clique em cada ataque do mapa (um clique por acorde). A pr?via ? uma
c?pia para revis?o, n?o o ?udio usado no jogo. Ela permite conferir os
instantes do mapa sem depender da entrada do teclado ou da renderiza??o.
N?o foi usada como prova de escuta humana por parte do assistente.

## Verifica??es

- O teste `mapa-completo` com reproducao real foi executado nesta revisao,
  mas falhou na assercao final de tela de ranking e audio finalizado
  (`tests/teste_partida.c:589`). Portanto, a execucao integral desta revisao
  **nao esta validada**. A saida nao identifica qual dos dois estados falhou;
  nao foi atribuida uma causa nem alterado o sistema de audio nesta etapa.
- O carregador real aceitou o arquivo, sem sobreposi??es inv?lidas.
- As 535 notas e 105 sustenta??es foram resolvidas nas simula??es completas
  de 30, 60, 144 e 240 atualiza??es por segundo, sem perdas ou interrup??es.
- Cada linha do mapa foi comparada ao JSON: todos os eventos t?m canal 3 ou 11.
- A su?te `make test` passou; o teste geral de cat?logo avisou que o arquivo
  de Breaking Free n?o est? dispon?vel e pulou apenas esse item.
- A l?gica usou aproximadamente 0,0024?0,0026 ms de CPU por atualiza??o,
  incluindo o jogador autom?tico e sem renderiza??o, nesta m?quina.

A an?lise continua sendo offline. O jogo l? apenas `notas.txt`; n?o houve
mudan?a na lista encadeada, nos Merge Sort, no rel?gio, nas regras ou no ?udio.
Os rankings existentes foram preservados; pontua??es de mapas diferentes
n?o s?o diretamente compar?veis.

Para reproduzir o candidato e a auditoria:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 referencias_musicais/dancing_queen/preparar_mapa.py
```

Esse comando verifica os hashes das entradas, gera os arquivos nas
refer?ncias e n?o substitui automaticamente o mapa publicado. A an?lise
usa NumPy e FFmpeg j? presentes no WSL. Para testar a reprodu??o completa:

```sh
make /tmp/final_act_teste_partida
GALLIUM_DRIVER=d3d12 /tmp/final_act_teste_partida mapa-completo
```
