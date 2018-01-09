#include <CUnit/CUnit.h>
#include "include/heap_metadata.h"
#include "hm_test.h"

#define CHUNK_SIZE 2048
#define MAX_OBJ_SIZE (CHUNK_SIZE - 16)
#define ALIGNMENT 16
// Maximum possible padding due to alignment, used to ensure a minimum heap size is reached.
// E.g. a heap given CHUNK_SIZE + ALIGNMENT_PADDING size will always be able to hold one chunk.
#define ALIGNMENT_PADDING (ALIGNMENT - 1)

#define HEAP_INIT(n_chunks, threshold) size_t head_size = hm_measure_header_size(n_chunks * CHUNK_SIZE + ALIGNMENT_PADDING); \
                                        char metadata[head_size]; \
                                        heap_t *heap = (heap_t *) metadata; \
                                        hm_init(heap, head_size + n_chunks * CHUNK_SIZE + ALIGNMENT_PADDING, false, threshold);

// Unit tests for the heap metadata module

void test_hm_init() {
    size_t head_size = hm_measure_header_size(CHUNK_SIZE + ALIGNMENT_PADDING);
    char metadata[head_size + 1];
    heap_t *heap = (heap_t *) metadata;
    memset(heap, -1, head_size + 1);
    hm_init(heap, head_size + CHUNK_SIZE + ALIGNMENT_PADDING, false, 1);
    CU_ASSERT_TRUE(metadata[0] != -1);
    CU_ASSERT_TRUE(metadata[head_size - 1] != -1);
    CU_ASSERT_TRUE(metadata[head_size] == -1);
}

void test_hm_get_amount_chunks() {
    int n_samples = 10;
    for(int n = 0; n < n_samples; n++) {
        HEAP_INIT(n, 1);
        CU_ASSERT_EQUAL(hm_get_amount_chunks(heap), n);
    }
}

void test_get_free_space() {
    //Allocates all heap space.
    int n_chunks = 10;
    size_t test_size = CHUNK_SIZE / 4 - 16;
    HEAP_INIT(n_chunks, 1);
    for(int i = 0; i < 4 * n_chunks; i++) {
        CU_ASSERT_TRUE(hm_get_free_space(heap, test_size) != NULL);
    }
    CU_ASSERT_EQUAL(hm_get_free_space(heap, test_size), NULL);
}

void test_get_free_space2() {
    //tests what happens when u reserve space of size 0
    size_t empty_object = 0;
    int n_chunks = 10;
    HEAP_INIT(n_chunks, 1);
    CU_ASSERT_EQUAL(hm_get_free_space(heap, empty_object), NULL);
}

void test_get_free_space3() {
    //Reserving space larger than Chunk_size.
    size_t too_big_object = CHUNK_SIZE + 1;
    int n_chunks = 10;
    HEAP_INIT(n_chunks, 1);
    CU_ASSERT_EQUAL(hm_get_free_space(heap, too_big_object), NULL);
}

void test_get_free_space4() {
    // Checking alignment of allocated objects
    size_t obj_size = CHUNK_SIZE/4;
    int n_chunks = 2;
    HEAP_INIT(n_chunks, 1);
    CU_ASSERT_TRUE((uintptr_t) hm_get_free_space(heap, obj_size) % ALIGNMENT == 0);
    CU_ASSERT_TRUE((uintptr_t) hm_get_free_space(heap, obj_size-1) % ALIGNMENT == 0);
    CU_ASSERT_TRUE((uintptr_t) hm_get_free_space(heap, obj_size-2) % ALIGNMENT == 0);
    CU_ASSERT_TRUE((uintptr_t) hm_get_free_space(heap, obj_size-3) % ALIGNMENT == 0);
    CU_ASSERT_TRUE((uintptr_t) hm_get_free_space(heap, obj_size) % ALIGNMENT == 0);
}


void test_alloc_spec_chunk1() {
    size_t object = 100;
    int n_chunks = 10;
    HEAP_INIT(n_chunks, 1);
    bool banned_chunks[n_chunks];
    memset(banned_chunks, true, n_chunks);
    CU_ASSERT_TRUE(hm_alloc_spec_chunk(heap, object, banned_chunks) == NULL);
}

