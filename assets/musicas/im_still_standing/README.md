# Mapa automatico inicial - I'm Still Standing

Gerado em 17/11/2026 com ferramentas/gerador_mapas, sem alterar seu codigo
ou parametros. Analise offline: mono 11025 Hz, FFT/Hann, HPSS, ataques
espectrais, pulso global estimado e sustentacoes por altura/energia.
Publicado: copia identica de mapas_gerados/notas_3.txt em notas.txt.
Os cinco candidatos foram preservados. Dificuldade do catalogo continua
0 / A definir: o preset nao equivale a uma classificacao musical revisada.

## Comparacao dos cinco presets

Densidade = ataques / duracao total do WAV. Distribuicao: D/F/G/J/K.
Lacuna = maior intervalo entre ataques; nao inclui introducao/cauda.
Conflitos = duplicatas ou sobreposicoes na mesma pista.

| Preset | Notas | Notas/s | Longas | D/F/G/J/K | Min. ataques (s) | Lacuna max. (s) | Max. teclas | Conflitos |
| --- | ---: | ---: | ---: | --- | ---: | ---: | ---: | ---: |
| 1 | 223 | 1.186 | 0 | 31/45/60/54/33 | 0.487610 | 1.753 | 1 | 0 |
| 2 | 288 | 1.532 | 1 | 42/56/72/74/44 | 0.348290 | 1.370 | 1 | 0 |
| 3 | 414 | 2.203 | 1 | 62/77/102/113/60 | 0.255409 | 1.451 | 1 | 0 |
| 4 | 565 | 3.006 | 1 | 79/114/140/154/78 | 0.185753 | 1.370 | 1 | 0 |
| 5 | 732 | 3.895 | 1 | 104/148/183/196/101 | 0.127700 | 1.370 | 1 | 0 |

## Escolha e viabilidade humana

Preset 3: preserva mais ataques detectados que o 2, com densidade
local variavel e limite observado de quatro ataques numa janela de 1 s.
Os presets 4/5 exigem intervalos de 186/128 ms e picos de 5/7 ataques
por segundo, sem acrescentar sustentacoes; nao sao a primeira escolha.
A distribuicao usa todas as pistas (14,5% a 27,3% dos ataques por pista).
Maximo de repeticoes consecutivas na mesma pista: 2.
Menor folga entre o fim de uma nota e novo ataque na mesma pista: 0.255421 s.
Essas medidas indicam viabilidade estrutural com cinco teclas, mas nao
substituem a avaliacao humana de conforto, leitura e dificuldade.

## Cobertura, ritmo e trechos para revisao auditiva

- Duracao do audio: 187.951020 s.
- Primeiro ataque: 1.927256 s.
- Ultimo fim de nota: 181.394287 s.
- Cauda sem notas: 6.557 s; o jogo deve aguardar o fim natural do audio.
- Sustentacoes: 1; duracoes em segundos: 0.417953.
- Ataques a ate 60 ms de picos detectados: 411/414.
  Isso e consistencia com o mesmo detector, nao uma medida independente de precisao musical.
- Eventos provenientes exclusivamente da grade estimada, sem candidata de ataque no mesmo instante:
  1.927256 s, 3.297234 s, 181.394287 s.
  Foram mantidos como saida original do gerador, identificados para revisao; nao foram acrescentados manualmente.
- Densidades em blocos consecutivos de 10 s (ultimo bloco parcial), em notas/s:
  1.80, 2.90, 2.70, 2.60, 2.20, 2.00, 2.20, 2.70, 2.30, 2.30, 2.30, 2.30, 2.20, 2.10, 2.10, 2.20, 2.30, 2.10, 0.13.

### Primeiros 1,5 segundos

RMS: -31.46 dBFS; pico: -15.10 dBFS.
Ha sinal sonoro; nao se trata de silencio digital.
O detector filtrado nao encontrou ataques neste intervalo. Isso nao
prova ausencia de eventos musicalmente relevantes: pode haver som
sustentado ou ataques suaves. A relevancia perceptiva requer escuta.

### Encerramento

RMS dos ultimos 5 s: -72.58 dBFS.
Nao houve picos filtrados apos a ultima nota; o encerramento tem
energia baixa. Nao foram inventadas notas para ocupar a cauda.

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
| audio.wav | 7aaaac5b2f045a551098d09d1bd04efdb858477d3c6a46c9daf4eb47c0cf99ab |
| notas.txt | 46967e6bad3f349254cd98da186d7aea146b0fded43d2a8467b1ef0b95a6c5b3 |
| mapas_gerados/notas_1.txt | 30cebe918339514fac42eb24245082be79dbbdb7b87517b8e8359a2409025c69 |
| mapas_gerados/notas_2.txt | c581a57d5949bd4ac63768b039b5c2656595196630e8988c2e0adea05df1cf4b |
| mapas_gerados/notas_3.txt | 46967e6bad3f349254cd98da186d7aea146b0fded43d2a8467b1ef0b95a6c5b3 |
| mapas_gerados/notas_4.txt | 9454d1150f0b71d0f5830b2a8ab816fa9d54c816cace97f2fa0066fdbf2c2de8 |
| mapas_gerados/notas_5.txt | d8aa5ee9eb579903840021f09f45f7a29b665d0c35c327f1860199e1a17febfe |

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
