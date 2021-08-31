CC = gcc
TEST_EXE = test
BUILDDIR = ../build
CFLAGS = -I../libInflate -I.
OBJECTS = 

all: tests
	@:

# just build all object files
libs: clean 
	cd ../libInflate && $(MAKE)

# build with debug flag
debug: CFLAGS += -g
debug: all
	@:

tests: clean libs $(BUILDDIR)/test.o $(BUILDDIR)/test_huffman.o $(BUILDDIR)/test_png.o $(BUILDDIR)/test_bitstream.o $(BUILDDIR)/test_inflate.o 
	cd $(BUILDDIR)  && $(CC) test.o inflate.o png.o bitstream.o huffman.o logging.o test_png.o test_bitstream.o test_huffman.o test_inflate.o $(CFLAGS)  -o $(TEST_EXE)

$(BUILDDIR)/test.o: test.c
	$(CC) -c test.c -o $(BUILDDIR)/test.o $(CFLAGS) 

$(BUILDDIR)/test_png.o: test_png.c
	$(CC) -c test_png.c -o $(BUILDDIR)/test_png.o $(CFLAGS) 
	
$(BUILDDIR)/test_bitstream.o: test_bitstream.c
	$(CC) -c test_bitstream.c -o $(BUILDDIR)/test_bitstream.o $(CFLAGS) 
	
$(BUILDDIR)/test_huffman.o: test_huffman.c
	$(CC) -c test_huffman.c -o $(BUILDDIR)/test_huffman.o $(CFLAGS) 
	
$(BUILDDIR)/test_inflate.o: test_inflate.c	
	$(CC) -c test_inflate.c -o $(BUILDDIR)/test_inflate.o $(CFLAGS) 

clean:
	cd $(BUILDDIR) && rm -f *.o $(TEST_EXE)

.PHONY: clean