void test_alloc_spec_chunk2() {
    int n_chunks = 8;
    HEAP_INIT(n_chunks, 1);
    bool banned_chunks[8] = {true, true, true, false, false, true, false, true};
    for (int i = 0; i < 3; i++) {
        void *allocated = hm_alloc_spec_chunk(heap, CHUNK_SIZE - 16, banned_chunks);
        CU_ASSERT_TRUE(allocated != NULL);
        chunk_t chunk = hm_get_pointer_chunk(heap, allocated);
        CU_ASSERT_TRUE(chunk == 3 || chunk == 4 || chunk == 6);
    }
    CU_ASSERT_TRUE(hm_alloc_spec_chunk(heap, 1, banned_chunks) == NULL);
}


void test_hm_over_threshold() {
    size_t chunk_object = CHUNK_SIZE - 16;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 0.5);
    hm_get_free_space(heap, chunk_object);
    hm_get_free_space(heap, chunk_object);
    hm_get_free_space(heap, chunk_object);
    CU_ASSERT_TRUE(hm_over_threshold(heap) == true);
}


void test_hm_under_threshold() {
    size_t object = 100;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 0.5);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    CU_ASSERT_TRUE(hm_over_threshold(heap) != true);
}




void test_hm_size_available1() {
    size_t object = 0;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    hm_get_free_space(heap, object);
    CU_ASSERT_EQUAL(hm_size_available(heap), n_chunks * CHUNK_SIZE);
}

void test_hm_size_available2() {
    size_t object = MAX_OBJ_SIZE;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    hm_get_free_space(heap, object);
    CU_ASSERT_EQUAL(hm_size_available(heap), CHUNK_SIZE * (n_chunks - 1));
}

void test_hm_size_available3() {
    size_t object = MAX_OBJ_SIZE;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    CU_ASSERT_EQUAL(hm_size_available(heap), 0);
}

void test_hm_size_used1() {
    size_t object = 0;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    hm_get_free_space(heap, object);
    CU_ASSERT_EQUAL(hm_size_used(heap), 0);
}

void test_hm_size_used2() {
  size_t object = MAX_OBJ_SIZE;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    hm_get_free_space(heap, object);
    CU_ASSERT_EQUAL(hm_size_used(heap), CHUNK_SIZE);
}

void test_hm_size_used3() {
    size_t object = MAX_OBJ_SIZE - 1;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    hm_get_free_space(heap, object);
    CU_ASSERT_TRUE(hm_size_used(heap) >= object * 5); // We can't know exact value, since the objects might be padded for alignment
}

void test_hm_pointer_exists1() {
    size_t object = MAX_OBJ_SIZE;
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    void *test_pointer = hm_get_free_space(heap, object);
    CU_ASSERT_TRUE(hm_pointer_exists(heap, test_pointer) == true);
    CU_ASSERT_TRUE(hm_pointer_exists(heap, test_pointer - 1) == false);
    CU_ASSERT_TRUE(hm_pointer_exists(heap, test_pointer + 1) == false);
}


void test_hm_pointer_exists2() {
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    void *test_pointer = "banan rullar snabbtare än applena";
    CU_ASSERT_TRUE(hm_pointer_exists(heap, test_pointer) == false);
}

void test_hm_pointer_exists3() {
    int n_chunks = 5;
    HEAP_INIT(n_chunks, 1);
    void *test_pointer = NULL;
    CU_ASSERT_TRUE(hm_pointer_exists(heap, test_pointer) == false);
}


void test_hm_measure_header_size() {
    // This is a bit difficult to test without making it too dependent
    // on implementation.
    size_t head_size = hm_measure_header_size(0);
    size_t head_size2 = hm_measure_header_size(1);
    CU_ASSERT_EQUAL(head_size, head_size2);
    head_size2 = hm_measure_header_size(CHUNK_SIZE - 1);
    CU_ASSERT_EQUAL(head_size, head_size2);
    head_size = hm_measure_header_size(CHUNK_SIZE);
    CU_ASSERT_TRUE(head_size2 < head_size);
    head_size2 = hm_measure_header_size(2 * CHUNK_SIZE);
    CU_ASSERT_TRUE(head_size < head_size2);
}

