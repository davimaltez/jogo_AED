# Makefile para o projeto FINAL ACT (Jogo de Ritmo)
# Configuracao para ambiente Linux / WSL

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Nome do executavel final
TARGET = final_act

# Arquivos fonte
SRCS = principal.c catalogo_musicas.c

# Regra padrao: compila o projeto
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Regra para compilar e executar diretamente
run: $(TARGET)
	./$(TARGET)

# Regra para limpar os arquivos gerados
clean:
	rm -f $(TARGET)

.PHONY: all run clean
