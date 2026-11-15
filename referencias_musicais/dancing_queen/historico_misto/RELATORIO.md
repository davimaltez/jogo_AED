# Dancing Queen — mapa difícil

## Entrega e limites

Mapa jogável de dificuldade **5/5**, cobrindo a gravação disponível: **750
notas, 147 sustentações e 136 ataques com duas teclas**. Primeiro evento em
0,285 s; último evento em 241,291 s. O áudio continua até seu término natural,
aproximadamente 244,227 s no decodificador usado pelo jogo. O contêiner MP3
reporta 244,272 s no ffprobe; essa diferença não deve ser usada para alongar
o mapa. A vitória também exige que todas as notas tenham sido resolvidas.

Este é um **alinhamento automático e uma adaptação musical**, não uma
transcrição certificada. A conferência auditiva humana ainda está pendente,
especialmente no fechamento. Os testes de execução não medem a latência
percebida nem demonstram que cada nota coincide exatamente com a voz/piano.

## Fontes e alinhamento

- Gravação: `assets/musicas/dancing_queen/audio.mp3`, preservada byte a byte.
- Referência principal: `abba_dancing_queen_3381.mid`; fontes do download no README desta pasta.
- Canal MIDI 4: candidato à linha vocal principal, identificado pela sequência
  melódica recorrente; 328 eventos aproveitados na adaptação.
- Canais MIDI 3 e 11: programas de piano, incluindo acordes e ornamentos;
  389 eventos aproveitados.
- Fechamento: 33 ataques tonais extraídos do registro agudo do próprio áudio.
  Essa análise não separa instrumentos e pode incluir outras fontes sonoras.

A análise compara energia das doze classes de altura (cromas) do MIDI e do
MP3. A busca inicial favoreceu a mesma tonalidade e aproximadamente
`tempo_audio = tempo_midi * 1,004 - 6,3`. O deslocamento remove a preparação
silenciosa do MIDI; não é uma calibração de hardware.

Foram refinadas âncoras locais em janelas distribuídas ao longo da faixa,
com interpolação contínua entre elas. Similaridade cosseno média dos trechos
principais: aproximadamente 0,744 (mínimo 0,626). Esses valores descrevem
semelhança harmônica; **não são probabilidades de acerto nem erro em ms**.

A gravação possui repetição final que o MIDI não representa da mesma forma.
A frase MIDI de aproximadamente 109,361–127,7 s foi reaproveitada na região
209–227 s da gravação. A partir de 227,6 s, os ataques agudos do áudio guiam
o fechamento, sem esticar arbitrariamente o restante do MIDI. O mapa não foi
obtido multiplicando os tempos pela razão entre as durações totais.

Referência metodológica: [sincronização por cromas e alinhamento temporal,
AudioLabs](https://www.audiolabs-erlangen.de/resources/MIR/FMP/C3/C3_MusicSynchronization.html).

## Adaptação às cinco teclas

- Melodia tem prioridade; ataques de piano complementam os espaços.
- Acordes grandes são reduzidos a até duas teclas. No máximo duas teclas
  são exigidas simultaneamente, incluindo notas sustentadas.
- Ornamentos muito rápidos são simplificados; piano sem voz tem pelo menos
  0,125 s entre grupos selecionados. O pico do mapa é 10 notas em uma janela
  de 1 s, contando as duas notas dos acordes.
- Sustentações menores que 0,45 s na voz ou 0,65 s no piano viram notas
  comuns. Caudas podem ser encurtadas para preservar espaço de soltura e
  rearticulação; duração máxima de 3 s. A nota musical continua no áudio.
- Há ao menos 0,12 s de espaço entre o fim de uma sustentação e a próxima
  nota na mesma pista. Não há sobreposição ou duplicatas na mesma pista.
- Distribuição D/F/G/J/K: 143 / 281 / 177 / 86 / 63 notas.

## Organização e desempenho

O jogo carrega somente o arquivo de texto final. Não lê MIDI, não executa
Python, não analisa o MP3 para gerar notas e não usa FFT/DTW durante a partida.
Os nós continuam na lista encadeada e passam pelo Merge Sort existente.

Em uma simulação a 240 atualizações por segundo, os 58.705 quadros consumiram
aproximadamente 199 ms de CPU no WSL desta máquina (0,0034 ms por atualização,
incluindo o jogador automático). Isso mede a lógica, **não a renderização**.
O volume de 750 nós não justificou modificar a arquitetura ou adicionar
estruturas auxiliares de execução.

## Reprodução e testes

Verificações concluídas em 14/11/2026:

- Compilação, testes de lógica e testes gráficos aprovados no WSL.
- Execução automática com o **MP3 e mapa reais inteiros**: 750 acertos,
  147 sustentações completas, nenhuma nota perdida/interrupção, vitória
  somente após 244,227 s de áudio. Nenhum ranking foi escrito pelo teste.
- Simulações completas a 30, 60, 144 e 240 atualizações por segundo aprovadas.
- Mesmo teste de mapa aprovado com AddressSanitizer e UndefinedBehaviorSanitizer.
- Nova geração offline produziu o mesmo SHA-256 do mapa publicado:
  `481e2b4f12b41cbb85d6ede3fdc2630366edcd1babc017c5c246ecb550fc72be`.
- MP3, ranking e executável preexistente mantiveram os hashes anteriores.
- A suíte geral avisou que o mapa de Breaking Free não existe e pulou essa
  verificação; isso não impediu os testes específicos de Dancing Queen.

A escuta/jogabilidade humana e o desempenho em tela cheia continuam pendentes.

Na raiz do projeto, no WSL:

```sh
make TARGET=/tmp/final_act_dancing_queen
GALLIUM_DRIVER=d3d12 /tmp/final_act_dancing_queen
make test
GALLIUM_DRIVER=d3d12 make test-grafico
# Percorre o MP3 e o mapa real inteiros, sem escrever no ranking:
GALLIUM_DRIVER=d3d12 /tmp/final_act_teste_partida mapa-completo
```

O driver acima é uma opção de execução local, não uma configuração gravada
no código compartilhado. O executável `final_act` preexistente foi preservado.

`teste_dancing_queen.c` usa o carregador e a lógica reais, valida cobertura,
cinco pistas, limite de teclas simultâneas, duração e uma execução perfeita
com todas as sustentações. A preparação offline usa somente NumPy e FFmpeg
já disponíveis no ambiente. Para reproduzir a geração:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 referencias_musicais/dancing_queen/preparar_mapa.py
```

Esse comando gera o candidato e o JSON de auditoria nesta pasta; não publica
automaticamente no catálogo nem altera o MP3. Os hashes dos arquivos de
entrada são verificados antes da geração. `mapa_dificil_auditoria.json`
identifica origem, canal e altura de cada nota, além das âncoras utilizadas.

O mapa anterior foi guardado em `notas_demonstracao_original.txt`. Rankings
anteriores foram preservados, mas seus pontos não são comparáveis aos deste
mapa. Ainda é necessário jogar/ouvir para avaliar dificuldade, alinhamento
perceptivo, trecho final e comportamento em tela cheia.
