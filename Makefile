CC = gcc
CFLAGS = -Wall -Wextra -std=c99
# Detecta o sistema: no Windows (MSYS2 UCRT64 + mingw32-make) a variavel OS
# vale Windows_NT; no WSL/Linux ela nao existe.
ifeq ($(OS),Windows_NT)
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
TARGET = final_act.exe
else
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
TARGET = final_act
endif

SRCS = principal.c interface.c partida.c midia_partida.c ranking.c catalogo_musicas.c lista_notas.c desempenho.c
HEADERS = interface.h partida.h midia_partida.h ranking.h catalogo_musicas.h lista_notas.h desempenho.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

ifeq ($(OS),Windows_NT)
clean:
	cmd /C "if exist $(TARGET) del /Q $(TARGET)"
else
clean:
	rm -f $(TARGET)
endif

.PHONY: all run clean
