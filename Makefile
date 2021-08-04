
CC = gcc
EXE = png
BUILDDIR = build

all: clean main.o png.o bitstream.o huffman.o inflate.o logging.o
	$(CC) main.o png.o bitstream.o huffman.o inflate.o logging.o -o $(EXE)

debug: clean main.c png.c bitstream.c huffman.c inflate.c
	$(CC) -c -g main.c -o main.o
	$(CC) -c -g png.c -o png.o
	$(CC) -c -g bitstream.c -o bitstream.o
	$(CC) -c -g huffman.c -o huffman.o
	$(CC) -c -g inflate.c -o inflate.o
	$(CC) main.o inflate.o png.o bitstream.o huffman.o -o $(EXE)

tests: clean test.o png.o bitstream.o huffman.o inflate.o logging.o	test_png.o test_bitstream.o test_huffman.o test_inflate.o 
	$(CC) -o test test.o inflate.o png.o bitstream.o huffman.o logging.o test_png.o test_bitstream.o test_huffman.o test_inflate.o -I. -Itests

test.o: tests/test.c
	$(CC) -c tests/test.c -o test.o -I. -Itests

test_png.o: tests/test_png.c
	$(CC) -c tests/test_png.c -o test_png.o -I. -Itests
	
test_bitstream.o: tests/test_bitstream.c
	$(CC) -c tests/test_bitstream.c -o test_bitstream.o -I. -Itests
	
test_huffman.o: tests/test_huffman.c
	$(CC) -c tests/test_huffman.c -o test_huffman.o -I. -Itests
	
test_inflate.o: tests/test_inflate.c	
	$(CC) -c tests/test_inflate.c -o test_inflate.o -I. -Itests

main.o: main.c
	$(CC) main.c -c -o main.o

png.o: png.c
	$(CC) png.c -c -o png.o -I.

inflate.o: inflate.c
	$(CC) inflate.c -c -o inflate.o -I.

bitstream.o: bitstream.c
	$(CC) bitstream.c -c -o bitstream.o -I.

huffman.o: huffman.c
	$(CC) huffman.c -c -o huffman.o -I.

logging.o: logging.c
	$(CC) logging.c -c -o logging.o -I.

clean:
	-@rm *.o $(EXE)  

.PHONY: clean
