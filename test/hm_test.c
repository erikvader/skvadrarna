#include <CUnit/CUnit.h>
#include "include/heap_metadata.h"
#include "hm_test.h"

#define CHUNK_SIZE 2048

// Unit tests for the heap metadata module

void test_hm_get_amount_chunks() {
    char heap[heap_header_size];
    size_t
    hm_init((heap_t *) heap, CHUNK_SIZE*10, false, 1);

    CU_ASSERT_EQUAL(hm_get_used_chunks(heapm ), 0);
}

void add_hm_test_suites() {
    CU_pSuite pSuite = CU_add_suite("Asdf", NULL, NULL);
    CU_ADD_TEST(pSuite, test_hm_get_amount_chunks);
}
