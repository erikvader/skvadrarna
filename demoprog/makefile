# to find source files in nested directories
# find . -name "*.c" | cut -c3- | xargs
CC = gcc
exe = lager
#$(filter-out tree.c, )
sources = $(wildcard libs/interface/*.c) $(wildcard libs/utils/utils.c)
objects=$(patsubst %.c,%.o,$(sources))
dep=$(patsubst %.o,%.d,$(objects))
flags=-MMD -Wall -g -Wl,--no-as-needed
bflags=
tags=GPATH GRTAGS GSYMS GTAGS
libs=libs/database/database.a libs/list/list.a libs/tree/tree.a libs/file/block_writer.a libs/stack/stack.a

.PHONY: all
all: $(exe)

.PHONY: run
run: all
	./$(exe)

.PHONY: test_all
test_all: compile_libs
	make -C libs/list test
	make -C libs/tree test
	make -C libs/database test

.PHONY: compile_libs
compile_libs:
	make -C libs/list compile
	make -C libs/tree compile
	make -C libs/file compile
	make -C libs/stack compile
	make -C libs/database compile

# usar inte objects no longer
$(exe): $(sources) compile_libs 
	$(CC) $(flags) -o $@ $(sources) $(libs) $(bflags)

-include $(dep)

.c.o:
	$(CC) $(flags) -o $@ -c $< $(bflags)

.PHONY: clean
clean:
	-rm -fr $(objects) $(exe) $(dep) $(tags) lager.d
	make -C libs/list clean
	make -C libs/tree clean
	make -C libs/file clean
	make -C libs/stack clean
	make -C libs/database clean

