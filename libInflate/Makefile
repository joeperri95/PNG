
CC = gcc
BUILDDIR = ../build
CFLAGS =

all: libs
	@:

# just build all object files
libs: clean png.o bitstream.o huffman.o inflate.o logging.o gzip.o
	@:

# build with debug flag
debug: CFLAGS += -g
debug: all
	@:

png.o: png.c
	$(CC) png.c -c -o $(BUILDDIR)/png.o -I. $(CFLAGS)

inflate.o: inflate.c
	$(CC) inflate.c -c -o $(BUILDDIR)/inflate.o -I. $(CFLAGS)

bitstream.o: bitstream.c
	$(CC) bitstream.c -c -o $(BUILDDIR)/bitstream.o -I. $(CFLAGS)

huffman.o: huffman.c
	$(CC) huffman.c -c -o $(BUILDDIR)/huffman.o -I. $(CFLAGS)

logging.o: logging.c
	$(CC) logging.c -c -o $(BUILDDIR)/logging.o -I. $(CFLAGS)

gzip.o: gzip.c
	$(CC) gzip.c -c -o $(BUILDDIR)/gzip.o -I. $(CFLAGS)

clean:
	-@rm -f $(BUILDDIR)/*.o 

.PHONY: clean