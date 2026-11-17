# Twist and Shout

Integrada ao catalogo com dificuldade inicial 3/5. O campo filme/origem
usa provisoriamente "Twist and Shout", aguardando a identificacao desejada.

- `audio.mp3`: arquivo fornecido, preservado; 155,226669 s decodificados.
- `fundo.png`: original fornecido, preservado. Apesar da extensao, e JPEG.
- `fundo_jogo.png`: copia convertida para PNG valido, 2752x1536, usada pelo jogo.
- `notas.txt`: nivel 3 do gerador offline, 329 notas e oito sustentacoes.
- `mapas_gerados/notas_1.txt` ate `notas_5.txt`: candidatos preservados;
  respectivamente 168, 231, 329, 524 e 626 notas.
- `ranking.txt`: caminho reservado no catalogo; criado pelo fluxo normal
  de registro de resultados, sem inserir pontuacoes artificiais.

O ultimo evento termina em 151,580 s. A partida continua aguardando o fim
do audio e a resolucao das notas, pelas mesmas regras das outras musicas.
O mapa usa as cinco pistas D/F/G/J/K. Foi gerado a partir da mistura completa
do MP3, com os parametros padrao de `ferramentas/gerador_mapas/`; nao se trata
de transcricao manual nem de isolamento de um instrumento.

Para reproduzir os candidatos, na raiz do projeto dentro do WSL, escolha
uma pasta de saida que ainda nao exista:

```sh
make gerador
/tmp/final_act_gerador_mapas assets/musicas/twist_and_shout/audio.mp3 /tmp/mapas_twist
```

Para testar o jogo sem sobrescrever o executavel preexistente:

```sh
make TARGET=/tmp/final_act_twist_and_shout
GALLIUM_DRIVER=d3d12 /tmp/final_act_twist_and_shout
```

Verificacoes executadas: carregamento real do mapa, duracao contra o audio
decodificado pela raylib, imagem PNG, uso das cinco pistas, ausencia de
sobreposicoes invalidas e simulacao integral em 30/60/144/240 Hz. Todas as
329 notas e oito sustentacoes foram resolvidas sem perdas. Esses testes
simulam o relogio de audio: nao equivalem a ouvir/jogar a faixa completa.
A conferencia auditiva e a avaliacao humana da dificuldade ainda estao pendentes.

SHA256 do MP3 preservado:
`e1882ec42842c43db848359cb5555a3a1bcc61babd3739d283f2d0c237740333`.
SHA256 do fundo original preservado:
`59c0773e3f3047180ac0e5f370caad80b7e42fec89e1906526f117b6fd388ca1`.
