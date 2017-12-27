#include <CUnit/CUnit.h>
#include "include/heap_metadata.h"
#include "hm_test.h"

#define CHUNK_SIZE 2048

// Unit tests for the heap metadata module

void test_hm_init() 
{


}


void test_hm_get_amount_chunks() {
    int n_samples = 100;
    char metadata[hm_measure_required_space((n_samples-1)* CHUNK_SIZE)];

    for (int n = 0; n < n_samples; n++) {
        hm_init((heap_t *) metadata, CHUNK_SIZE*n, false, 1);
        CU_ASSERT_EQUAL(hm_get_amount_chunks((heap_t *) metadata), n);
    }
}

void test_reserve_space()
{
  	//Allocates all heap space.
	int n_chunks = 100;
	size_t test_size = 50;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    heap_header_t *head = hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 1);
	while (hm_reserve_space(heap, test_size))
	{
		puts("i donbt n�ow");
	}
	CU_ASSERT_EQUAL(hm_reserve_space(heap, test_size), NULL);
}

void test_reserve_space2()
{
  	//tests what happens when u reserve space of size 0
  	size_t empty_object = 0;
	int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 1);
	CU_ASSERT_EQUAL(hm_reserve_space(test_heap, empty_object), NULL);
}

void test_reserve_space3()
{
  	//Reserving space larger than Chunk_size.
	size_t too_big_object = 2049;
	int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 1);
	CU_ASSERT_EQUAL(hm_reserve_space(test_heap, too_big_object), NULL);
}


void test_alloc_spec_chunk1()
{
	size_t object = 100;
	int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 1);
	CU_ASSERT_TRUE(hm_alloc_spec_chunk(test_heap, object, 3) != NULL);
}

void test_alloc_spec_chunk2()
{
  	size_t object1 = 100;
	size_t object2 = 1949;
	int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 1);
	CU_ASSERT_TRUE(hm_alloc_spec_chunk(test_heap, object1, 3) != NULL);
	CU_ASSERT_EQUAL(hm_alloc_spec_chunk(test_heap, object2, 3), NULL);
}

void test_index_alloc_spec_chunk1()
{
  	size_t object = 100;
	int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 1);
	CU_ASSERT_TRUE(hm_alloc_spec_chunk(test_heap, object, 0) != NULL);
}

void test_index_alloc_spec_chunk2()
{  
  	size_t object = 100;
	int n_chunks = 100;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 1);
	CU_ASSERT_TRUE(hm_alloc_spec_chunk(test_heap, object, 101) == NULL);
}	
	
  	
void test_hm_over_threshold()
{
  	size_t chunk_object = 2048;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	hm_reserve_space(test_heap, chunk_object);
	hm_reserve_space(test_heap, chunk_object);
	hm_reserve_space(test_heap, chunk_object);
	CU_ASSERT_TRUE(hm_over_threshold(test_heap) == true);
} 


void test_hm_under_threshold()
{
  	size_t object = 100;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	hm_reserve_space(test_heap, chunk_object);
	hm_reserve_space(test_heap, chunk_object);
	CU_ASSERT_TRUE(hm_over_threshold(test_heap) != true);
} 




void test_hm_size_available1()
{
  	size_t object = 0;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	hm_reserve_space(test_heap, object);
	CU_ASSERT_EQUAL(hm_size_available(test_heap), 10240);
}

void test_hm_size_available2()
{
  	size_t object = 2048;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	hm_reserve_space(test_heap, object);
	CU_ASSERT_EQUAL(hm_size_available(test_heap), 8192);
}

void test_hm_size_available3()
{
  	size_t object = 2048;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	hm_reserve_space(test_heap, object);
	hm_reserve_space(test_heap, object);
	hm_reserve_space(test_heap, object);
	hm_reserve_space(test_heap, object);
	hm_reserve_space(test_heap, object);
	CU_ASSERT_EQUAL(hm_size_available(test_heap), 0);
}

void test_hm_size_used1()
{
  	size_t object = 0;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	hm_reserve_space(test_heap, object);
	CU_ASSERT_EQUAL(hm_size_available(test_heap), 0);
}

void test_hm_size_used2()
{
  	size_t object = 2048;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	hm_reserve_space(test_heap, object);
	CU_ASSERT_EQUAL(hm_size_used(test_heap), 2048);
}

void test_hm_size_used3()
{
  	size_t object = 2048;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	hm_reserve_space(test_heap, object);
	hm_reserve_space(test_heap, object);
	hm_reserve_space(test_heap, object);
	hm_reserve_space(test_heap, object);
	hm_reserve_space(test_heap, object);
	CU_ASSERT_EQUAL(hm_size_used(test_heap), 10240);
}

void test_hm_pointer_exists1()
{
    size_t object = 2048;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	void *test_pointer = hm_reserve_space(test_heap, object);
	CU_ASSERT_TRUE(hm_pointer_exists(test_heap, test_pointer) == true);
}


void test_hm_pointer_exists2()
{
    size_t object = 2048;
	int n_chunks = 5;
    size_t head_size = hm_measure_required_space(CHUNK_SIZE*n_chunks);
    char metadata[head_size];
	heap_t *test_heap = (heap_t *) metadata;
    hm_init(test_heap, CHUNK_SIZE*n_chunks, false, 0.5);
	void *test_pointer = "banan rullar snabbtare �n applena";
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
    head_size2 = hm_measure_required_space(2*CHUNK_SIZE);
    CU_ASSERT_TRUE(head_size < head_size2);
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
