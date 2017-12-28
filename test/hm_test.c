#include <CUnit/CUnit.h>
#include "include/heap_metadata.h"
#include "hm_test.h"

#define CHUNK_SIZE 2048

// Unit tests for the heap metadata module

void test_hm_init() {
    size_t head_size = hm_measure_required_space(CHUNK_SIZE);
    char metadata[head_size + 1];
    memset(metadata, -1, head_size + 1);
    hm_init((heap_t *) metadata, CHUNK_SIZE, false, 1);
    CU_ASSERT_TRUE(metadata[0] != -1);
    CU_ASSERT_TRUE(metadata[head_size - 1] != -1);
    CU_ASSERT_TRUE(metadata[head_size] == -1);
}

void test_hm_get_amount_chunks() {
    int n_samples = 100;
    char metadata[hm_measure_required_space((n_samples - 1)* CHUNK_SIZE)];

    for(int n = 0; n < n_samples; n++) {
        hm_init((heap_t *) metadata, CHUNK_SIZE * n, false, 1);
        CU_ASSERT_EQUAL(hm_get_amount_chunks((heap_t *) metadata), n);
    }
}

void test_reserve_space() {
    //Allocates all heap space.
    int n_chunks = 100;
    size_t test_size = 50;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 1);
    for(int i = 0; i < n_chunks; i++) {
        CU_ASSERT_TRUE(hm_reserve_space(test_heap, test_size) != NULL);
    }
    CU_ASSERT_EQUAL(hm_reserve_space(test_heap, test_size), NULL);
}

void test_reserve_space2() {
    //tests what happens when u reserve space of size 0
    size_t empty_object = 0;
    int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 1);
    CU_ASSERT_EQUAL(hm_reserve_space(test_heap, empty_object), NULL);
}

void test_reserve_space3() {
    //Reserving space larger than Chunk_size.
    size_t too_big_object = CHUNK_SIZE + 1;
    int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 1);
    CU_ASSERT_EQUAL(hm_reserve_space(test_heap, too_big_object), NULL);
}


void test_alloc_spec_chunk1() {
    size_t object = 100;
    int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 1);
    bool banned_chunks[n_chunks];
    memset(banned_chunks, true, n_chunks);
    CU_ASSERT_TRUE(hm_alloc_spec_chunk(test_heap, object, banned_chunks) == NULL);
}

void test_alloc_spec_chunk2() {
    int n_chunks = 8;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];

    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 1);
    bool banned_chunks[8] = {true, true, true, false, false, true, false, true};
    for (int i = 0; i < 3; i++) {
        void *allocated = hm_alloc_spec_chunk(test_heap, CHUNK_SIZE, banned_chunks);
        CU_ASSERT_TRUE(allocated != NULL);
        chunk_t chunk = hm_get_pointer_chunk(test_heap, allocated);
        CU_ASSERT_TRUE(chunk == 3 || chunk == 4 || chunk == 6);
    }
    CU_ASSERT_TRUE(hm_alloc_spec_chunk(test_heap, 1, banned_chunks) == NULL);
}


void test_hm_over_threshold() {
    size_t chunk_object = CHUNK_SIZE;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    hm_reserve_space(test_heap, chunk_object);
    hm_reserve_space(test_heap, chunk_object);
    hm_reserve_space(test_heap, chunk_object);
    CU_ASSERT_TRUE(hm_over_threshold(test_heap) == true);
}


void test_hm_under_threshold() {
    size_t object = 100;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    CU_ASSERT_TRUE(hm_over_threshold(test_heap) != true);
}




void test_hm_size_available1() {
    size_t object = 0;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    hm_reserve_space(test_heap, object);
    CU_ASSERT_EQUAL(hm_size_available(test_heap), 10240);
}

void test_hm_size_available2() {
    size_t object = CHUNK_SIZE;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    hm_reserve_space(test_heap, object);
    CU_ASSERT_EQUAL(hm_size_available(test_heap), 8192);
}

void test_hm_size_available3() {
    size_t object = CHUNK_SIZE;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    CU_ASSERT_EQUAL(hm_size_available(test_heap), 0);
}

void test_hm_size_used1() {
    size_t object = 0;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    hm_reserve_space(test_heap, object);
    CU_ASSERT_EQUAL(hm_size_used(test_heap), 0);
}

void test_hm_size_used2() {
    size_t object = CHUNK_SIZE;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    hm_reserve_space(test_heap, object);
    CU_ASSERT_EQUAL(hm_size_used(test_heap), CHUNK_SIZE);
}

void test_hm_size_used3() {
    size_t object = CHUNK_SIZE;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    hm_reserve_space(test_heap, object);
    CU_ASSERT_EQUAL(hm_size_used(test_heap), 10240);
}

void test_hm_pointer_exists1() {
    size_t object = CHUNK_SIZE;
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    void *test_pointer = hm_reserve_space(test_heap, object);
    CU_ASSERT_TRUE(hm_pointer_exists(test_heap, test_pointer) == true);
}


