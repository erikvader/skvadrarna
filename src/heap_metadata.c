#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "include/heap_metadata.h"

// Note that many functions in this file are written heavily under the assumption that CHUNK_SIZE is divisible by OBJECT_ALIGNMENT
#define CHUNK_SIZE 2048
#define MIN_OBJ_SIZE 16
#define OBJECT_ALIGNMENT MIN_OBJ_SIZE
const size_t header_size = 8; // \TODO: fix this so it imports instead 

typedef uint8_t bitarr_t;

/*
 * The metadata consists of this struct and two arrays containing allocation data.
 * The rest of the heap is divided into chunks, for which allocation data is kept
 * in the arrays.
 */
typedef struct heap_header {
    // the start of where the free space is at init
    void *chunks_start;
    // size of the usable heap, that is, heap_size - header_size
    size_t heap_siz;
    // size of each chunk
    size_t chunk_siz;
    // if the stack is safe or not
    bool unsafe_stack;
    //bit for checking whether objects are explored or not
    bool exploration_bit;
    // the threshold for when a gc_event should trigger
    float gc_threshold;
    // the bit_array storing which addresses are allocated
    bitarr_t *alloc_map;
    // arr of pointers for where the free space begins in every chunk
    void **free_pointers;
} heap_header_t;


void set_addr_allocated(heap_t *heap, void *addr, bool allocated) {
    heap_header_t *head = (heap_header_t *) heap;
    int index = (addr - head -> chunks_start) / OBJECT_ALIGNMENT;
    int byte_index = index / 8;
    int bit_index = index % 8;
    head->alloc_map[byte_index] &= ~(1 << bit_index);
    head->alloc_map[byte_index] |= (allocated << bit_index);
}



bool get_addr_allocated(heap_t *heap, void *addr) {
    heap_header_t *head = (heap_header_t *) heap;
    size_t index = (addr - head -> chunks_start) / OBJECT_ALIGNMENT;
    int byte_index = index / 8;
    int bit_index = index % 8;
    uint8_t byte = head -> alloc_map[byte_index];
    return byte & (1 << bit_index);
}



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

// Calculates where the first chunk should start (i.e. where the header should end)
void* get_chunks_start(heap_t *header_start, size_t total_size) {
    void *heap_end = (void *) header_start + total_size;
    total_size -= sizeof(heap_header_t);
    int n_chunks = total_size / CHUNK_SIZE;

    void *chunks_start;
    do {
        size_t alloc_map_size = (n_chunks * CHUNK_SIZE / OBJECT_ALIGNMENT) * sizeof(bitarr_t) / 8;
        chunks_start = (void *) header_start + sizeof(heap_header_t) + sizeof(void *) * n_chunks + alloc_map_size;
        chunks_start = align_pointer(chunks_start);
        n_chunks--;
    } while (chunks_start + n_chunks * CHUNK_SIZE > heap_end);
    return chunks_start;
}

void hm_init(heap_t *heap, size_t size, bool unsafe_stack, float gc_threshold) {
    heap_header_t *head = (heap_header_t *) heap;
    head -> chunks_start = get_chunks_start(heap, size);

    size_t available_space = (void *) heap + size - head->chunks_start;
    head -> heap_siz = available_space / CHUNK_SIZE * CHUNK_SIZE; // Rounds down to whole chunks
    head -> chunk_siz = CHUNK_SIZE;
    head -> unsafe_stack = unsafe_stack;
    head -> gc_threshold = gc_threshold;
    head -> free_pointers = ((void *) heap) + sizeof(heap_header_t);
    head -> exploration_bit = true;
    int n_chunks = hm_get_amount_chunks(heap);
    for(int i = 0; i < n_chunks; i++) {
        head->free_pointers[i] = head->chunks_start + i * head->chunk_siz;
    }
    head -> alloc_map = (bitarr_t *) head -> free_pointers + sizeof(void *) * n_chunks;
    for(void *addr = head->chunks_start; addr < head -> chunks_start + head->heap_siz; addr += OBJECT_ALIGNMENT) {
        set_addr_allocated(heap, addr, false);
    }
}

