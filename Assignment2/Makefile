CC = gcc $(CFLAGS)
CFLAGS = -lpthread

all: hr-timer functionCall systemCall processSwitch threadSwitch
program: hr-timer.o functionCall.o systemCall.o processSwitch.o threadSwitch.o
my_timer.o: hr-timer.c functionCall.c systemCall.c processSwitch.c threadSwitch.c

clean:
	rm -f hr-timer hr-timer.o
	rm -f functionCall functionCall.o
	rm -f systemCall systemCall.o
	rm -f processSwitch processSwitch.o
	rm -f threadSwitch threadSwitch.o
run: hr-timer
	./hr-timer
