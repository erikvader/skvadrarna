#include "include/gc_event.h"
#include "include/heap_metadata.h"
#include "include/stack_iter.h"
#include "include/object_metadata.h"
#include <string.h>
#include <stdio.h>

// extracts the last two bits of a pointer
static
char get_last_two(void **p) {
    char last = (unsigned long)(*p) & 0b11;
    *p = (void *)((unsigned long)(*p) & (~0b11));
    return last;
}

// restores the last two bits of a previously extracted pointer
static
void restore_last_two(void **p, char last) {
    *p = (void *)((unsigned long)(*p) | last);
}

// copy all live objects
static
void explore(heap_t *heap, void **obj, bool *unsafe_chunks, bool *locked) {
    char backup = get_last_two(obj);
    if(!hm_pointer_exists(heap, *obj)){
        restore_last_two(obj, backup);
        return;
    }

    if(om_has_forwarding(*obj)) {
        *obj = om_get_forwarding(*obj);
        restore_last_two(obj, backup);

    } else if(!om_get_explored(heap, *obj)) {
        om_set_explored(heap, *obj);
        int cur_chunk = hm_get_pointer_chunk(heap, *obj);

        if(!unsafe_chunks[cur_chunk]) {
            void *copy = hm_alloc_spec_chunk(heap, om_size(*obj), locked);
            if(copy == NULL){
                fprintf(stderr, "Garbage collection error: Not enough space, too many live objects or threshold too high.\n");
                exit(1);
            }
            om_copy(copy, *obj); //dest, src
            om_set_forwarding(*obj, copy);
            *obj = copy;
        }

        restore_last_two(obj, backup);

        //loop all pointers
        int pointer_num = om_amount_pointers(*obj);
        void **pointers[pointer_num];
        om_get_pointers(*obj, pointers);
        for(int i = 0; i < pointer_num; i++) {
            explore(heap, pointers[i], unsafe_chunks, locked);
        }
    }
}

static
void init_variable_sized_array(bool *arr, int arr_size, bool init) {
    for(int i = 0; i < arr_size; i++) {
        arr[i] = init;
    }
}

void gce_run_gc_event(heap_t *heap) {

    int num_chunks = hm_get_amount_chunks(heap);

    //starting point for stack searching
    void **stack_search;

    //unsafe
    bool unsafe = hm_is_unsafe(heap);
    bool unsafe_chunks[num_chunks];
    init_variable_sized_array(unsafe_chunks, false, num_chunks);

    //mark all unsafe
    if(unsafe) {
        stack_search = (void **)&stack_search;
        while(stack_search != NULL) {
            stack_search = si_next_pointer(heap, stack_search);
            if(stack_search != NULL) {
                int cur_chunk = hm_get_pointer_chunk(heap, *stack_search);
                unsafe_chunks[cur_chunk] = true;
            }
        }

    }

    //get locked chunks
    bool locked[num_chunks];
    init_variable_sized_array(locked, false, num_chunks);
    hm_get_used_chunks(heap, locked);

    //explore
    stack_search = (void **)&stack_search;
    while(stack_search != NULL) {
        stack_search = si_next_pointer(heap, stack_search);
        if(stack_search != NULL) {
            explore(heap, stack_search, unsafe_chunks, locked);
        }
    }

    //free chunks
    for(int i = 0; i < num_chunks; i++) {
        if(!unsafe_chunks[i] && locked[i]) {
            hm_reset_chunk(heap, i);
        }
    }

    hm_toggle_explored_bit(heap);

}

