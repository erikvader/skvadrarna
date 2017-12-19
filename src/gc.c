#include <stdlib.h>
#include "include/gc.h"

size_t global_side_siz = 2048;


typedef struct heap_header{
  void *heap_start;
  size_t heap_siz;
  size_t side_siz;
  bool unsafe_stack;
  float gc_threshold;
  bool *used_arr;
  void **free_pointers;
} heap_header_t;

heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold) {
  heap_t *heap = (heap_t *) malloc(bytes);

  
  
  heap_header_t heap_header = { .heap_start =  (((void *)heap) + sizeof(heap_header_t)),
                                .heap_siz = bytes,
                                .side_siz = global_side_siz,
                                .unsafe_stack = unsafe_stack,
                                .gc_threshold = gc_threshold,
                                .used_arr = NULL,
                                .free_pointers = NULL};

  return heap;
}
