#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "include/heap_metadata.h"

#define CHUNK_SIZE 2048
#define OBJECT_ALIGNMENT 4

typedef bool bitarr_t;

typedef struct heap_header {
    // the start of where the free space is at init
    void *heap_start;
    // size of the usabe heap, that is, heap_size - header_size
    size_t heap_siz;
    // should always be 2048
    size_t chunk_siz;
    // if the stack is safe or not
    bool unsafe_stack;
    //bit for checking whether objects are explored or not.
    bool exploration_bit;
    // the threashold for when a gc_event should trigger
    float gc_threshold;
    // the bit_array for which space is used
    bitarr_t *used_arr;
    // arr of pointers fore where the free space begins in every chunk
    void **free_pointers;
} heap_header_t;


/*
 * Initialization functions
 */
// Rounds a pointer upwards, so that pointer % OBJECT_ALIGNMENT == 0
void *align_pointer(void *pointer) {
    uintptr_t pval = (uintptr_t) pointer;
    --pval;
    pval /= OBJECT_ALIGNMENT;
    pval *= OBJECT_ALIGNMENT;
    return (void *) pval + OBJECT_ALIGNMENT;
}

void hm_init(heap_t *heap, size_t size, bool unsafe_stack, float gc_threshold) {
    heap_header_t *head = (heap_header_t *) heap;
    void *unaligned_heap_start = ((void *)heap) + hm_measure_required_space(size);
    head -> heap_start = align_pointer(unaligned_heap_start);

    size_t available_space = unaligned_heap_start + size - head->heap_start;
    head -> heap_siz = available_space / CHUNK_SIZE * CHUNK_SIZE; // Rounds down to whole chunks

    head -> chunk_siz = CHUNK_SIZE;
    head -> unsafe_stack = unsafe_stack;
    head -> gc_threshold = gc_threshold;

    head -> free_pointers = ((void *) heap) + sizeof(heap_header_t);
    head -> exploration_bit = true;
    int n_chunks = hm_get_amount_chunks(heap);
    for(int i = 0; i < n_chunks; i++) {
        head->free_pointers[i] = head->heap_start + i * head->chunk_siz;
    }
}

size_t hm_measure_required_space(size_t heap_siz) {
    int n_chunks = heap_siz / CHUNK_SIZE;
    return sizeof(heap_header_t) + sizeof(void *) * n_chunks;
}


/*
 * Allocation/deallocation functions
 */
size_t chunk_calc_avail_space(heap_t *heap, chunk_t chunk) {
    heap_header_t *header = (heap_header_t *) heap;
    void *chunk_start = header->heap_start + header->chunk_siz * chunk;
    size_t used_space = header->free_pointers[chunk] - chunk_start;
    return header->chunk_siz - used_space;
}

void *hm_get_free_space(heap_t *heap, size_t obj_siz) { //TODO: Must work with mutiple objects in same chunk.
    int n_chunks = hm_get_amount_chunks(heap);
    bool banned_chunks[n_chunks];
    memset(banned_chunks, false, n_chunks);
    return hm_alloc_spec_chunk(heap, obj_siz, banned_chunks);
}

void *hm_alloc_spec_chunk(heap_t *heap, size_t obj_siz, bool *ban) {
    if(obj_siz == 0) {
        return NULL;
    }
    if(obj_siz > CHUNK_SIZE) {
        return NULL;
    }
    heap_header_t *head = (heap_header_t *) heap; //So we're able to use header metadata
    void *free_space = head->heap_start;
    for(int i = 0; i < hm_get_amount_chunks(heap); i++) {
        if(chunk_calc_avail_space(heap, i) >= obj_siz && !ban[i]) {
            void *allocated = head->free_pointers[i];
            head->free_pointers[i] += obj_siz;
            head->free_pointers[i] = align_pointer(head->free_pointers[i]);
            return allocated;
        }
        free_space += head->chunk_siz;
    }
    return NULL;
}

void hm_reset_chunk(heap_t *heap, chunk_t index) {
    assert(heap && index >= 0 && index < hm_get_amount_chunks(heap));
    heap_header_t *header = (heap_header_t *) heap;
    void *chunk_start = header->heap_start + header->chunk_siz * index;
    header->free_pointers[index] = chunk_start;
}


/*
 * Memory availability/pressure functions
 */
bool hm_over_threshold(heap_t *heap) {
    heap_header_t *head = (heap_header_t *) heap;
    float used = (float) hm_size_used(heap);
    float total = (float)(hm_get_amount_chunks(heap) * (head -> chunk_siz));
    if((used / total) > (head -> gc_threshold)) {
        return true;
    } else {
        return false;
    }
}

size_t hm_size_available(heap_t *heap) { //
    size_t free_space = 0;
    for(int i = 0; i < hm_get_amount_chunks(heap); i ++) {
        free_space += chunk_calc_avail_space(heap, i);
    }
    return free_space;
}

size_t hm_size_used(heap_t *heap) {
    size_t used_space;
    heap_header_t *head = (heap_header_t *) heap;
    used_space = (head -> heap_siz) - hm_size_available(heap);
    return used_space;
}

void hm_get_used_chunks(heap_t *heap, bool *data) {
    assert(heap && data);
    int n_chunks = hm_get_amount_chunks(heap);
    for(int i = 0; i < n_chunks; i++) {
        data[i] = chunk_calc_avail_space(heap, i) < CHUNK_SIZE;
    }
}

/*
 * Other getters & info functions
 */
bool hm_is_unsafe(heap_t *heap) {
    heap_header_t *head = (heap_header_t *) heap;
    return (head -> unsafe_stack);
}


int hm_get_amount_chunks(heap_t *heap) {
    assert(heap);
    heap_header_t *header = (heap_header_t *) heap;
    return header->heap_siz / header->chunk_siz;
}

chunk_t hm_get_pointer_chunk(heap_t *heap, void *pointer) {
    assert(heap);
    heap_header_t *header = (heap_header_t *) heap;
    int n_chunks = hm_get_amount_chunks(heap);
    void *chunk_start = header->heap_start;

    for(int i = 0; i < n_chunks; i++) {
        void *next_chunk = chunk_start + header->chunk_siz;
        if(chunk_start <= pointer && pointer < next_chunk) {
            return i;
        }
        chunk_start = next_chunk;
    }
    return -1;
}

bool hm_pointer_exists(heap_t *heap, void *pointer) {
    heap_header_t *head = (heap_header_t *) heap;
    void *upper_limit = (head -> heap_start) + (head -> heap_siz);
    void *lower_limit = (head -> heap_start);
    if(pointer <= upper_limit && pointer >= lower_limit) {
        return true;
    } else {
        return false;
    }
}

bool hm_get_explored_bit(heap_t *heap) {
    heap_header_t *header = (heap_header_t *) heap;
    return (header -> exploration_bit);
}


void hm_toggle_explored_bit(heap_t *heap) {
    heap_header_t *header = (heap_header_t *) heap;
    header -> exploration_bit = !(header -> exploration_bit);
}
