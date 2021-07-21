
CC = gcc
EXE = png

all: clean main.o png.o bitstream.o huffman.o
	$(CC) main.o png.o bitstream.o huffman.o -o $(EXE)

debug: clean main.c png.c bitstream.c huffman.o
	$(CC) -c -g main.c -o main.o
	$(CC) -c -g png.c -o png.o
	$(CC) -c -g bitstream.c -o bitstream.o
	$(CC) -c -g huffman.c -o huffman.o
	$(CC) main.o png.o bitstream.o huffman.o -o $(EXE)


main.o: main.c
	$(CC) main.c -c -o main.o

png.o: png.c
	$(CC) png.c -c -o png.o -I.

bitstream.o: bitstream.c
	$(CC) bitstream.c -c -o bitstream.o -I.


huffman.o: huffman.c
	$(CC) huffman.c -c -o huffman.o -I.


clean:
	-@rm *.o $(EXE) 

.PHONY: clean
