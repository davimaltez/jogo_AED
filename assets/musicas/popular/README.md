# Popular (Wicked)

Dificuldade 4/5 no catalogo. Usa a versao com letra.

- `popular-from-wicked-the-soundtrack-1_U0A7jBty.mp3`: audio usado pelo jogo;
  225,290 s decodificados pela raylib.
- `popular.wav`: instrumental antigo (231,262 s); nao e mais usado pelo catalogo.
- `fundo.png`: fundo da partida, PNG 2752x1536.
- `notas.txt`: nivel 4 do gerador offline, 572 notas e 46 sustentacoes.
- `mapas_gerados/notas_1.txt` ate `notas_5.txt`: candidatos preservados;
  respectivamente 234, 312, 457, 572 e 760 notas (0, 21, 32, 46 e 54 longas).
- `mapas_gerados/notas_anterior_ressincronizado.txt`: mapa anterior (368 notas,
  feito sobre `popular.wav`), deslocado em -0,675 s para a versao com letra.
  Fica guardado para comparacao ou retorno.
- `ranking.txt`: pontuacoes registradas com o mapa antigo.

O ultimo evento termina em 225,245 s. O mapa usa as cinco pistas D/F/G/J/K
(47/142/226/113/44 notas). Ele foi gerado a partir da mistura completa do MP3,
com os parametros padrao de `ferramentas/gerador_mapas/`. Nao e transcricao
manual. BPM estimado pelo gerador: 90,67 (confianca 0,505).

Para reproduzir os candidatos, na raiz do projeto dentro do WSL, escolha
uma pasta de saida que ainda nao exista:

```sh
make gerador
/tmp/final_act_gerador_mapas assets/musicas/popular/popular-from-wicked-the-soundtrack-1_U0A7jBty.mp3 /tmp/mapas_popular
```

Verificacoes executadas: os seis mapas passaram por `carregar_notas_arquivo`
e `validar_duracao_notas` (da lista de notas do jogo) contra 225,290 s.
A conferencia auditiva e a avaliacao humana da dificuldade ainda estao pendentes.

SHA256 do MP3:
`2bc3e7edd7e55b6b07e4546e26320eed59796a47a89aab52fc23c2ead9a977ed`.
