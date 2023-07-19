CFLAGS += -Wall -Werror -std=gnu11 -O2
LDFLAGS += -lm

CercaPercorso: CercaPercorso.c
	gcc $(CFLAGS) CercaPercorso.c -o CercaPercorso $(LDFLAGS)

