CFLAGS+=-g -Wall -pipe

all: switch.c
	$(CC) $(CFLAGS) switch.c -o switch
