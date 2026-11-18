# Mapa automatico inicial - Um Sonho Eu Tenho

Gerado em 17/11/2026 com ferramentas/gerador_mapas, sem alterar seu codigo
ou parametros. Analise offline: mono 11025 Hz, FFT/Hann, HPSS, ataques
espectrais, pulso global estimado e sustentacoes por altura/energia.
Publicado: copia identica de mapas_gerados/notas_2.txt em notas.txt.
Os cinco candidatos foram preservados. Dificuldade do catalogo definida pelo usuario em 4/5 em 17/11/2026.
O preset de geracao foi preservado e nao determina essa classificacao.

## Comparacao dos cinco presets

Densidade = ataques / duracao total do WAV. Distribuicao: D/F/G/J/K.
Lacuna = maior intervalo entre ataques; nao inclui introducao/cauda.
Conflitos = duplicatas ou sobreposicoes na mesma pista.

| Preset | Notas | Notas/s | Longas | D/F/G/J/K | Min. ataques (s) | Lacuna max. (s) | Max. teclas | Conflitos |
| --- | ---: | ---: | ---: | --- | ---: | ---: | ---: | ---: |
| 1 | 198 | 1.030 | 0 | 23/45/65/44/21 | 0.487610 | 2.392 | 1 | 0 |
| 2 | 310 | 1.613 | 7 | 34/74/111/61/30 | 0.348297 | 1.950 | 2 | 0 |
| 3 | 393 | 2.044 | 7 | 47/99/135/76/36 | 0.255417 | 1.950 | 2 | 0 |
| 4 | 637 | 3.314 | 9 | 87/172/211/112/55 | 0.185745 | 1.950 | 2 | 0 |
| 5 | 779 | 4.052 | 11 | 101/224/259/131/64 | 0.127701 | 1.950 | 2 | 0 |

## Escolha e viabilidade humana

Preset 2: conserva as sete sustentacoes presentes no preset 3, com
intervalo minimo de 348 ms em vez de 255 ms e pico de tres ataques
numa janela de 1 s. Ha no maximo duas teclas exigidas simultaneamente.
O preset 1 elimina as longas e tem tres lacunas internas acima de 2 s.
A pista G concentra 35,8% das notas; todas as pistas participam, mas
nao ha equilibrio perfeito. Nao foram redistribuidas notas artificialmente.
Maximo de repeticoes consecutivas na mesma pista: 2.
Menor folga entre o fim de uma nota e novo ataque na mesma pista: 0.348297 s.
Essas medidas indicam viabilidade estrutural com cinco teclas, mas nao
substituem a avaliacao humana de conforto, leitura e dificuldade.

## Cobertura, ritmo e trechos para revisao auditiva

- Duracao do audio: 192.235102 s.
- Primeiro ataque: 2.054966 s.
- Ultimo fim de nota: 188.499588 s.
- Cauda sem notas: 3.736 s; o jogo deve aguardar o fim natural do audio.
- Sustentacoes: 7; duracoes em segundos: 0.766258, 0.417957, 0.499229, 0.650162, 0.545670, 0.417961, 0.534058.
- Ataques a ate 60 ms de picos detectados: 307/310.
  Isso e consistencia com o mesmo detector, nao uma medida independente de precisao musical.
- Eventos provenientes exclusivamente da grade estimada, sem candidata de ataque no mesmo instante:
  4.469841 s, 5.352199 s, 122.705849 s.
  Foram mantidos como saida original do gerador, identificados para revisao; nao foram acrescentados manualmente.
- Densidades em blocos consecutivos de 10 s (ultimo bloco parcial), em notas/s:
  1.00, 1.50, 1.80, 1.80, 1.80, 2.10, 1.30, 1.80, 1.60, 1.70, 1.60, 1.80, 1.50, 1.50, 2.00, 1.40, 1.60, 1.70, 1.50, 0.00.

### Primeiros 1,5 segundos

RMS: -26.68 dBFS; pico: -6.37 dBFS.
Ha sinal sonoro; nao se trata de silencio digital.
Foram detectados seis picos harmonicos/percussivos (alguns podem representar
o mesmo evento) que o corte IGNORAR_INICIO descarta:
0.232200 s, 0.243810 s, 0.359909 s, 0.394739 s, 0.510839 s, 1.114558 s.
Ha evidencia de eventos candidatos relevantes omitidos. Nenhuma nota
foi acrescentada; esse trecho deve ser priorizado na revisao manual.

### Encerramento

RMS dos ultimos 5 s: -20.61 dBFS.
Ha um pico detectado apos a ultima nota: 188.638916 s.
A distancia para o ultimo ataque e inferior ao intervalo minimo do preset.
O fim ainda contem energia; nao foi classificado automaticamente como silencio.

## Validacoes

Os cinco candidatos passaram pelo carregador real e validar_duracao_notas,
usando a duracao decodificada do WAV. Verificadas pistas, tempos, ordem,
sobreposicoes, intervalos e sustentacoes. Simulacoes a 30/60/144/240 Hz
concluiram cada candidato sem perdas, sem longas interrompidas e sem
vitoria antes do fim simulado. O notas.txt instalado foi validado novamente.
Nenhuma regra de producao foi alterada. A verificacao auditiva esta pendente.

## Integridade

| Arquivo | SHA-256 |
| --- | --- |
| audio.wav | 40410260106f219035a5750aa15ad0d517f22447881cd03f6e7694c27a08b64d |
| notas.txt | c82f7e13f5f163f73a959f4654952a36bd9d2a17b70c63c2bb295dd78f77b78c |
| mapas_gerados/notas_1.txt | bf447fe91bb15c31afa17a7c82078e92f982ee1aa387f4bcb98d21fa37a3692b |
| mapas_gerados/notas_2.txt | c82f7e13f5f163f73a959f4654952a36bd9d2a17b70c63c2bb295dd78f77b78c |
| mapas_gerados/notas_3.txt | 99ecd0efd8ab5697907dd3366b449af08f0188ea46afc082e51e25dd54df8730 |
| mapas_gerados/notas_4.txt | 44336cff61a8b812e054c391ffe3a1620ed4f9b1cc744dec90ceea92ba718993 |
| mapas_gerados/notas_5.txt | 294f99ce775dc88235f5b3e37004a3af575dae3ab0dfb7f66ce61ee6eaeca833 |

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

## Fundo e dificuldade definidos pelo usuario - 17/11/2026

Dificuldade: 4/5. Fundo: fundo.png, copia identica de enrolados.png,
em C:/Users/gabri/OneDrive/Desktop/Nova pasta/. PNG valido, 1672x941,
sem conversao; original preservado. O caminho ja estava no catalogo.
Carregamento confirmado pelo modulo midia_partida e capturas da partida
inspecionadas em 1280x720 e 1920x1080, incluindo notas sustentadas.
Notas, teclas, pontuacao e medidor legiveis com o escurecimento existente.
Mapas, WAV, ranking e regras nao foram alterados.