void test_hm_get_pointer_chunk() {
    int n_chunks = 10;
    HEAP_INIT(n_chunks, 1);
    void *chunk_pointer = metadata;
    CU_ASSERT_EQUAL(hm_get_pointer_chunk(heap, chunk_pointer), -1);
    chunk_pointer += head_size + ALIGNMENT_PADDING;
    for(int n = 0; n < n_chunks; n++) {
        CU_ASSERT_EQUAL(hm_get_pointer_chunk(heap, chunk_pointer), n);
        chunk_pointer += CHUNK_SIZE - 1 - ALIGNMENT_PADDING;
        CU_ASSERT_EQUAL(hm_get_pointer_chunk(heap, chunk_pointer), n);
        chunk_pointer += 1 + ALIGNMENT_PADDING;
    }
    CU_ASSERT_EQUAL(hm_get_pointer_chunk(heap, chunk_pointer), -1);
}

void test_hm_reset_chunk1() {
    int n_chunks = 10;
    HEAP_INIT(n_chunks, 1);

    for(int n = 0; n < n_chunks; n++) {
        void *p = hm_get_free_space(heap, CHUNK_SIZE-16); //alloc all available space
        
    }

    for(int n = 0; n < n_chunks; n++) {
        void *allocd = hm_get_free_space(heap, 1);
        CU_ASSERT_TRUE(allocd == NULL);
        hm_reset_chunk(heap, n);
        allocd = hm_get_free_space(heap, MAX_OBJ_SIZE);
        chunk_t chunk = hm_get_pointer_chunk(heap, allocd);
        CU_ASSERT_TRUE(chunk == n);
    }
}

void test_hm_reset_chunk2() {
    // Tests resetting of allocation map
    HEAP_INIT(1, 1);
    size_t obj1 = 153;
    size_t obj2 = 2;
    void *alloc1 = hm_get_free_space(heap, obj1);
    void *alloc2 = hm_get_free_space(heap, obj2);
    CU_ASSERT_TRUE(hm_pointer_exists(heap, alloc1));
    CU_ASSERT_TRUE(hm_pointer_exists(heap, alloc2));
    hm_reset_chunk(heap, 0);
    CU_ASSERT_FALSE(hm_pointer_exists(heap, alloc1));
    CU_ASSERT_FALSE(hm_pointer_exists(heap, alloc2));
}

void test_hm_get_used_chunks() {
    const int n_chunks = 8;
    HEAP_INIT(n_chunks, 1);

    bool banned[8] = {true, true, true, false, false, true, false, true};
    for(int i = 0; i < n_chunks; i++) {
        hm_alloc_spec_chunk(heap, MAX_OBJ_SIZE, banned);
    }

    bool used[n_chunks];
    hm_get_used_chunks(heap, used);
    for(int i = 0; i < n_chunks; i++) {
        CU_ASSERT_EQUAL(!banned[i], used[i]);
    }

}

void add_hm_test_suites() {
    CU_pSuite initSuite = CU_add_suite("Heap metadata initialization", NULL, NULL);
    CU_ADD_TEST(initSuite, test_hm_measure_header_size);
    CU_ADD_TEST(initSuite, test_hm_init);

    CU_pSuite allocSuite = CU_add_suite("Heap metadata allocation", NULL, NULL);
    CU_ADD_TEST(allocSuite, test_hm_reset_chunk1);
    CU_ADD_TEST(allocSuite, test_hm_reset_chunk2);
    CU_ADD_TEST(allocSuite, test_get_free_space);
    CU_ADD_TEST(allocSuite, test_get_free_space2);
    CU_ADD_TEST(allocSuite, test_get_free_space3);
    CU_ADD_TEST(allocSuite, test_get_free_space4);
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
