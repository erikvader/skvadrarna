# Library source files
export SRCDIR=$(CURDIR)/src
# Test source files
TESTDIR=test
# Complete binary files
export OUTDIR=$(CURDIR)/out
# .o files, mostly
export BUILDDIR=$(CURDIR)/intermediates
# .d files (GCC dependency files)
export DEPSDIR=$(BUILDDIR)/deps

export CC=gcc
export CFLAGS=-g -Wall
export DEPSFLAGS=-MM -MP
export LDFLAGS=-L $(OUTDIR) -lskvadrarna

export LIBRARYNAME=libskvadrarna.a
export TESTEXECNAME=skvadrarna_test

.PHONY: all test memtest dirs clean format demo

all: $(OUTDIR)/$(LIBRARYNAME)

$(OUTDIR)/$(LIBRARYNAME): dirs
	@make -C $(SRCDIR) $@ --no-print-directory


$(OUTDIR)/$(TESTEXECNAME): dirs $(OUTDIR)/$(LIBRARYNAME)
	@make -C $(TESTDIR) $(OUTDIR)/$(TESTEXECNAME) --no-print-directory

test: $(OUTDIR)/$(TESTEXECNAME)
	$(OUTDIR)/$(TESTEXECNAME)

memtest: $(OUTDIR)/$(TESTEXECNAME)
	valgrind --leak-check=full --track-origins=yes $(OUTDIR)/$(TESTEXECNAME)

dirs:
	@mkdir -p $(OUTDIR) $(BUILDDIR) $(DEPSDIR)

clean:
	rm -rf $(OUTDIR) $(BUILDDIR) $(DEPSDIR)
	@make -C demo clean

format:
	find ./src -name *.c | xargs -d'\n' -n1 astyle -n --options=astylerc

demo: all
	@make -C demo demo
