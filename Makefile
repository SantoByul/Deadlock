CC = gcc
CFLAGS = -Wall -Wextra

all: banker
	./banker 10 5 7

banker: banker.c
	$(CC) $(CFLAGS) -o banker banker.c


clean:
	rm -f banker result.txt