#ifndef __heap_metadata_h__
#define __heap_metadata_h__
/// \file heap_metadata.h
/// \authors exp meta druid 200 LI+

#include <stdlib.h>
#include <stdbool.h>
#include "gc.h"

typedef int chunk_t;

/// The size of a heap header
const size_t heap_header_size;

/// Initializes a header. The header is placed at the location pointed to by the heap parameter.
///
/// \param heap the heap for which to allocate a header
/// \param size the size of the heap, should be a multiple of 2048
/// \param unsafe_stack true if pointers on the stack to this heap are to be considered unsafe pointers
/// \param gc_threshold the memory pressure at which gc should be triggered for this heap
void hm_init(heap_t *heap, size_t size, bool unsafe_stack, float gc_threshold);

/// Reserves a space on the heap where obj_size bytes can be written.
/// Marks that space as occupied.
///
/// \param heap the heap for which to find the space
/// \param obj_size the size of the space to reserve
/// \return a pointer to the reserved space, or NULL if no such space was available
void *hm_get_free_space(heap_t *heap, size_t obj_siz);

/// Reserves a space in a specific chunk where obj_size bytes can be written.
/// Marks that space as occupied.
///
/// \param heap the heap for which to find the space
/// \param obj_size the size of the space to reserve
/// \param index the chunk to reserve the memory in
/// \return a pointer to the reserved space, or NULL if no such space was available
void *hm_alloc_spec_chunk(heap_t *heap, size_t obj_siz, chunk_t index);

/// Checks if the percentage of allocated memory is above the threshold for garbage collection.
///
/// \param heap the heap to check
/// \param return true if the memory pressure is above the threshold
bool hm_over_threshold(heap_t *heap);

/// Gets the total available space in a heap
///
/// \param heap the heap
/// \return the available space in the heap
size_t hm_size_available(heap_t *heap);

/// Gets the total used space in a heap
///
/// \param heap the heap
/// \return the used space in the heap
size_t hm_size_used(heap_t *heap);

/// Checks if a pointer points to an active object on the heap.
/// \param the heap
/// \param pointer the pointer to check
/// \param true if the pointer points to a living object on the heap
bool hm_pointer_exists(heap_t *heap, void *pointer);

/// Gets the number of chunks on a heap
/// \param heap the heap
/// \return the number of chunks on the heap
int hm_get_amount_chunks(heap_t *heap);

/// Gets the chunk which contains the memory pointed to by pointer.
///
/// \param heap the heap
/// \pointer pointer to the memory to locate
/// \return if pointer points inside a chunk, gives that chunk, otherwise -1
chunk_t hm_get_pointer_chunk(heap_t *heap, void *pointer);

/// Marks all memory in a chunk as unoccupied, making it available
/// for future allocations.
///
/// \param heap the heap
/// \param index the chunk to reset
void hm_reset_chunk(heap_t *heap, chunk_t index);

// returns a bool for every chunk, true if it is in use, false otherwise.
/// Gives which chunks are used (have some occupied memory) and which are not.
/// This information is written into data, which should be an array able to fit
/// as many items as there are chunks in the heap, see \ref hm_get_amount_chunks
///
/// \param heap the heap
/// \param data the array to write the data to
void hm_get_used_chunks(heap_t *heap, bool *data);


#endif
