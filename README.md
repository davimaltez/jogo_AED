# THE FINAL ACT

Jogo de ritmo com tema de **filmes musicais**, desenvolvido em **C** com a biblioteca gráfica **raylib** para a disciplina de Algoritmos e Estruturas de Dados.

O jogador escolhe uma música do catálogo e precisa apertar a tecla certa quando cada nota chega à linha dourada. Acertos somam pontos e combos; erros baixam o medidor de desempenho. Se o medidor zerar, é Game Over. Quem termina a música grava o nome no ranking daquela música.


## ▶️ Como executar

Dá para compilar de duas formas: pelo **WSL/Ubuntu** ou direto no **Windows** (MSYS2). O `Makefile` detecta o sistema sozinho. No Windows o áudio sai pelo WASAPI, sem o chiado que aparece no WSL.

### Opção A: WSL/Ubuntu

#### Requisitos

- Ubuntu/WSL
- GCC e Make
- raylib 5.5

#### 1. Instalar as dependências e a raylib (uma vez só)

```sh
sudo apt update
sudo apt install build-essential git libasound2-dev libx11-dev libxrandr-dev \
    libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev

git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
cd ../..
```

#### 2. Compilar e jogar

Dentro da pasta do projeto:

```sh
make
make run
```

### Opção B: Windows (MSYS2 UCRT64)

#### 1. Instalar (uma vez só)

Instale o [MSYS2](https://www.msys2.org/) e, no terminal **MSYS2 UCRT64**:

```sh
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-raylib
```

#### 2. Compilar e jogar

No PowerShell, dentro da pasta do projeto:

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;$env:PATH"   # gcc, make e a DLL da raylib
mingw32-make
.\final_act.exe
```

Isso gera `final_act.exe` (ignorado pelo git). Os testes (`make test`) continuam só no WSL.

### Controles

| Tecla | Ação |
|---|---|
| `D` `F` `G` `J` `K` | Tocar as notas das 5 pistas |
| `ESC` | Pausar / voltar |
| Setas + `Enter` | Navegar nos menus (o mouse também funciona) |
| `F11` | Alternar tela cheia |

---

## 🔗 Estrutura de dados: lista simplesmente encadeada

As notas de cada música ficam em uma **lista simplesmente encadeada**, que é a estrutura central da partida (`lista_notas.c`).

Cada nó é uma nota:

```c
typedef struct Nota {
    float tempo_alvo;       // segundo em que a nota deve ser tocada
    int pista;              // 0 = D, 1 = F, 2 = G, 3 = J, 4 = K
    EstadoNota estado;      // pendente, acertada ou perdida
    struct Nota *proximo;   // ponteiro para a próxima nota
} Nota;
```

Durante o jogo, a lista é usada para:

- **criar e inserir** as notas lidas do arquivo `notas.txt` (`criar_nota`, `inserir_nota`, `carregar_notas_arquivo`);
- **percorrer** e desenhar as notas que estão na tela;
- **buscar** a nota da pista apertada dentro da janela de acerto (`verificar_acerto`);
- **atualizar** o estado das notas que passaram da linha (`atualizar_notas`);
- **verificar** se todas foram resolvidas, para detectar a vitória (`todas_notas_resolvidas`);
- **liberar** toda a memória ao sair da partida (`liberar_lista_notas`).

---

## 🔀 Algoritmo de ordenação: Merge Sort

O projeto usa **Merge Sort** em três lugares:

| Onde | Estrutura | O que ordena | Arquivo |
|---|---|---|---|
| **Notas da música** | Lista encadeada | Notas por tempo, em ordem cronológica, ao carregar a música. Os ponteiros `proximo` são reorganizados, sem copiar os nós. | `lista_notas.c` (`merge_sort_notas`, `dividir_lista`, `intercalar_listas`) |
| **Catálogo de músicas** | Vetor de `Musica` | Título A-Z, dificuldade crescente ou decrescente (filtros da tela de seleção) | `catalogo_musicas.c` (`ordenar_catalogo`) |
| **Ranking** | Vetor de registros | Maior pontuação primeiro; no empate, nome em ordem alfabética | `ranking.c` (`ordenar_ranking`) |

O Merge Sort divide os dados ao meio até sobrar um elemento, depois intercala as partes já ordenadas. A complexidade é **O(n log n)**. A versão usada é **estável**: elementos iguais mantêm a ordem original.

