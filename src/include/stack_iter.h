#ifndef __stack_iter_h__
#define __stack_iter_h__

#include "gc.h"

extern char **environ;


// steps towards environ until it finds a valid pointer that points on the heap.
void **si_next_pointer(heap_t heap,void **top);




#endif
