#CC = /media/data/open/SaxonSoc/artyA7SmpUpdate/buildroot-build/host/bin/riscv32-linux-gcc

CFLAGS+=  -O3 -Wall -Wextra
LDFLAGS+= -lm

mandelbrot: mandelbrot.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o mandelbrot

all: mandelbrot

clean:
	rm mandelbrot
	rm mandelbrot.o
