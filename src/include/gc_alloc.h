#ifndef __gc_alloc_h__
#define __gc_alloc_h__

//#include <stdlib.h>
#include "gc.h"

void *gca_alloc_struct(heap_t *heap, char *format);

void *gca_alloc_data(heap_t *heap, size_t bytes);

#endif
