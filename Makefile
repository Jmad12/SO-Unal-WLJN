CC = gcc
CFLAGS = -lpthread

all: Cliente/p2-dogClient.c  Servidor/p2-dogServer.c Servidor/generador.c
	$(CC) -o Cliente/p2-dogClient Cliente/p2-dogClient.c $(CFLAGS)
	$(CC) -o Servidor/p2-dogServer Servidor/p2-dogServer.c $(CFLAGS)
	$(CC) -o Servidor/generador Servidor/generador.c
	
clear:
	rm -f Cliente/p2-dogClient Servidor/p2-dogServer Servidor/generador
