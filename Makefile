IDIR =inc
LOG_LEVEL ?= LOG_LEVEL_FATAL
CFLAGS=-I$(IDIR) -DLOG_LEVEL=$(LOG_LEVEL) -Wall -lpthread -g -lm

SDIR=src
ODIR=obj

_COMMON_OBJ = queue.o logs.o main.o mandelbrot.o
COMMON_OBJ=$(patsubst %,$(ODIR)/%,$(_COMMON_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
ifdef N_THREADS
	$(CC) -c -o $@ $< $(CFLAGS) -DN_THREADS=$(N_THREADS)
else
	$(CC) -c -o $@ $< $(CFLAGS)
endif


build: prog

prog: $(COMMON_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: build
	./prog $(ARGS)

.PHONY: benchmark
benchmark: build
	$(MAKE) clean build
	hyperfine $(ARGS) 'make run ARGS=bench/test.case'



.PHONY: clean
clean:
	rm -f $(ODIR)/*.o
	rm -f prog

