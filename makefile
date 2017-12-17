export CC=gcc
export CFLAGS=-g -Wall
# Complete binary files
export OUTDIR=$(CURDIR)/out
# .o files, mostly
export BUILDDIR=$(CURDIR)/intermediates
# .d files (GCC dependency files)
export DEPSDIR=$(BUILDDIR)/deps

.PHONY: test memtest dirs clean format

dirs:
	@mkdir -p $(OUTDIR) $(BUILDDIR) $(DEPSDIR)

clean:
	rm -rf $(OUTDIR) $(BUILDDIR) $(DEPSDIR)

format:
	find ./src -name *.c | xargs -d'\n' -n1 astyle -n --options=astylerc
