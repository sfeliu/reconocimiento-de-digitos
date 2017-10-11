CC=g++
CFLAGS= -Wall -O3

all: main cross_validation
	
main: main.cpp matriz.o fputils.o ocr.o
	$(CC) $(CFLAGS) $^ -o $@
	
cross_validation: cross_validation.cpp matriz.o fputils.o ocr.o
	$(CC) $(CFLAGS) $^ -o $@
	
ocr.o: lib/ocr.cpp
	$(CC) -c $(CFLAGS) $^ -o $@
	
matriz.o: lib/matriz.cpp
	$(CC) -c $(CFLAGS) $^ -o $@
	
fputils.o: lib/fputils.cpp
	$(CC) -c $(CFLAGS) $^ -o $@
	
clean:
	rm -f *.o
	rm -f main