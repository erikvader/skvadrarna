SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=$(BUILDDIR)/%.o)

$(OUTDIR)/$(LIBRARYNAME): $(OBJS)
	ar r $@ $^
	ranlib $@
	@mkdir -p $(OUTDIR)/include/
	cp include/gc.h $(OUTDIR)/include/gc.h

-include $(SRCS:%.c=$(DEPSDIR)/%.d)

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@
	$(CC) $(DEPSFLAGS) $(CFLAGS) $^ > $(DEPSDIR)/$*.d
