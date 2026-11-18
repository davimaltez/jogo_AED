Preciso de um GERADOR AUTOMÁTICO DE MAPAS DE NOTAS para o meu jogo de ritmo.
O gerador recebe um arquivo de música e produz o mapa de notas que o jogo lê.

- Linguagem/biblioteca: C99 + raylib.
- Como o áudio é lido: LoadMusicStream e UpdateMusicStream; tempo obtido por GetMusicTimePlayed. Formatos usados no projeto: MP3 e WAV.
- Número de pistas/teclas: 5 — D, F, G, J e K.
- Formato do arquivo de mapa: notas.txt, uma nota por linha: tempo_em_segundos pista [duracao]; pistas de 0 a 4.
- Tem notas longas? Sim. O terceiro campo opcional representa a duração em segundos; omitido ou zero indica nota comum.
- Dificuldades: Escala manual de 1 a 5 por música. Densidades por nível ainda não definidas.
- Quando o gerador roda: Ainda não definido. A preparação de mapas existente ocorre offline; o jogo carrega o arquivo pronto.
- Restrições do projeto: C, nomes em português sem acentos e em snake_case, funções com responsabilidades claras, código didático e tratamento de erros/memória. Preservar lista simplesmente encadeada e Merge Sort; novas dependências exigem justificativa e aprovação.

## Método que quero (mesma ideia de um projeto que já funcionou)

1. Preparar o áudio: juntar em mono, normalizar e reamostrar para ~11025 Hz. Isso basta para ritmo e melodia e deixa tudo mais rápido.
2. Espectrograma (STFT): FFT própria (radix-2, sem biblioteca externa), janela de Hann, N = 512, hop = 128 (~11,6 ms por quadro). Guardar a magnitude de cada quadro.
3. Separar harmônico de percussivo (HPSS): filtro de mediana no tempo (realça o harmônico: melodia, voz) e na frequência (realça o percussivo: bumbo, caixa). Gerar uma máscara para cada parte. Isso evita que o bumbo "vaze" para a detecção de melodia.
4. Fluxo espectral (onsets): para cada quadro, somar só os aumentos positivos de magnitude em relação ao quadro anterior (de preferência em escala log). O resultado é uma curva de "novidade".
   - Escolher picos com limiar adaptativo: média/mediana local × fator (~1,15) + um mínimo (~0,2 da curva normalizada), mais uma distância mínima entre picos.
   - Calcular separadamente para a parte percussiva e para a harmônica.
5. Detector de batidas:
   - Autocorrelação da curva de fluxo para estimar o BPM (procurar entre 60 e 200, preferindo 90–160).
   - Achar a fase da grade de batidas que mais coincide com os picos.
   - Gerar a lista de tempos das batidas (e meias batidas, se quiser).
   - Alinhar levemente as notas detectadas à grade quando estiverem perto (tolerância de ~1 quadro).
6. Melodia (altura das notas): na parte harmônica, estimar a frequência fundamental de cada quadro por SOMA HARMÔNICA (somar a magnitude em f, 2f, 3f, ... e escolher o f com a maior soma). Isso corrige erros de oitava.
   - Uma nota de melodia começa num pico de fluxo harmônico.
   - Ela dura enquanto a energia e a altura se mantêm estáveis (sustentação).
   - Se durar mais que um mínimo (ex.: 0,4 s), vira nota longa com essa duração.
   - Notas fracas que caem dentro de uma nota longa são descartadas ("engolidas").
7. Força e seleção por dificuldade:
   - Cada nota candidata recebe uma força (altura do pico normalizada pelo percentil 90 da música).
   - Cada dificuldade aceita notas acima de um corte (ex.: fácil 0,35, médio 0,15, difícil 0,08), respeitando um intervalo mínimo entre notas.
   - Se uma dificuldade ficar esparsa demais, completar com notas nas batidas.
   - No fácil, não usar notas longas.
8. Escolher a pista de cada nota:
   - Mapear a altura da melodia para as pistas (notas mais agudas → pistas da direita), sem colocar a mesma pista muitas vezes seguidas.
   - Notas só de percussão podem alternar pistas ou seguir um padrão.
   - Nunca duas notas na mesma pista com a duração se sobrepondo.
9. Saída:
   - Ordenar por tempo e salvar no formato do meu jogo, um arquivo por dificuldade.
   - Ignorar os primeiros ~1–2 s, para o jogador se preparar.

## Como quero receber

- Código dividido em módulos (ex.: fft, espectrograma, onsets, batidas, melodia, gerador), com comentários explicando cada etapa em linguagem simples.
- Todas as constantes de ajuste (limiares, tamanhos, cortes por dificuldade) num lugar só.
- Um teste simples com áudio sintético (batidas + uma escala de notas geradas no próprio código), mostrando quantas notas foram detectadas e se há falsos positivos.
- Explique também como ajustar os parâmetros se o mapa ficar cheio ou vazio demais.