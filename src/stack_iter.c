#include "include/stack_iter.h"
//#include <stdio.h>
#include "include/heap_metadata.h"
#include <stdint.h>

extern char **environ;
#define bot ((uintptr_t) environ)
size_t global_alignment = sizeof(void *);

#define iter_step(mod,siz)     \
  if ( out > bot) {            \
    mod -= siz;                \
  }                            \
  else {                       \
    mod += siz;                \
  }                            \

#define get_pointer(p) (* ((void **) p))


void **si_next_pointer_dbg(heap_t *heap, void **top, si_pointer_check_fun fun, size_t alignment) {
  if(top == NULL) {
    return NULL;
  }
  if(top == (void **) bot) {
    return NULL;
  }
  
  uintptr_t out = (uintptr_t) top;
  
  if (out % alignment == 0) {
    iter_step(out, alignment);
  } else {
    if(out > bot) {
      out -= out % alignment;
    } else {
      out += (alignment - (out % alignment));
    }
  }

  while((out != bot) && (!fun(heap, get_pointer(out)))) {
      iter_step(out, alignment);
    }

    if(out == bot) {
        return NULL;
    } else {
      return (void **) out;
    }
}

/*void **si_next_pointer(heap_t *heap, void **top) {
    return si_next_pointer_dbg(heap, top, hm_pointer_exists, global_alignment);
    }*/
