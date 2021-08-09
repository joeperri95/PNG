
CC = gcc
EXE = png
GUNZIP_EXE = gunzip
TEST_EXE = test
BUILDDIR = build
CFLAGS =

all: clean main.o libs 
	$(CC) $(CFLAGS) main.o png.o bitstream.o huffman.o inflate.o logging.o gzip.o -o $(EXE)

gunzip: clean gunzip.o libs
	$(CC) $(CFLAGS) gunzip.o png.o bitstream.o huffman.o inflate.o logging.o gzip.o -o $(GUNZIP_EXE) 


# just build all object files
libs: clean png.o bitstream.o huffman.o inflate.o logging.o gzip.o
	@:

# build with debug flag
debug: CFLAGS += -g
debug: all
	@:

tests: clean test.o png.o bitstream.o huffman.o inflate.o logging.o test_png.o test_bitstream.o test_huffman.o test_inflate.o 
	$(CC) test.o inflate.o png.o bitstream.o huffman.o logging.o test_png.o test_bitstream.o test_huffman.o test_inflate.o -I. -Itests -o $(TEST_EXE)

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
	$(CC) main.c -c -o main.o -I. $(CFLAGS)

png.o: png.c
	$(CC) png.c -c -o png.o -I. $(CFLAGS)

inflate.o: inflate.c
	$(CC) inflate.c -c -o inflate.o -I. $(CFLAGS)

bitstream.o: bitstream.c
	$(CC) bitstream.c -c -o bitstream.o -I. $(CFLAGS)

huffman.o: huffman.c
	$(CC) huffman.c -c -o huffman.o -I. $(CFLAGS)

logging.o: logging.c
	$(CC) logging.c -c -o logging.o -I. $(CFLAGS)

gzip.o: gzip.c
	$(CC) gzip.c -c -o gzip.o -I. $(CFLAGS)

gunzip.o: gunzip.c
	$(CC) gunzip.c -c -o gunzip.o -I. $(CFLAGS)

clean:
	-@rm -f *.o $(EXE) $(GUNZIP_EXE) $(TEST_EXE)

.PHONY: clean
