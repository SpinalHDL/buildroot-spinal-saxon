CC = /media/data/open/SaxonSoc/artyA7SmpUpdate/buildroot-build/host/bin/riscv32-linux-gcc

CFLAGS+=  -O3 -Wall -Wextra
LDFLAGS+= -lm

gpio-interrupt: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o gpio-interrupt

all: mandelbrot

clean:
	rm gpio-interrupt
	rm main.o
