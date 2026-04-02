CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
SRC = src/main.c src/app_state.c src/auth.c src/checkin.c src/console.c \ src/denuncia.c src/emergencia.c src/menu.c src/monitoramento.c \ src/telefones.c src/ver_dados.c src/storage.c
TARGET = viva_segura

$(TARGET): $(SRC) $(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean: rm -f $(TARGET) $(TARGET).exe data/*.tmp
