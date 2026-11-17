# Gerador offline de mapas

Ferramenta C99 que recebe MP3/WAV e gera cinco mapas `tempo pista duracao`.
Usa somente a raylib ja instalada e a biblioteca matematica do C. Nao precisa
abrir janela, tocar a musica, usar MIDI, Python ou FFmpeg.

Na raiz do projeto, em um terminal WSL:

```sh
make gerador
/tmp/final_act_gerador_mapas assets/musicas/dancing_queen/audio.mp3 /tmp/mapas_dancing_queen
```

O destino deve ser uma **pasta nova**, cujo diretorio pai ja exista. Serao
criados `notas_1.txt` ate `notas_5.txt`. A ferramenta recusa destinos existentes,
audio invalido e mapas vazios. Em erro de escrita, remove os arquivos da pasta
que acabou de criar. O executavel `final_act`, os mapas publicados e o catalogo
nao sao modificados. `/tmp` e temporario: recompile se o executavel desaparecer.
Para outro destino de compilacao: `make gerador GERADOR_TARGET=./gerador_mapas`.

Os cinco arquivos sao alternativas para revisao. O jogo ainda seleciona um
unico `notas.txt` por musica, com dificuldade manual no catalogo. Depois de
ouvir/testar o candidato escolhido, ele pode substituir esse arquivo em uma
etapa separada, preservando uma copia do anterior. Nao ha selecao automatica
de dificuldade no jogo nem alteracao de pontuacao, sincronizacao ou ranking.

## Modulos e percurso dos dados

| Arquivo | Responsabilidade |
| --- | --- |
| `configuracao.h` | Taxa, FFT, filtros, limiares e cinco niveis |
| `gerador.h` | Estruturas e contratos entre os modulos |
| `audio.c` | Decodificacao, conversao mono/11025 Hz e normalizacao |
| `fft.c` | FFT radix-2 iterativa, escrita em C |
| `espectrograma.c` | Hann, STFT e HPSS por medianas |
| `onsets.c` | Fluxo logaritmico, picos e rejeicao de solturas/vazamentos |
| `batidas.c` | Autocorrelacao, BPM, fase e alinhamento pequeno |
| `melodia.c` | Soma harmonica e estabilidade da sustentacao |
| `gerador.c` | Candidatas, Merge Sort, dificuldades, pistas e escrita |
| `principal.c` | Argumentos, pasta nova, mensagens e liberacao dos recursos |

1. **Audio:** `LoadWave` decodifica o arquivo inteiro; `WaveFormat` converte
   para float mono a 11025 Hz. A normalizacao divide pelo maior modulo; silencio
   continua zero. O limite inicial e 900 s. Esse limite e verificado depois da
   decodificacao: um arquivo enorme ainda pode consumir memoria dentro da raylib.
2. **STFT:** janela Hann de 512 amostras, passo de 128. Cada quadro esta
   centrado em `quadro * 128 / 11025` segundos; zeros completam as bordas.
   Guarda apenas os 257 bins de frequencia nao negativa. A FFT combina grupos
   de tamanhos 2, 4, 8...512, sem biblioteca externa de FFT.
3. **HPSS:** a mediana de 17 quadros no tempo realca componentes persistentes;
   a mediana de 17 bins realca ataques largos em frequencia. Para medianas H/P,
   a mascara harmonica e `H*H / (H*H + P*P)`; a percussiva e seu complemento.
   Sao magnitudes separadas para analise, sem reconstrucao de arquivos de audio.
4. **Ataques:** soma aumentos de `log(1 + ganho * magnitude)` entre quadros.
   O limiar e media local vezes 1,15 mais 0,20, na curva normalizada. Existe
   um piso de referencia para nao amplificar pequenas oscilacoes de sons
   sustentados. Quedas de energia sao rejeitadas para evitar ataques falsos
   na soltura. A forca da candidata e dividida pelo percentil 90 dos picos
   da respectiva componente. Uma forca pode superar 1.
