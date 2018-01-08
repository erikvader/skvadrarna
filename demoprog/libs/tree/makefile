exe = tree_test
lib=tree.a
sources = tree.c
objects=$(patsubst %.c,%.o,$(sources))
test_sources=tree_test.c
test_objects=$(patsubst %.c,%.o,$(test_sources))
flags=-Wall -g -MMD
bflags=-lcunit
dep=$(patsubst %.o,%.d,$(objects)) $(patsubst %.o,%.d,$(test_objects))

.PHONY: compile
compile: $(lib)

$(lib): $(objects)
	ar rc $@ $^
	ranlib $@

.PHONY: test
test: $(exe)
	./$(exe)

$(exe): $(test_objects) $(lib)
	$(CC) $(flags) -o $@ $^ $(bflags)

-include $(dep)

.c.o:
	$(CC) $(flags) -o $@ -c $<

.PHONY: clean
clean:
	-rm -fr $(objects) $(exe) $(test_objects) $(lib) $(dep)
