
CC = gcc
EXE = png
GUNZIP_EXE = gunzip
TEST_EXE = test
BUILDDIR = build
CFLAGS = -IlibInflate

all: png gunzip
    @:

png: clean libs $(BUILDDIR)/main.o 
	cd $(BUILDDIR) && $(CC) $(CFLAGS) main.o png.o bitstream.o huffman.o inflate.o logging.o gzip.o -o $(EXE)

gunzip: clean libs $(BUILDDIR)/gunzip.o
	cd $(BUILDDIR) && $(CC) $(CFLAGS) gunzip.o png.o bitstream.o huffman.o inflate.o logging.o gzip.o -o $(GUNZIP_EXE) 

# just build all object files
libs: clean 
	cd libInflate && $(MAKE)
	cd ..

# build with debug flag
debug: CFLAGS += -g
debug: all
	@:

tests: clean 
	cd tests && $(MAKE)

tests2: clean test.o png.o bitstream.o huffman.o inflate.o logging.o test_png.o test_bitstream.o test_huffman.o test_inflate.o 
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

$(BUILDDIR)/main.o: main.c
	$(CC) main.c -c -o $(BUILDDIR)/main.o -I. $(CFLAGS)

$(BUILDDIR)/gunzip.o: gunzip.c
	$(CC) gunzip.c -c -o $(BUILDDIR)/gunzip.o -I. $(CFLAGS)

clean:
	cd $(BUILDDIR) && rm -f *.o $(EXE) $(GUNZIP_EXE) $(TEST_EXE)

.PHONY: clean
