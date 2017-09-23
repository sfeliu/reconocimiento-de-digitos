CC=g++
CFLAGS= -Wall -O3

all: main experimentos
	
main: main.cpp
	$(CC) $(CFLAGS) $^ -o $@
	
experimentos: experimentos.cpp
	$(CC) $(CFLAGS) $^ -o $@
	
clean:
	rm -f *.o
	rm -f main
	rm -f experimentos