CFLAGS+=-g -Wall -pipe

.PHONY: install

mausberry-switch: switch.c
	$(CC) $(CFLAGS) switch.c -o mausberry-switch

install: mausberry-switch
	install mausberry-switch $(DESTDIR)/usr/bin/mausberry-switch

clean:
	rm -f mausberry-switch