void test_hm_pointer_exists2() {
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    void *test_pointer = "banan rullar snabbtare än applena";
    CU_ASSERT_TRUE(hm_pointer_exists(test_heap, test_pointer) == false);
}

void test_hm_pointer_exists3() {
    int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE * n_chunks, false, 0.5);
    void *test_pointer = NULL;
    CU_ASSERT_TRUE(hm_pointer_exists(test_heap, test_pointer) == false);
}


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
    head_size2 = hm_measure_required_space(2 * CHUNK_SIZE);
    CU_ASSERT_TRUE(head_size < head_size2);
}

void test_hm_get_pointer_chunk() {
    int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];

    hm_init((heap_t *) metadata, CHUNK_SIZE * n_chunks, false, 1);
    void *chunk_pointer = metadata;
    CU_ASSERT_EQUAL(hm_get_pointer_chunk((heap_t *) metadata, chunk_pointer), -1);
    chunk_pointer += head_size;
    for(int n = 0; n < n_chunks; n++) {
        CU_ASSERT_EQUAL(hm_get_pointer_chunk((heap_t *) metadata, chunk_pointer), n);
        chunk_pointer += CHUNK_SIZE - 1;
        CU_ASSERT_EQUAL(hm_get_pointer_chunk((heap_t *) metadata, chunk_pointer), n);
        chunk_pointer++;
    }
    CU_ASSERT_EQUAL(hm_get_pointer_chunk((heap_t *) metadata, chunk_pointer), -1);
}

void test_hm_reset_chunk() {
    int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];
    hm_init((heap_t *) metadata, CHUNK_SIZE * n_chunks, false, 1);

    for(int n = 0; n < n_chunks; n++) {
        hm_reserve_space((heap_t *)metadata, CHUNK_SIZE); //alloc all available space
    }

    for(int n = 0; n < n_chunks; n++) {
        void *allocd = hm_reserve_space((heap_t *) metadata, 1);
        CU_ASSERT_TRUE(allocd == NULL);
        hm_reset_chunk((heap_t *) metadata, n);
        allocd = hm_reserve_space((heap_t *) metadata, CHUNK_SIZE);
        chunk_t chunk = hm_get_pointer_chunk((heap_t *) metadata, allocd);
        CU_ASSERT_TRUE(chunk == n);
    }
}

void test_hm_get_used_chunks() {
    const int n_chunks = 8;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE * n_chunks);
    char metadata[head_size];

    hm_init((heap_t *) metadata, CHUNK_SIZE * n_chunks, false, 1);
    bool banned[8] = {true, true, true, false, false, true, false, true};
    for(int i = 0; i < n_chunks; i++) {
        hm_alloc_spec_chunk((heap_t *) metadata, CHUNK_SIZE, banned);
    }

    bool used[n_chunks];
    hm_get_used_chunks((heap_t *) metadata, used);
    for(int i = 0; i < n_chunks; i++) {
        CU_ASSERT_EQUAL(!banned[i], used[i]);
    }

}

void add_hm_test_suites() {
    CU_pSuite initSuite = CU_add_suite("Heap metadata initialization", NULL, NULL);
    CU_ADD_TEST(initSuite, test_hm_measure_required_space);
    CU_ADD_TEST(initSuite, test_hm_init);

    CU_pSuite allocSuite = CU_add_suite("Heap metadata allocation", NULL, NULL);
    CU_ADD_TEST(allocSuite, test_hm_reset_chunk);
    CU_ADD_TEST(allocSuite, test_reserve_space);
    CU_ADD_TEST(allocSuite, test_reserve_space2);
    CU_ADD_TEST(allocSuite, test_reserve_space3);
    CU_ADD_TEST(allocSuite, test_alloc_spec_chunk1);
    CU_ADD_TEST(allocSuite, test_alloc_spec_chunk2);

    CU_pSuite gettersSuite = CU_add_suite("Heap metadata getters and query functions", NULL, NULL);
    CU_ADD_TEST(gettersSuite, test_hm_get_amount_chunks);
    CU_ADD_TEST(gettersSuite, test_hm_get_pointer_chunk);
    CU_ADD_TEST(gettersSuite, test_hm_get_used_chunks);
    CU_ADD_TEST(gettersSuite, test_hm_over_threshold);
    CU_ADD_TEST(gettersSuite, test_hm_under_threshold);
    CU_ADD_TEST(gettersSuite, test_hm_size_available1);
    CU_ADD_TEST(gettersSuite, test_hm_size_available2);
    CU_ADD_TEST(gettersSuite, test_hm_size_available3);
    CU_ADD_TEST(gettersSuite, test_hm_size_used1);
    CU_ADD_TEST(gettersSuite, test_hm_size_used2);
    CU_ADD_TEST(gettersSuite, test_hm_size_used3);
    CU_ADD_TEST(gettersSuite, test_hm_pointer_exists1);
    CU_ADD_TEST(gettersSuite, test_hm_pointer_exists2);
    CU_ADD_TEST(gettersSuite, test_hm_pointer_exists3);
}
