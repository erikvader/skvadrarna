#include "include/stack_iter.h"
//#include <stdio.h>
#include "include/heap_metadata.h"
#include <stdint.h>
#include <setjmp.h>

extern char **environ;
//extern void **test_bot;
#define bot ((uintptr_t) environ)
//#define bot ((uintptr_t) test_bot)
size_t global_alignment = sizeof(void *);

#define iter_step(mod,siz,dir) \
  if ( dir) {                  \
    mod -= siz;                \
  }                            \
  else {                       \
    mod += siz;                \
  }                            \

#define environ_check(curr,dir) (dir ? curr > bot : curr < bot)

#define get_pointer(p) (* ((void **) p))

#define Dump_registers()                        \
  jmp_buf env;                                  \
  if (setjmp(env)) abort();                     \


void **si_next_pointer_dbg(heap_t *heap, void **top, si_pointer_check_fun fun, size_t alignment) {
    if(top == NULL) {
        return NULL;
    }
    if(top == (void **) bot) {
        return NULL;
    }

    Dump_registers();

    uintptr_t out = (uintptr_t) top;

    bool descending = out > bot;

    if(out % alignment == 0) {
        iter_step(out, alignment, descending);
    } else {
        if(descending) {
            out -= out % alignment;
        } else {
            out += (alignment - (out % alignment));
        }
    }

    while(environ_check(out, descending) &&
            (!fun(heap, get_pointer(out)))) {
        iter_step(out, alignment, descending);
    }

    if(!environ_check(out, descending)) {
        return NULL;
    } else {
        return (void **) out;
    }
}

void **si_next_pointer(heap_t *heap, void **top) {
    return si_next_pointer_dbg(heap, top, hm_pointer_exists, global_alignment);
}
