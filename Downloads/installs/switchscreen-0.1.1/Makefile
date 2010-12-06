
CC=gcc
CFLAGS=-O2 -Wall -I/usr/X11R6/include
LDFLAGS=-L/usr/X11R6/lib

switchscreen: switchscreen.o
	gcc -Wall -o switchscreen switchscreen.o ${LDFLAGS} -lX11

clean:
	rm -f *~ *.o switchscreen

