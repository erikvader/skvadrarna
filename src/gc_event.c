#include "include/gc_event.h"
#include "include/heap_metadata.h"
#include "include/stack_iter.h"
#include "include/object_metadata.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// extracts the last two bits of a pointer
static
char get_last_two(void **p) {
    char last = (uintptr_t)(*p) & 0b11;
    if (last != '\0') {
      *p = (void *)((uintptr_t)(*p) & (~0b11));
    }
    return last;
}

// restores the last two bits of a previously extracted pointer
static
void restore_last_two(void **p, char last) {
    last = last & 0b11;
    *p = (void *)((uintptr_t)(*p) | last);
}

// copy all live objects
static
void explore(heap_t *heap, void **obj, bool *unsafe_chunks, bool *locked) {
    assert(obj != NULL);
    char backup = get_last_two(obj);
    if(!hm_pointer_exists(heap, *obj)) {
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
            size_t obj_size = om_size(*obj);
            void *copy = hm_alloc_spec_chunk(heap, obj_size, locked);
            if(copy == NULL) {
                fprintf(stderr, "Garbage collection error: Not enough space, too many live objects or threshold too high.\n");
                exit(1);
            }
            om_build_copy(copy, *obj); //dest, src
            memcpy(copy, *obj, obj_size);
            om_set_forwarding(*obj, copy);
            *obj = copy;
        }

        //loop all pointers
        int pointer_num = om_amount_pointers(*obj);
        void **pointers[pointer_num];
        om_get_pointers(*obj, pointers);

        restore_last_two(obj, backup);

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

void gce_run_gc_event_dbg(heap_t *heap, bool unsafe) {
    fprintf(stderr, "<<<<<<<<<<<<<<<<<<<<<<<<<<<< GC ran!!! before: %d", (int)h_used(heap));//TODO: debug

    int num_chunks = hm_get_amount_chunks(heap);

    //starting point for stack searching
    void **stack_search;

    //unsafe
    bool unsafe_chunks[num_chunks];
    //memset(unsafe_chunks,false,num_chunks);
    init_variable_sized_array(unsafe_chunks, num_chunks, false);

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
    //memset(unsafe_chunks,false,num_chunks);
    init_variable_sized_array(locked, num_chunks, false);
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

    fprintf(stderr, ", after: %d\n", (int)h_used(heap)); //TODO: remove!

}

void gce_run_gc_event(heap_t *heap) {
    gce_run_gc_event_dbg(heap, hm_is_unsafe(heap));
}
