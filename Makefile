CC = gcc
#For older gcc, use -O3 or -O2 instead of -Ofast
# CFLAGS = -lm -pthread -Ofast -march=native -funroll-loops -Wno-unused-result

# Use -Ofast with caution. It speeds up training, but the checks for NaN will not work
# (-Ofast turns on --fast-math, which turns on -ffinite-math-only,
# which assumes everything is NOT NaN or +-Inf, so checks for NaN always return false
# see https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
# CFLAGS = -lm -pthread -Ofast -march=native -funroll-loops -Wall -Wextra -Wpedantic

CFLAGS = -lm -pthread -O3 -march=native -funroll-loops -Wall -Wextra -Wpedantic
BUILDDIR := build
SRCDIR := src
OBJDIR := $(BUILDDIR)
SCRIPTSDIR := scripts

OBJ := $(OBJDIR)/vocab_count.o $(OBJDIR)/cooccur.o $(OBJDIR)/shuffle.o $(OBJDIR)/glove.o
HEADERS := $(SRCDIR)/common.h
MODULES := $(BUILDDIR)/vocab_count $(BUILDDIR)/cooccur $(BUILDDIR)/shuffle $(BUILDDIR)/glove

PYTHONVERSION := 3.8
PYTHONINCLUDE := /usr/include/python$(PYTHONVERSION)
PYTHONLIBDIR := /usr/lib/python$(PYTHONVERSION)/config-$(PYTHONVERSION)-x86_64-linux-gnu
PYTHONLIB = python$(PYTHONVERSION)m
PYCFLAGS := -O3 -I$(PYTHONINCLUDE) -L$(PYTHONLIBDIR) -l$(PYTHONLIB) -lc

all: dir $(OBJ) $(MODULES)
dir :
	mkdir -p $(BUILDDIR)
$(BUILDDIR)/glove : $(OBJDIR)/glove.o $(OBJDIR)/common.o
	$(CC) $^ -o $@ $(CFLAGS)
$(BUILDDIR)/shuffle : $(OBJDIR)/shuffle.o $(OBJDIR)/common.o
	$(CC) $^ -o $@ $(CFLAGS)
$(BUILDDIR)/cooccur : $(OBJDIR)/cooccur.o $(OBJDIR)/common.o
	$(CC) $^ -o $@ $(CFLAGS)
$(BUILDDIR)/vocab_count : $(OBJDIR)/vocab_count.o $(OBJDIR)/common.o
	$(CC) $^ -o $@ $(CFLAGS)
$(OBJDIR)/%.o : $(SRCDIR)/%.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean scripts dir
clean:
	rm -rf $(BUILDDIR)

scripts: $(SCRIPTSDIR)/reencode $(SCRIPTSDIR)/twogram

$(SCRIPTSDIR)/reencode: $(SCRIPTSDIR)/reencode.pyx
	cython --embed $(SCRIPTSDIR)/reencode.pyx
	gcc $(SCRIPTSDIR)/reencode.c $(PYCFLAGS) -o $@
	rm $(SCRIPTSDIR)/reencode.c

$(SCRIPTSDIR)/twogram: $(SCRIPTSDIR)/twogram.pyx
	cython --embed $(SCRIPTSDIR)/twogram.pyx
	gcc $(SCRIPTSDIR)/twogram.c $(PYCFLAGS) -o $@
	rm $(SCRIPTSDIR)/twogram.c
