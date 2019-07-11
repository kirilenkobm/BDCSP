CC = gcc
CFLAGS = -fPIC -shared

all:
	$(CC) $(CFLAGS) -o libs/CSP.so src/CSP.c
