CC=gcc
CFLAGS=

all: hr-timer functionCall systemCall
program: hr-timer.o functionCall.o systemCall.o
my_timer.o: hr-timer.c functionCall.c systemCall.c

clean:
	rm -f hr-timer hr-timer.o
	rm -f functionCall functionCall.o
	rm -f systemCall systemCall.o
run: hr-timer
	./hr-timer
