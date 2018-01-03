#include <CUnit/CUnit.h>
#include "../src/include/stack_iter.h"
#include "si_test.h"

extern char **environ;

bool si_true_is_in_heap_fun(heap_t *heap,void *p) {
  return true;
}

bool si_false_is_in_heap_fun(heap_t *heap, void *p) {
  return false;
}

bool si_DEADBEEF_is_in_heap_fun(heap_t *heap,void *p) {
  unsigned long l = (unsigned long) p;
  if ( l == 0xDEADBEEF) {
    return true;
  }
  else {
    return false;
  }
}


void si_test_null() {
  void **out = si_next_pointer_dbg(NULL, NULL, si_true_is_in_heap_fun, sizeof(void *));
  CU_ASSERT_TRUE( out == NULL);
}


void si_test_environ() {
  void **out = si_next_pointer_dbg(NULL, (void **) environ, si_true_is_in_heap_fun, sizeof(void *));
  CU_ASSERT_TRUE( out == NULL);
  
}

void si_test_align_neg() {
  unsigned long alignment = 8;

  unsigned long start = ((unsigned long) environ) - 128 - 3;
  
  void **out = si_next_pointer_dbg(NULL, (void **) start ,si_true_is_in_heap_fun,alignment);


  CU_ASSERT_TRUE( ((unsigned long) out) % alignment == 0);
  CU_ASSERT_TRUE( ((unsigned long) out) == (start + 3));
}

void si_test_align_pos() {
  unsigned long alignment = 8;

  unsigned long start = ((unsigned long) environ) + 128 + 3;
  
  void **out  = si_next_pointer_dbg(NULL, (void **) start,si_true_is_in_heap_fun,alignment);

  CU_ASSERT_TRUE( ((unsigned long) out) % alignment == 0);
  CU_ASSERT_TRUE( (unsigned long) out == start - 3);
}

void si_test_step_once() {
  unsigned long alignment = 8;

  unsigned long start = (unsigned long) environ + (8 * alignment);
  
  void ** out = si_next_pointer_dbg(NULL,(void **) start, si_true_is_in_heap_fun,alignment);

  CU_ASSERT_TRUE(((unsigned long) out) == start - alignment);
}

void si_test_step_to_environ() {
  unsigned long alignment = 8;

  unsigned long start = (unsigned long) environ + (8 * alignment);
  
  void ** out = si_next_pointer_dbg(NULL,(void **) start, si_false_is_in_heap_fun,alignment);

  CU_ASSERT_TRUE(out == NULL);
}

void **si_test_step_to_DEADBEEF_aux(int n,size_t alignment) {
  void *p = NULL;
  if (n == 0) {
    return si_next_pointer_dbg(NULL,&p,si_DEADBEEF_is_in_heap_fun,alignment);
  }
  else {
    return si_test_step_to_DEADBEEF_aux(n-1,alignment);
  }
}

void si_test_step_to_DEADBEEF() {
  unsigned long alignment = 8;

  void *to_find = (void *) 0xDEADBEEF;

  void ** out = si_test_step_to_DEADBEEF_aux(10,alignment);

  CU_ASSERT_TRUE(out == &to_find);

}

void add_si_test_suites() {
  
  CU_pSuite binpSuite = CU_add_suite("Stack Iteration Bad Inputs", NULL, NULL);
  CU_ADD_TEST(binpSuite, si_test_null);
  CU_ADD_TEST(binpSuite, si_test_environ);
  
  CU_pSuite alignSuite = CU_add_suite("Stack Iteration Alignment",NULL,NULL);
  CU_ADD_TEST(alignSuite, si_test_align_neg);
  CU_ADD_TEST(alignSuite, si_test_align_pos);
  
  CU_pSuite stepSuite = CU_add_suite("Stack Iteration Stepping",NULL,NULL);
  CU_ADD_TEST(stepSuite, si_test_step_once);
  CU_ADD_TEST(stepSuite, si_test_step_to_environ);
  CU_ADD_TEST(stepSuite, si_test_step_to_DEADBEEF);
  
}
