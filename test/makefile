CFLAGS+= -I $(SRCDIR)
SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=$(BUILDDIR)/%.o)

.PHONY: $(OUTDIR)/$(TESTEXECNAME)

$(OUTDIR)/$(TESTEXECNAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit $(LDFLAGS)

-include $(SRCS:%.c=$(DEPSDIR)/%.d)

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@
	$(CC) $(DEPSFLAGS) $(CFLAGS) $^ > $(DEPSDIR)/$*.d
