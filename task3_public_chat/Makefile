CC=gcc
CFLAGS=-Wall -Werror -Wextra -g
LIBS=-lpthread -lncurses -lrt

all: server client

server: server.c 
	$(CC) $(CFLAGS) -o server server.c $(LIBS)

client: client.c 
	$(CC) $(CFLAGS) -o client client.c $(LIBS)

clean:
	rm -f server client

.PHONY: all clean
