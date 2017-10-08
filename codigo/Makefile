CC=g++
CFLAGS= -Wall -O3

all: main experimentos
	
main: main.cpp lib/matriz.h lib/matriz.cpp lib/fputils.h lib/fputils.cpp lib/ocr.h lib/ocr.cpp
	$(CC) $(CFLAGS) $^ -o $@
	
experimentos: experimentos.cpp
	$(CC) $(CFLAGS) $^ -o $@
	
clean:
	rm -f *.o
	rm -f main
	rm -f experimentos