#include <CUnit/CUnit.h>
#include "../src/include/gc.h"
#include "gc_test.h"
#include <string.h>


#define CHUNK_SIZE 2048

/*
 * Stupid List Thing
 */
typedef struct node node_t;

struct node {
  int elem;
  node_t *next;
};
const char *node_format_string = "i*";

typedef struct list {
  node_t *head;
  heap_t *heap;
} list_t;
const char *list_format_string = "**";

list_t *list_init(heap_t *heap) {
  list_t *list = h_alloc_struct(heap,"**");
  if (list != NULL) {
    *list = (list_t) { .head = NULL, .heap = heap };
  }
  return list;
}

bool list_prepend(list_t *list, int e) {
  node_t *succ = h_alloc_struct(list->heap, "i*");

  if (succ == NULL) {
    return false;
  }

  *succ = (node_t) { .elem = e , .next = list->head };
  list->head = succ;
  return true;
}

int list_size(list_t *list) {
  int size = 0;
  node_t *curr = list->head;

  while (curr != NULL) {
    curr =  curr->next;
    ++size;
  }

  return size;
}

int list_read(list_t *list, int index) {
  node_t *curr = list->head;

  for (int i = 0; i < index; ++i) {
    if (curr == NULL) {
      return -1;
    }
    curr = curr->next;
  }

  if (curr != NULL) {
    return curr->elem;
  }
  return -1;
}

void list_remove_every_other(list_t *list) {
  node_t *curr = list->head;

  while (curr != NULL) {
    if (curr->next != NULL) {
      curr->next = curr->next->next;
      curr = curr->next;
    }
    else {
      return;
    }
  }
}

/*
 * end of stupid list thing
 */

typedef struct test_struct_1 {
  int i;
  char c;
  void *p;
} test_struct_1_t;
char test_struct_1_format_string[4] = { 'i', 'c','*','\0' };

void gc_test_init_high_address() {
  heap_t *heap = h_init(10 * CHUNK_SIZE, true, 1.0);

  CU_ASSERT_TRUE((uintptr_t) heap >= (0x10000000));

  h_delete(heap);
}

void gc_test_alloc_size() {
  heap_t *heap = h_init(10 * CHUNK_SIZE, true, 1.0);

  size_t before = h_used(heap);
  void *p = h_alloc_data(heap,128);
  size_t after = h_used(heap);
 
  CU_ASSERT_TRUE(before + 128 <= after);

  h_delete(heap);
}

void gc_test_alloc_HELLO_WORLD() {
  heap_t *heap = h_init(10 * CHUNK_SIZE, true, 1.0); 

  const char *hello = "HELLO WORLD";

  char *heap_hello = h_alloc_data(heap,strlen(hello)+1);

  memcpy(heap_hello,hello,strlen(hello)+1);

  CU_ASSERT_TRUE(strcmp(heap_hello,hello) == 0);
  h_delete(heap);
}

void gc_test_alloc_zeroed() {
  heap_t *heap = h_init(10 * CHUNK_SIZE, true, 1.0);

  int *i = h_alloc_data(heap,sizeof(int));

  CU_ASSERT_TRUE(*i == 0);
  h_delete(heap);
}

void gc_test_alloc_struct_1() {
  heap_t *heap = h_init(10 * CHUNK_SIZE, true, 1.0);

  test_struct_1_t to_insert = (test_struct_1_t) { .i = 42 , .c = 'a' , .p = (void *) 0xDEADBEEF };

  test_struct_1_t *test = h_alloc_struct(heap, test_struct_1_format_string);

  *test = to_insert;

  CU_ASSERT_TRUE(test->i == 42 && test->c == 'a' && test->p == (void *) 0xDEADBEEF);
  h_delete(heap);
}


void gc_test_gc_event_1() {
  size_t heap_size = 16*CHUNK_SIZE;

  heap_t *heap = h_init(heap_size, true, 1.0);


  int *p = NULL;
  for (int i = 0; i < 1024; ++i) {
    p = h_alloc_struct(heap,"i");
    *p = i;
  }
  printf("%lu\n",h_used(heap));

  h_gc(heap);

  CU_ASSERT_TRUE(h_used(heap) < heap_size/4);

  h_delete(heap);
}

void gc_test_gc_event_2() {
  size_t heap_size = 16*CHUNK_SIZE;
  int tests = 1024;

  heap_t *heap = h_init(heap_size,true,1.0);

  int *p[tests];

  for (int i = 0; i < tests; ++i) {
    p[i] = h_alloc_struct(heap,"i");
  }

  size_t before = h_used(heap);

  h_gc(heap);
  size_t after = h_used(heap);

  CU_ASSERT_TRUE(before == after);
  h_delete(heap);
}

void gc_test_gc_event_stress() {
  size_t heap_size = 16*CHUNK_SIZE;
  int tests = 1024*16;

  heap_t *heap = h_init(heap_size, true, 0.5);

  int *p = NULL;
  for (int i = 0; i < tests; ++i) {
    p = h_alloc_struct(heap,"i");
  }

  CU_ASSERT_TRUE(h_used(heap) < h_avail(heap));
  h_delete(heap);
}

void gc_test_alloc_list_stress() {
  int fst_loop = 2048;
  int snd_loop = 128;
  
  size_t heap_size = 16*CHUNK_SIZE;
  heap_t *heap = h_init(heap_size,true,0.5);
  list_t *list = list_init(heap);

  int failed = 0;
  for (int i = 0; i < fst_loop; ++i) {
    int siz1 = list_size(list);
    int expected1[siz1 + snd_loop];
    for (int q = 0; q < siz1; ++q) {
      expected1[snd_loop+q] = list_read(list,q);
    }
    for (int q = 0; q < snd_loop; ++q) {
      bool succ = list_prepend(list,snd_loop*i+q);
      expected1[snd_loop-q-1] = snd_loop*i+q;
      if (!succ) {
        ++failed; 
      }
    }
    for (int q = 0; q < siz1 + snd_loop; ++q) {
      if (expected1[q] != list_read(list,q)) {
        ++failed;
      }
    }
    int siz2 = list_size(list);
    int expected2[siz2 /2 + siz2 % 2];
    for (int q = 0; q < siz2; q+=2) {
      expected2[q/2] = list_read(list,q);
    }
    list_remove_every_other(list);
    for (int q = 0; q < siz2 / 2 + siz2 % 2; ++q) {
      if (expected2[q] != list_read(list,q)) {
          ++failed;
      }
    }
  }
  CU_ASSERT_TRUE(failed == 0);
  h_delete(heap);
}

void add_gc_test_suites() {
  
  CU_pSuite gcInit = CU_add_suite("GC init", NULL, NULL);
  CU_ADD_TEST(gcInit, gc_test_init_high_address);

  CU_pSuite gcAlloc = CU_add_suite("GC alloc", NULL, NULL);
  CU_ADD_TEST(gcAlloc, gc_test_alloc_size);
  CU_ADD_TEST(gcAlloc, gc_test_alloc_HELLO_WORLD);
  CU_ADD_TEST(gcAlloc, gc_test_alloc_zeroed);
  

  CU_pSuite gcEvent = CU_add_suite("GC Event", NULL, NULL);
  CU_ADD_TEST(gcEvent, gc_test_gc_event_1);
  CU_ADD_TEST(gcEvent, gc_test_gc_event_2);
  CU_ADD_TEST(gcEvent, gc_test_gc_event_stress);
  CU_ADD_TEST(gcEvent, gc_test_alloc_list_stress);
}
