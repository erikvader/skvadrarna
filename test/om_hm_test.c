#include <CUnit/CUnit.h>
#include "include/heap_metadata.h"
#include "hm_test.h"
#include "include/object_metadata.h"

#define CHUNK_SIZE 2048
#define ALIGNMENT 16


void test_omhm_overwrite()
{
	heap_t *heap = malloc(100000);
	int n_chunks = 10;
	hm_init(heap, 100000, true, 1);
	long *toAlloc = hm_get_free_space(heap, sizeof(toAlloc));
	om_build_pointerless(heap, sizeof(long));
	*toAlloc = 421/3;
	CU_ASSERT_EQUAL(*toAlloc, (421/3));
	long *toAlloc2 = hm_get_free_space(heap, sizeof(toAlloc));
	om_build_pointerless(heap, sizeof(long));
	*toAlloc2 = 427/3;
	CU_ASSERT_EQUAL(*toAlloc, (421/3));
	om_set_forwarding(toAlloc2, toAlloc);
	CU_ASSERT_EQUAL(*om_get_forwarding(toAlloc2), *toAlloc);
	free(heap);
}



