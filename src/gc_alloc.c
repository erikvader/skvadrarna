#include "include/heap_metadata.h"
#include "include/object_metadata.h"
#include <string.h>


void gca_set_mem_to_zero(void *obj, size_t bytes) {
    memset(obj, 0, bytes);
}

void *gca_alloc_data(heap_t *heap, size_t bytes) {

    void *heap_pointer = hm_get_free_space(heap, bytes);

    if(heap_pointer == NULL) {
        return NULL;
    }

    om_build_pointerless(heap, heap_pointer, bytes);

    gca_set_mem_to_zero(heap_pointer, bytes);
    return heap_pointer;
}

void *gca_alloc_struct(heap_t *heap, char *format) {
    size_t bytes = om_size_format(format); //får storleken på strukten som format visar på

    /*
     * Denna del görs även i om_build.
     *

    char *new_format = h_alloc_data(heap, bytes);

    new_format = strcpy(new_format, format);
    */

    void *heap_pointer = hm_get_free_space(heap, bytes); //får tillräckligt stor pekare på heapen

    if(heap_pointer == NULL) {
        return NULL;
    }

    bool try = om_build(heap, heap_pointer, format); //Bygger headern till objektet.

    if(try == false) {
            return NULL;
        }

    gca_set_mem_to_zero(heap_pointer, bytes);
    return heap_pointer;
}