5. **Batidas:** compara o fluxo com copias atrasadas entre 60 e 200 BPM,
   com preferencia leve por 90--160. A fase maximiza a soma sobre a grade.
   Sem ataques suficientes ou confianca, nao ha grade nem preenchimento.
   So move uma nota se a batida estiver a ate um quadro (~11,6 ms).
6. **Altura:** testa notas MIDI 48--88, soma magnitudes em f, 2f...5f com
   pesos 1/h e exige energia na fundamental. A altura e estimada logo apos
   o ataque. A sustentacao termina quando energia/altura deixam de ser
   estaveis por tres quadros; aceita duracoes de 0,4 a 4 s. Nao e simplesmente
   o intervalo ate a proxima batida.
7. **Selecao:** une ataques harmonicos/percussivos proximos para preservar a
   cauda de uma mesma nota, ordena candidatas por Merge Sort e aplica cortes
   e intervalos. Notas fracas dentro de uma sustentacao sao descartadas.
   Preenche lacunas de duas batidas somente onde existe energia e grade
   confiavel. O nivel 1 sempre remove duracoes longas.
8. **Pistas:** mapeia a extensao de alturas da musica para 0--4 (D/F/G/J/K).
   Percussao alterna pistas. Uma pista ocupada ate o fim de uma sustentacao
   fica bloqueada, com folga de 80 ms; busca-se a pista livre mais proxima.
   Evita mais de duas notas consecutivas na mesma pista. Esta versao gera
   uma cabeca por ataque selecionado, sem transcrever acordes polifonicos;
   sustentações podem coexistir com notas em outras pistas.

Os primeiros 1,5 s sao ignorados **sem subtrair esse tempo dos timestamps**.
O instante salvo e o de acerto relativo ao inicio do audio. Nao se subtrai o
tempo de viagem pela tela: o jogo ja posiciona as notas usando esse instante
e seu relogio de reproducao. Ao usar outro corte/edicao do audio, gere novo mapa.

Vetores contiguos sao usados apenas na analise offline. Cada modulo libera
suas alocacoes em caso de erro, e o chamador libera resultados apos usa-los.
O jogo continua carregando sua lista simplesmente encadeada e ordenando-a
com o Merge Sort existente. Nenhum dos algoritmos de AED do jogo foi removido.

## Ajustes

Altere `configuracao.h` e rode `make gerador` novamente. Valores iniciais:

| Nivel | Corte de forca | Intervalo minimo | Longas |
| --- | ---: | ---: | --- |
| 1 | 0,35 | 0,48 s | Nao |
| 2 | 0,25 | 0,34 s | Sim |
| 3 | 0,15 | 0,25 s | Sim |
| 4 | 0,10 | 0,18 s | Sim |
| 5 | 0,08 | 0,12 s | Sim |

- **Cheio demais:** aumente `corte` ou `intervalo` em `AJUSTES_DIFICULDADE`.
  Aumentar `FATOR_LIMIAR`, `MINIMO_LIMIAR` ou `INTERVALO_PICOS` reduz os
  ataques detectados para todos os niveis. Aumente `LACUNA_BATIDAS` para
  reduzir preenchimentos.
- **Vazio demais:** reduza primeiro o corte do nivel. Se faltam candidatas,
  reduza o limiar de ataques com cuidado e confira novamente os falsos
  positivos. Baixar so o corte nao recupera ataques rejeitados na analise.
- **Longas demais:** reduza `DURACAO_LONGA_MAXIMA` ou aumente
  `ENERGIA_RELATIVA`. Para exigir caudas mais estaveis, reduza
  `TOLERANCIA_ALTURA`. Aumente `DURACAO_LONGA_MINIMA` para menos longas.
