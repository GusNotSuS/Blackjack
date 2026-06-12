CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

OBJS = main.o jogo.o baralho.o mao.o

blackjack: $(OBJS)
	$(CC) $(CFLAGS) -o blackjack $(OBJS)

main.o: Codigos/main.c Codigos/jogo.h Codigos/baralho.h Codigos/mao.h Codigos/config.h
	$(CC) $(CFLAGS) -c Codigos/main.c -o main.o

jogo.o: Codigos/jogo.c Codigos/jogo.h Codigos/baralho.h Codigos/mao.h Codigos/config.h
	$(CC) $(CFLAGS) -c Codigos/jogo.c -o jogo.o

baralho.o: Codigos/baralho.c Codigos/baralho.h
	$(CC) $(CFLAGS) -c Codigos/baralho.c -o baralho.o

mao.o: Codigos/mao.c Codigos/mao.h Codigos/baralho.h
	$(CC) $(CFLAGS) -c Codigos/mao.c -o mao.o

clean:
	rm -f *.o blackjack

.PHONY: clean