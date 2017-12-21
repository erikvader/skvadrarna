#ifndef __heap_metadata_h__
#define __heap_metadata_h__

#include <stdlib.h>
#include <stdbool.h>
#include "gc.h"

typedef int chunk_t;
typedef bool bitarr_t; 

// creates the metadata object
// creates 3 different structs: heap_header, used_arr and free_pointers
void hm_init(heap_t *heap,size_t size,bool unsafe_stack, float gc_threshold);

// requests a place to store an object
// moves a free pointer accordingly
void *hm_get_free_space(heap_t *heap,size_t obj_siz);

// gets an adress to store an object in a specific chunk
void *hm_alloc_spec_chunk(heap_t *heap, size_t obj_siz, chunk_t index);

// check if gc is currently over threshold.
bool hm_over_threshold(heap_t *heap);

// gets size avaiable in the heap
size_t hm_size_available(heap_t *heap);

// gets the used amount of memory in the heap
size_t hm_size_used(heap_t *heap);

// asks if a pointer is a pointer on the stack, and point to an object on it
bool hm_pointer_exists(heap_t *heap, void *pointer);

// gets the amounts of chunk we have in the heap
int hm_get_amount_chunks(heap_t *heap);

// gets the chunk a pointer is pointing to
chunk_t hm_get_pointer_chunk(heap_t *heap, void *pointer);

// resets a chunk back to 0, effectivly clearing it.
void hm_reset_chunk(heap_t *heap, chunk_t index);

// returns a bool for every chunk, true if it is in use, false otherwise.
void hm_get_used_chunks(heap_t *heap, bool *data);


// will be hidden in the .c file.
typedef struct heap_header{
  // the start of where the free space is at init
  void *heap_start;
  // size of the usabe heap, that is, heap_size - header_size
  size_t heap_siz;
  // should always be 2048
  size_t chunk_siz;
  // if the stack is safe or not
  bool unsafe_stack;
  // the threashold for when a gc_event should trigger
  float gc_threshold;
  // the bit_array for which space is used
  bitarr_t *used_arr;
  // arr of pointers fore where the free space begins in every chunk
  void **free_pointers;
} heap_header_t;


#endif
