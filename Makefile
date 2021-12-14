IDIR =inc
LOG_LEVEL ?= 8
CFLAGS=-I$(IDIR) -DLOG_LEVEL=$(LOG_LEVEL) -Wall -lpthread -g -lm

SDIR=src
ODIR=obj

_COMMON_OBJ = queue.o logs.o main.o mandelbrot.o
COMMON_OBJ=$(patsubst %,$(ODIR)/%,$(_COMMON_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(COMMON_OBJ)
	$(CC) -o prog $^ $(CFLAGS) $(LIBS)

run: build
	./prog $(ARGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f prog

