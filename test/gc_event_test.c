#include <CUnit/CUnit.h>
#include "include/gc.h"
#include "gc_event_test.h"

typedef struct tree tree_t;
struct tree{
    int id;
    tree_t *next;
};

static
tree_t *new_tree(heap_t *heap, int id){
    tree_t *new = h_alloc_struct(heap, "i*");
    new->id = id;
    new->next = NULL;
    return new;
}

static inline
void point(tree_t *t1, tree_t *t2){
    t1->next = t2;
}

void gc_event_test_the_only_test(){
    heap_t *heap = h_init(5*2048, false, 1);

    // alloc some things in hopefully different chunks
    tree_t *t1  = new_tree(heap, 1);
    tree_t *t2  = new_tree(heap, 2);
    tree_t *t3  = new_tree(heap, 3);
    h_alloc_data(heap, 1024);
    tree_t *t4  = new_tree(heap, 4);
    tree_t *t5  = new_tree(heap, 5);
    tree_t *t6  = new_tree(heap, 6);
    tree_t *t7  = new_tree(heap, 7);
    h_alloc_data(heap, 1024);
    tree_t *t8  = new_tree(heap, 8);
    tree_t *t9  = new_tree(heap, 9);
    tree_t *t10 = new_tree(heap, 10);
    h_alloc_data(heap, 1024);

    // link them
    point(t1, t2);
    point(t2, t4);
    point(t8, t9);
    point(t9, t3);
    point(t7, t9);
    point(t10, t6);

    //remove stack variables
    t2 = t3 = t4 = t5 = t6 = t7 = t9 = t10 = NULL;

    size_t mem_before = h_used(heap);

    //run!
    h_gc(heap);

    size_t mem_after = h_used(heap);

    CU_ASSERT_TRUE(mem_after < mem_before);

    tree_t *temp = t1->next;
    CU_ASSERT_TRUE(t1->id = 1);
    CU_ASSERT_TRUE(temp->id == 2);

    temp = temp->next;
    CU_ASSERT_TRUE(temp->id == 4);

    temp = t8->next;
    CU_ASSERT_TRUE(t8->id == 8);
    CU_ASSERT_TRUE(temp->id == 9);

    temp = temp->next;
    CU_ASSERT_TRUE(temp->id == 3);
    h_delete(heap);
}

void add_gc_event_test_suites() {
    CU_pSuite initSuite = CU_add_suite("gc_event test", NULL, NULL);
    CU_ADD_TEST(initSuite, gc_event_test_the_only_test);

    // CU_pSuite allocSuite = CU_add_suite("Heap metadata allocation", NULL, NULL);
    // CU_ADD_TEST(allocSuite, test_hm_reset_chunk1);
    // CU_ADD_TEST(allocSuite, test_hm_reset_chunk2);
    // CU_ADD_TEST(allocSuite, test_get_free_space);
    // CU_ADD_TEST(allocSuite, test_get_free_space2);
    // CU_ADD_TEST(allocSuite, test_get_free_space3);
    // CU_ADD_TEST(allocSuite, test_get_free_space4);
    // CU_ADD_TEST(allocSuite, test_alloc_spec_chunk1);
    // CU_ADD_TEST(allocSuite, test_alloc_spec_chunk2);

    // CU_pSuite gettersSuite = CU_add_suite("Heap metadata getters and query functions", NULL, NULL);
    // CU_ADD_TEST(gettersSuite, test_hm_get_amount_chunks);
    // CU_ADD_TEST(gettersSuite, test_hm_get_pointer_chunk);
    // CU_ADD_TEST(gettersSuite, test_hm_get_used_chunks);
    // CU_ADD_TEST(gettersSuite, test_hm_over_threshold);
    // CU_ADD_TEST(gettersSuite, test_hm_under_threshold);
    // CU_ADD_TEST(gettersSuite, test_hm_size_available1);
    // CU_ADD_TEST(gettersSuite, test_hm_size_available2);
    // CU_ADD_TEST(gettersSuite, test_hm_size_available3);
    // CU_ADD_TEST(gettersSuite, test_hm_size_used1);
    // CU_ADD_TEST(gettersSuite, test_hm_size_used2);
    // CU_ADD_TEST(gettersSuite, test_hm_size_used3);
    // CU_ADD_TEST(gettersSuite, test_hm_pointer_exists1);
    // CU_ADD_TEST(gettersSuite, test_hm_pointer_exists2);
    // CU_ADD_TEST(gettersSuite, test_hm_pointer_exists3);
}