- **Alturas graves imprecisas:** uma FFT maior melhora resolucao de frequencia,
  mas espalha mais o ataque no tempo. Reavalie janelas de mediana e testes
  ao mudar a FFT. N deve continuar sendo potencia de dois.

Esses niveis sao presets iniciais, nao uma classificacao musical calibrada.
Os mapas nao sao necessariamente subconjuntos uns dos outros: mudar o
intervalo pode mudar quais ataques competem e quais pistas estao livres.

## Testes e limites

```sh
make test-gerador
make test
```

`tests/teste_gerador_mapas.c` gera no proprio C: silencio, audio curtissimo,
20 pulsos ruidosos a 120 BPM, uma escala de dez notas com harmonicos e
duracao de 0,7 s, e a mistura dos dois. Compara os ataques com os instantes
conhecidos (tolerancia de 60 ms), mede falsos positivos e erro maximo, verifica
alturas, duracoes, FFT, conservacao das magnitudes HPSS, intervalos, limites
de pistas e conversao de WAV stereo 48000 Hz. Todos os mapas sinteticos sao
salvos e carregados pela lista real do jogo. Arquivos temporarios sao removidos.

Resultado de 17/11/2026, configuracao padrao:

- Batidas isoladas: 20/20, zero extras, erro maximo 9,3 ms; BPM 120,19.
- Escala isolada: 10/10, zero extras, erro maximo 5,4 ms.
- Mistura: 20/20 batidas e 10/10 notas, zero extras; erros maximos 8,5/6,2 ms.
- Mapas da mistura nos niveis 1--5: 15, 15, 20, 30, 30 notas;
  respectivamente 0, 5, 7, 10, 10 longas; zero falsos positivos nesses mapas.
- Sanitizadores AddressSanitizer e UndefinedBehaviorSanitizer passaram
  no teste sintetico, sem erros reportados.
- MP3 real de Dancing Queen (244,227 s): geracao executada em 3,71 s,
  pico de memoria residente de 105432 KiB nesta maquina. Produziu
  314/391/572/694/1184 notas e 0/11/16/29/38 longas nos niveis 1--5.
  BPM estimado: 99,38. Esses mapas nao foram publicados nem avaliados
  auditivamente; o numero estimado nao foi tratado como BPM de referencia.
  Os cinco arquivos passaram pelo carregador real e pela validacao de
  duracao da lista de notas do jogo.
- Destino existente foi recusado sem alterar seu conteudo; arquivo ausente
  falhou sem publicar mapas. Hashes do MP3, mapa publicado e executavel
  preexistente permaneceram iguais antes/depois da geracao.

Esses numeros valem para os sinais sinteticos descritos, nao para qualquer
musica. HPSS nao isola teclado: voz, cordas e outros sons tonais ficam na
parte harmonica. A soma harmonica reduz algumas ambiguidades, mas nao
garante fundamental correta em acordes, vibrato ou fundamental ausente.
A FFT de 512 pontos tem bins separados por ~21,5 Hz, insuficientes para
distinguir alguns semitons graves. Um unico BPM nao acompanha rubato ou
mudancas de andamento; o alinhamento pequeno evita arrastar ataques para
uma grade distante. A filtragem conservadora de energia tambem pode perder
ataques suaves sobre sons que estejam decaindo.

O algoritmo normaliza a faixa inteira: ruido de uma gravacao muito baixa
pode ser amplificado. Precisa de revisao auditiva e ajuste de parametros
por repertorio; nao substitui uma transcricao de instrumento isolado.
Latencia de hardware/reproducao e uma questao separada, nao medida pelo teste.

Referencias do metodo e APIs:
- [raylib 5.5: LoadWave e WaveFormat](https://www.raylib.com/cheatsheet/raylib_cheatsheet_v5.5.pdf).
- [AudioLabs: separacao harmonica/percussiva por medianas](https://www.audiolabs-erlangen.de/resources/MIR/FMP/C8/C8S1_HPS.html).
