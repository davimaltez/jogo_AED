# Makefile para o projeto FINAL ACT (Jogo de Ritmo)
# Configuracao para ambiente Linux / WSL

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Nome do executavel final
TARGET = final_act

# Arquivos fonte
SRCS = principal.c catalogo_musicas.c lista_notas.c desempenho.c
HEADERS = catalogo_musicas.h lista_notas.h desempenho.h
TEST_FLAGS = $(CFLAGS) -Werror -I.
TEST_BINS = /tmp/final_act_teste_carregamento_notas /tmp/final_act_teste_merge_sort_notas /tmp/final_act_teste_desempenho /tmp/final_act_teste_partida /tmp/final_act_teste_conclusao_partida

# Regra padrao: compila o projeto
all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Regra para compilar e executar diretamente
run: $(TARGET)
	./$(TARGET)

/tmp/final_act_teste_carregamento_notas: tests/teste_carregamento_notas.c lista_notas.c catalogo_musicas.c lista_notas.h catalogo_musicas.h
	$(CC) $(TEST_FLAGS) tests/teste_carregamento_notas.c lista_notas.c catalogo_musicas.c -lm -Wl,--wrap=malloc,--wrap=fgetc,--wrap=ferror,--wrap=fclose -o $@

/tmp/final_act_teste_merge_sort_notas: tests/teste_merge_sort_notas.c lista_notas.c lista_notas.h
	$(CC) $(TEST_FLAGS) tests/teste_merge_sort_notas.c lista_notas.c -lm -o $@

/tmp/final_act_teste_desempenho: tests/teste_desempenho.c desempenho.c lista_notas.c catalogo_musicas.c $(HEADERS)
	$(CC) $(TEST_FLAGS) tests/teste_desempenho.c desempenho.c lista_notas.c catalogo_musicas.c -lm -o $@

/tmp/final_act_teste_partida: tests/teste_partida.c principal.c desempenho.c lista_notas.c catalogo_musicas.c $(HEADERS)
	$(CC) $(TEST_FLAGS) tests/teste_partida.c desempenho.c lista_notas.c catalogo_musicas.c -Wl,--wrap=IsKeyPressed,--wrap=IsMouseButtonPressed,--wrap=GetFrameTime,--wrap=GetMousePosition,--wrap=SetConfigFlags,--wrap=ToggleFullscreen,--wrap=WindowShouldClose,--wrap=carregar_notas_arquivo,--wrap=iniciar_desempenho,--wrap=DrawText -o $@ $(LIBS)

/tmp/final_act_teste_conclusao_partida: tests/teste_conclusao_partida.c lista_notas.c desempenho.c lista_notas.h desempenho.h
	$(CC) $(TEST_FLAGS) tests/teste_conclusao_partida.c lista_notas.c desempenho.c -lm -o $@

test: /tmp/final_act_teste_carregamento_notas /tmp/final_act_teste_merge_sort_notas /tmp/final_act_teste_desempenho /tmp/final_act_teste_conclusao_partida
	/tmp/final_act_teste_carregamento_notas
	/tmp/final_act_teste_merge_sort_notas
	/tmp/final_act_teste_desempenho
	/tmp/final_act_teste_conclusao_partida

test-grafico: /tmp/final_act_teste_partida
	/tmp/final_act_teste_partida

# Regra para limpar os arquivos gerados
clean:
	rm -f $(TARGET) $(TEST_BINS)

.PHONY: all run clean test test-grafico
