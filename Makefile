
CC = gcc
EXE = png
GUNZIP_EXE = gunzip
TEST_EXE = test
BUILDDIR = build
CFLAGS = -IlibInflate

all: png gunzip
    @:

png: clean libs $(BUILDDIR)/main.o 
	cd $(BUILDDIR) && $(CC) $(CFLAGS) main.o checksum.o png.o bitstream.o huffman.o inflate.o logging.o gzip.o -o $(EXE)

gunzip: clean libs $(BUILDDIR)/gunzip.o
	cd $(BUILDDIR) && $(CC) $(CFLAGS) gunzip.o checksum.o png.o bitstream.o huffman.o inflate.o logging.o gzip.o -o $(GUNZIP_EXE) 

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

$(BUILDDIR)/main.o: main.c
	$(CC) main.c -c -o $(BUILDDIR)/main.o -I. $(CFLAGS)

$(BUILDDIR)/gunzip.o: gunzip.c
	$(CC) gunzip.c -c -o $(BUILDDIR)/gunzip.o -I. $(CFLAGS)

clean:
	cd $(BUILDDIR) && rm -f *.o $(EXE) $(GUNZIP_EXE) $(TEST_EXE)

.PHONY: clean
