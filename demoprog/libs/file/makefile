lib=block_writer.a
sources = block_writer.c
objects=$(patsubst %.c,%.o,$(sources))
flags=-Wall -g -MMD
bflags=-lcunit
dep=$(patsubst %.o,%.d,$(objects))

.PHONY: compile
compile: $(lib)

$(lib): $(objects)
	ar rc $@ $^
	ranlib $@

-include $(dep)

.c.o:
	$(CC) $(flags) -o $@ -c $<

.PHONY: clean
clean:
	-rm -fr $(objects) $(lib) $(dep)
