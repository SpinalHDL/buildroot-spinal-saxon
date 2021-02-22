CC = /media/data/open/SaxonSoc/artyA7SmpUpdate/buildroot-build/host/bin/riscv32-linux-gcc
#CC = riscv64-unknown-elf-gcc
CFLAGS =  -O3 -Wall -Wextra -c   -I/media/data/open/SaxonSoc/artyA7SmpUpdate/mandelbrot
LFLAGS = -lm 

#-I/media/data/open/SaxonSoc/artyA7SmpUpdate/buildroot-build/build/linux-headers-spinal-5.10-latest/usr/include

mandelbrot: mandelbrot.o
	$(CC)  mandelbrot.o $(LFLAGS) -o mandelbrot

mandelbrot.o: mandelbrot.c
	$(CC)   $(CFLAGS) mandelbrot.c

all: mandelbrot

clean:
	rm mandelbrot
	rm mandelbrot.o
