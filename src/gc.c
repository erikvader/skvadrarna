#include <stdlib.h>
#include "include/gc.h"
#include "include/heap_metadata.h"
#include "include/stack_iter.h"
#include "include/gc_alloc.h"
#include "include/gc_event.h"

#define LARGE_ADDRESS ((size_t) 0x10000000)


heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold) {
  void *mem_pos;
  posix_memalign(&mem_pos,LARGE_ADDRESS,bytes);
  heap_t *heap = mem_pos;
  
  hm_init(heap,bytes,unsafe_stack,gc_threshold);

  return heap;
}


void h_delete(heap_t *heap) {
  free(heap);
}

void h_delete_dbg(heap_t *heap, void *dbg_value) {
  void *p = NULL;
  void **iter = si_next_pointer(heap,&p);
  while (iter != NULL) {
    *iter = dbg_value;
    iter = si_next_pointer(heap,iter);
  }
  
  h_delete(heap);
}

void *h_alloc_struct(heap_t *heap, char *format) {
  if (hm_over_threshold(heap)) {
    h_gc(heap);
  }

  return gca_alloc_struct(heap,format);
}

void *h_alloc_data(heap_t *heap, size_t bytes) {
  if (hm_over_threshold(heap)) {
    h_gc(heap);
  }
  
  return gca_alloc_data(heap,bytes);
}

size_t h_gc(heap_t *heap) {
    return h_gc_dbg(heap, hm_is_unsafe(heap));
}


size_t h_gc_dbg(heap_t *heap, bool unsafe_stack) {
  size_t before = h_used(heap);

  gce_run_gc_event_dbg(heap, unsafe_stack);

  size_t after = h_used(heap);

  return before - after;
}

size_t h_avail(heap_t *heap) {
  return hm_size_available(heap);
}

size_t h_used(heap_t *heap) {
  return hm_size_used(heap);
}

