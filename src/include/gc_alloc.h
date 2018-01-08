#ifndef __gc_alloc_h__
#define __gc_alloc_h__

//#include <stdlib.h>
#include "gc.h"

/// Allocates an object based on a format string.
///
/// \param heap the heap where the object will be allocated
/// \param format the format of the object where *=pointer, i=int, c=char, 
///               l=long, f=float, d=double
/// \returns the pointer to the allocated object, or null if the heap is full.
void *gca_alloc_struct(heap_t *heap, char *format);

/// Allocates an object based on its size in bytes.
///
/// \param heap the heap where the object will be allocated
/// \param bytes the size in bytes
/// \returns the pointer to the allocated object, or null if the heap is full.
void *gca_alloc_data(heap_t *heap, size_t bytes);

#endif
