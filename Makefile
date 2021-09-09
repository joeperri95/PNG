
CC = gcc
EXE = png
GUNZIP_EXE = gunzip
TEST_EXE = test
CRC_EXE = crc32
ADLER_EXE = adler32
BUILDDIR = build
APPSDIR = applications
CFLAGS = -IlibInflate

all: pre_build png gunzip crc32 adler32
    @:

pre_build:
	mkdir -p $(BUILDDIR)

crc32: clean libs $(BUILDDIR)/crc32.o 
	cd $(BUILDDIR) && $(CC) $(CFLAGS) crc32.o checksum.o fileutils.o logging.o -o $(CRC_EXE) 

adler32: clean libs $(BUILDDIR)/adler32.o 
	cd $(BUILDDIR) && $(CC) $(CFLAGS) adler32.o checksum.o fileutils.o logging.o -o $(ADLER_EXE) 

png: clean libs $(BUILDDIR)/png_decode.o 
	cd $(BUILDDIR) && $(CC) $(CFLAGS) png_decode.o image.o checksum.o png.o bitstream.o huffman.o inflate.o logging.o gzip.o -o $(EXE)

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

$(BUILDDIR)/crc32.o: $(APPSDIR)/crc32.c
	$(CC) $(APPSDIR)/crc32.c -c -o $(BUILDDIR)/crc32.o -I. $(CFLAGS)

$(BUILDDIR)/adler32.o: $(APPSDIR)/adler32.c
	$(CC) $(APPSDIR)/adler32.c -c -o $(BUILDDIR)/adler32.o -I. $(CFLAGS)

$(BUILDDIR)/png_decode.o: $(APPSDIR)/png_decode.c
	$(CC) $(APPSDIR)/png_decode.c -c -o $(BUILDDIR)/png_decode.o -I. $(CFLAGS)

$(BUILDDIR)/gunzip.o: $(APPSDIR)/gunzip.c
	$(CC) $(APPSDIR)/gunzip.c -c -o $(BUILDDIR)/gunzip.o -I. $(CFLAGS)

clean:
	cd $(BUILDDIR) && rm -f *.o $(EXE) $(GUNZIP_EXE) $(TEST_EXE) $(CRC_EXE) $(ADLER_EXE)

.PHONY: clean
