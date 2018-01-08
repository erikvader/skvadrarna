exe = database_test
lib=database.a
sources = database.c db_file_save.c db_types.c db_undo.c
objects=$(patsubst %.c,%.o,$(sources))
test_sources=database_test.c
test_objects=$(patsubst %.c,%.o,$(test_sources))
flags=-Wall -g -MMD
bflags=-lcunit
dep=$(patsubst %.o,%.d,$(objects)) $(patsubst %.o,%.d,$(test_objects))
libs= ../list/list.a ../tree/tree.a ../file/block_writer.a ../stack/stack.a

.PHONY: compile
compile: $(lib)

$(lib): $(objects)
	ar rc $@ $^
	ranlib $@

.PHONY: test
test: $(exe)
	./$(exe)

.PHONY: valgrind
valgrind: $(exe)
	valgrind --leak-check=full ./$(exe)

#utgår ifrån att libs finns!!!
$(exe): $(test_objects) $(lib)
	$(CC) $(flags) -o $@ $^ $(libs) $(bflags)

-include $(dep)

.c.o:
	$(CC) $(flags) -o $@ -c $<

.PHONY: clean
clean:
	-rm -fr $(objects) $(exe) $(test_objects) $(lib) $(dep) databas
