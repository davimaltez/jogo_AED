# Referencias MIDI de Dancing Queen

Baixadas em 13/11/2026 para estudar o futuro mapa dificil de teclado e
melodia vocal. A revisao atual usa somente os canais de piano 3 e 11 do
arquivo 3381 na adaptacao dificil publicada no
`notas.txt`. O processo e suas limitacoes estao em [RELATORIO.md](RELATORIO.md).

| Arquivo | Pagina de origem | Download original | Duracao calculada |
| --- | --- | --- | --- |
| `abba_dancing_queen_3381.mid` | https://bitmidi.com/abba-dancing_queen-mid | https://bitmidi.com/uploads/3381.mid | 239,040 s |
| `abba_dancing_queen_3370.mid` | https://bitmidi.com/abba_-_dancing_queen-mid | https://bitmidi.com/uploads/3370.mid | 236,647 s |

Os dois arquivos foram lidos como Standard MIDI File, formato 0, com uma
trilha contendo varios canais. Cada um possui 8311 eventos de inicio de nota;
portanto, podem ser variantes do mesmo arranjo, nao transcricoes independentes.
O formato 0 nao impede separar instrumentos por canal.

O andamento registrado e aproximadamente 100,4 BPM em ambos. A inspecao
encontrou programas General MIDI 2, 34, 49, 53, 91, 95 e 106 (numeracao
iniciando em 1). Os canais ainda precisam ser ouvidos e comparados ao MP3
para confirmar quais representam o teclado e a melodia vocal. Nao ha eventos
de letra identificados; isso nao prova ausencia de uma melodia instrumental
correspondente a voz.

O MP3 do jogo dura 244,227 s e nao apresentou tags de titulo, artista ou
album no ffprobe. A correspondencia da versao nao foi confirmada. Diferencas
de introducao, final, andamento e estrutura precisam ser verificadas; nao
aplicar simplesmente um fator de escala baseado na duracao total.

`inspecao_midi.json` registra tamanhos, hashes SHA-256, formato, canais,
programas, andamento e duracao calculada ate o ultimo evento de cada MIDI.
O MP3 e o ranking continuam preservados. O catalogo agora indica dificuldade
5 e o mapa anterior esta guardado em `notas_demonstracao_original.txt`.
O mapa misto de 750 notas, substituido a pedido do usuario, foi arquivado
com seu gerador e auditoria em `historico_misto/`. A versao atual tem 535 notas
somente de piano. A previa `previa_teclado_30s.ogg` acrescenta cliques aos
primeiros 30 segundos para facilitar a conferencia auditiva, sem modificar o MP3.
