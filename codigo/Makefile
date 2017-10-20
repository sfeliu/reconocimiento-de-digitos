CC=g++
CFLAGS= -std=c++11 -Wall -O3

all: clean main.bin cross_validation.bin

main.bin: main.cpp matriz.o ocr.o
	$(CC) $(CFLAGS) $^ -o $@

cross_validation.bin: cross_validation.cpp matriz.o ocr.o
	$(CC) $(CFLAGS) $^ -o $@

ocr.o: lib/ocr.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

matriz.o: lib/matriz.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f *.bin