size_t hm_measure_header_size(size_t heap_siz) {
    int n_chunks = heap_siz / CHUNK_SIZE;
    size_t struct_size = sizeof(heap_header_t);
    size_t free_pointer_arr_size = sizeof(void *) * n_chunks;
    size_t alloc_map_size = (n_chunks * CHUNK_SIZE / OBJECT_ALIGNMENT) * sizeof(bitarr_t) / 8;
    return struct_size + free_pointer_arr_size + alloc_map_size;
}


/*
 * Allocation/deallocation functions
 */
size_t chunk_calc_avail_space(heap_t *heap, chunk_t chunk) {
    heap_header_t *header = (heap_header_t *) heap;
    void *chunk_start = header->chunks_start + header->chunk_siz * chunk;
    size_t used_space = header->free_pointers[chunk] - chunk_start;
    return header->chunk_siz - used_space;
}

void *hm_get_free_space(heap_t *heap, size_t obj_siz) { //TODO: Must work with mutiple objects in same chunk.
    int n_chunks = hm_get_amount_chunks(heap);
    bool banned_chunks[n_chunks];
    memset(banned_chunks, false, n_chunks);
    return hm_alloc_spec_chunk(heap, obj_siz, banned_chunks);
}

bool chunk_can_alloc(heap_t *heap,chunk_t index,size_t obj_size) {
  heap_header_t *head = (heap_header_t *) heap;
  //to_align
  
  //OBJECT_ALIGNMENT
  uintptr_t int_pointer = (uintptr_t) head->free_pointers[index];
  uintptr_t int_pointer_mod = int_pointer;

  int_pointer_mod += header_size;
  int_pointer_mod += OBJECT_ALIGNMENT - (int_pointer_mod % OBJECT_ALIGNMENT);

  size_t tot_size = obj_size + (int_pointer_mod - int_pointer);
  return chunk_calc_avail_space(heap,index) >= tot_size;  
  
}

void *hm_alloc_spec_chunk(heap_t *heap, size_t obj_siz, bool *ban) {
    if(obj_siz == 0) {
        return NULL;
    }
    if(obj_siz > CHUNK_SIZE) {
        return NULL;
    }
    //if(obj_siz < MIN_OBJ_SIZE) {
    //    obj_siz = MIN_OBJ_SIZE;
    //}
    heap_header_t *head = (heap_header_t *) heap; //So we're able to use header metadata
    //void *free_space = head->chunks_start;
    for(int i = 0; i < hm_get_amount_chunks(heap); i++) {
      if(chunk_can_alloc(heap,i,obj_siz) && !ban[i]) {
        //printf("%p , ", head->free_pointers[i]);
        head->free_pointers[i] += header_size;
        
        head->free_pointers[i] = align_pointer(head->free_pointers[i]);
        void *allocated = head->free_pointers[i];
        //printf("%p\n", head->free_pointers[i]);
        set_addr_allocated(heap, allocated, true);
        head->free_pointers[i] += obj_siz;
        
            
        return allocated;
      }
      //free_space += head->chunk_siz;
    }
    return NULL;
}

void hm_reset_chunk(heap_t *heap, chunk_t index) {
    assert(heap && index >= 0 && index < hm_get_amount_chunks(heap));
    heap_header_t *header = (heap_header_t *) heap;
    void *chunk_start = header->chunks_start + header->chunk_siz * index;
    for(void *addr = chunk_start; addr < header->free_pointers[index]; addr += OBJECT_ALIGNMENT) {
        set_addr_allocated(heap, addr, false);
    }

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
    void *chunk_start = header->chunks_start;

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
    void *upper_limit = (head -> chunks_start) + (head -> heap_siz);
    void *lower_limit = (head -> chunks_start);
    if(pointer <= upper_limit && pointer >= lower_limit) {
        if((uintptr_t) pointer % OBJECT_ALIGNMENT == 0) {
            return get_addr_allocated(heap, pointer);
        }
    }
    return false;
}

bool hm_get_explored_bit(heap_t *heap) {
    heap_header_t *header = (heap_header_t *) heap;
    return (header -> exploration_bit);
}


void hm_toggle_explored_bit(heap_t *heap) {
    heap_header_t *header = (heap_header_t *) heap;
    header -> exploration_bit = !(header -> exploration_bit);
}
