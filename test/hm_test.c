#include <CUnit/CUnit.h>
#include "include/heap_metadata.h"
#include "hm_test.h"

#define CHUNK_SIZE 2048

// Unit tests for the heap metadata module

void test_hm_measure_required_space() {
    // This is a bit difficult to test without making it too dependent
    // on implementation.
    size_t head_size = hm_measure_required_space(0);
    size_t head_size2 = hm_measure_required_space(1);
    CU_ASSERT_EQUAL(head_size, head_size2);
    head_size2 = hm_measure_required_space(CHUNK_SIZE - 1);
    CU_ASSERT_EQUAL(head_size, head_size2);
    head_size = hm_measure_required_space(CHUNK_SIZE);
    CU_ASSERT_TRUE(head_size2 < head_size);
    head_size2 = hm_measure_required_space(2*CHUNK_SIZE);
    CU_ASSERT_TRUE(head_size < head_size2);
}

void test_hm_get_amount_chunks() {
    int n_samples = 100;
    char metadata[hm_measure_required_space((n_samples-1)* CHUNK_SIZE)];

    for (int n = 0; n < n_samples; n++) {
        hm_init((heap_t *) metadata, CHUNK_SIZE*n, false, 1);
        CU_ASSERT_EQUAL(hm_get_amount_chunks((heap_t *) metadata), n);
    }
}

void test_hm_get_pointer_chunk() {
    int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
    hm_init((heap_t *) metadata, CHUNK_SIZE*n_chunks, false, 1);
    void *chunk_pointer = metadata;
    CU_ASSERT_EQUAL(hm_get_pointer_chunk((heap_t *) metadata, chunk_pointer), -1);
    chunk_pointer += head_size;
    for (int n = 0; n < n_chunks; n++) {
        CU_ASSERT_EQUAL(hm_get_pointer_chunk((heap_t *) metadata, chunk_pointer), n);
        chunk_pointer += CHUNK_SIZE - 1;
        CU_ASSERT_EQUAL(hm_get_pointer_chunk((heap_t *) metadata, chunk_pointer), n);
        chunk_pointer++;
    }
    CU_ASSERT_EQUAL(hm_get_pointer_chunk((heap_t *) metadata, chunk_pointer), -1);
}

void add_hm_test_suites() {
    CU_pSuite pSuite = CU_add_suite("", NULL, NULL);
    CU_ADD_TEST(pSuite, test_hm_measure_required_space);
    CU_ADD_TEST(pSuite, test_hm_get_amount_chunks);
    CU_ADD_TEST(pSuite, test_hm_get_pointer_chunk);
}
