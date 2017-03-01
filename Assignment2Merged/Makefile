CC=gcc $(CFLAGS)
CFLAGS= -lpthread

all: timer
program: timer.o
my_timer.o: timer.c

clean:
	rm -f timer timer.o
run: timer
	./timer
