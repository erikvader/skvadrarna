#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
//#include <CUnit/Automated.h>
#include "list.h"
#include "../common.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>

void test_list_new(void)
{
	list_t *t = list_new(NULL, NULL, NULL);

	CU_ASSERT_FALSE(t==NULL);
	CU_ASSERT_EQUAL(list_length(t), 0);

	list_delete(t, false);
}

void test_list_append(void)
{
	list_t *t = list_new(NULL, NULL, NULL);

	int x = INT_MAX;
	int y = INT_MIN;

	list_append(t, (elem_t) 1);
	CU_ASSERT_EQUAL(list_length(t), 1);

	list_append(t, (elem_t) 2);
	CU_ASSERT_EQUAL(list_length(t), 2);

	list_append(t, (elem_t) -1);
	CU_ASSERT_EQUAL(list_length(t), 3);

	list_append(t, (elem_t) 0);
	CU_ASSERT_EQUAL(list_length(t), 4);

	list_append(t, (elem_t) x);
	CU_ASSERT_EQUAL(list_length(t), 5);

	list_append(t, (elem_t) y);
	CU_ASSERT_EQUAL(list_length(t), 6);

	list_delete(t, false);
}

void test_list_prepend(void)
{
	list_t *t = list_new(NULL, NULL, NULL);

	list_prepend(t, (elem_t) 1);
	CU_ASSERT_EQUAL(list_length(t), 1);

	list_prepend(t, (elem_t) 2);
	CU_ASSERT_EQUAL(list_length(t), 2);

	list_prepend(t, (elem_t) 3);
	CU_ASSERT_EQUAL(list_length(t), 3);

	list_prepend(t, (elem_t) -1);
	CU_ASSERT_EQUAL(list_length(t), 4);

	list_prepend(t, (elem_t) 0);
	CU_ASSERT_EQUAL(list_length(t), 5);

	list_delete(t, false);
}

void test_list_contains(void)
{
	list_t *t = list_new(NULL, NULL, NULL);

	list_prepend(t, (elem_t) 1);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 1), 0);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 3), -1);

	list_prepend(t, (elem_t) 2);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 2), 0);

	list_append(t, (elem_t) -1);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) -1), 2);

	list_append(t, (elem_t) 0);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 0), 3);

	list_prepend(t, (elem_t) 1);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 1), 0);

	list_prepend(t, (elem_t) 2);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 2), 0);

	list_delete(t, false);
}

void test_list_insert(void)
{
	list_t *t = list_new(NULL, NULL, NULL);


    elem_t ret;
    CU_ASSERT_FALSE(list_get(t, 0, &ret));
    CU_ASSERT_FALSE(list_get(t, -1, &ret));
    CU_ASSERT_FALSE(list_get(t, -2, &ret));
	CU_ASSERT_FALSE(list_get(t, 5, &ret));

	int x = INT_MAX;
	int y = INT_MIN;

	list_insert(t, 0, (elem_t) 0);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 0), 0);

	list_insert(t, x, (elem_t) x);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) x), 1);

	list_insert(t, 0, (elem_t) 0);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 0), 0);

	list_insert(t, 2, (elem_t) 2);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 2), 2);

	list_insert(t, 2000, (elem_t) 3);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 3), 4);

	list_insert(t, -5, (elem_t) 4);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 4), 1);

	list_insert(t, 0, (elem_t) 5);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 5), 0);

	list_insert(t, y, (elem_t) 6);
	CU_ASSERT_EQUAL(list_contains(t,(elem_t) 6), 0);

	CU_ASSERT_TRUE(list_get(t, 0, &ret));
    CU_ASSERT_EQUAL(ret.i, 6);

    CU_ASSERT_TRUE(list_get(t, y, &ret));
    CU_ASSERT_EQUAL(ret.i, 6);

    CU_ASSERT_TRUE(list_get(t, -1, &ret));
    CU_ASSERT_EQUAL(ret.i, 3);

    CU_ASSERT_TRUE(list_get(t, x, &ret));
    CU_ASSERT_EQUAL(ret.i, 3);

    CU_ASSERT_TRUE(list_get(t, 7, &ret));
    CU_ASSERT_EQUAL(ret.i, 3);

    CU_ASSERT_TRUE(list_get(t, 2, &ret));
CU_ASSERT_EQUAL(ret.i, 0);

	list_delete(t, false);
}

void test_list_remove_minmax(void)
{
	list_t *t = list_new(NULL, NULL, NULL);

	int x = INT_MAX;
	int y = INT_MIN;

	list_insert(t, 0, (elem_t) 0);
	list_insert(t, 1, (elem_t) 1);
	list_insert(t, 2, (elem_t) 2);
	list_insert(t, 3, (elem_t) 3);

	list_remove(t, x, false);
	CU_ASSERT_EQUAL(list_contains(t, (elem_t) 3), -1);  

	list_remove(t, y, false);
	CU_ASSERT_EQUAL(list_contains(t, (elem_t) 0), -1); 

	list_delete(t, false);
}

void test_list_remove(void)
{
	list_t *t = list_new(NULL, NULL, NULL);
	const int TEST_VALUES[] = {3,5,1,7,4,8,0,9,11,13,14,15,16,6,17,12,18,19,2,20,10};
	const int LEN = sizeof(TEST_VALUES)/sizeof(int);

	for (int i = LEN -1 ; i >= 0; --i)
	{
	   list_prepend(t,(elem_t) TEST_VALUES[i]);
	}

	for (int i = 0; i<LEN; ++i)
	{
	   list_remove(t, 0, false); 
	   CU_ASSERT_TRUE(list_contains(t, (elem_t) TEST_VALUES[i]) == -1);
	}

	for (int i = LEN-1; i >= 0; --i)
	{
	   list_prepend(t,(elem_t) TEST_VALUES[i]);
	}

	for (int i = LEN-1; i >= 0; --i)
	{
	   list_remove(t, i, false);
	   CU_ASSERT_EQUAL(list_contains(t, (elem_t) TEST_VALUES[i]), -1);
	}

	for (int i = LEN-1; i >= 0; --i)
	{
	   list_prepend(t,(elem_t) TEST_VALUES[i]);
	}

	for (int i = LEN-10; i >= 0; --i)
	{
	   list_remove(t, i, false);
	   CU_ASSERT_EQUAL(list_contains(t, (elem_t) TEST_VALUES[i]), -1);
	}

	list_delete(t, false);
}

void test_list_get(void)
{
	list_t *t = list_new(NULL, NULL, NULL);
	elem_t result;

	int x = INT_MAX;
   	int y = INT_MIN;
	
   	//CU_ASSERT_FALSE(list_get(t, 1, &result)); 
   	
   	list_insert(t, 0, (elem_t) y);
   	CU_ASSERT_TRUE(list_get(t, 0, &result));
   	CU_ASSERT(result.i == y);
   	
	list_insert(t, 1, (elem_t) 0);
	CU_ASSERT_TRUE(list_get(t, 1, &result));
	CU_ASSERT(result.i == 0);
	
	list_insert(t, -1, (elem_t) -1);
	CU_ASSERT_TRUE(list_get(t, -1, &result));
	CU_ASSERT(result.i == -1);
	
	list_insert(t, y, (elem_t) 1);
	CU_ASSERT_TRUE(list_get(t, y, &result));
	CU_ASSERT(result.i == 1);
	
	list_insert(t, x, (elem_t) 5);
	CU_ASSERT_TRUE(list_get(t, x, &result));
	CU_ASSERT(result.i == 5);
	
	list_remove(t, -1, false);

	list_delete(t, false);
}

elem_t list_s_dup(elem_t e){
   return (elem_t) { .p=strdup(e.p)};
}

void list_s_free(elem_t e){
   free(e.p);
}

int list_s_cmp(elem_t a, elem_t b){
   return strcmp(a.p, b.p);
}

void test_list_string(){
   list_t *l = list_new(list_s_dup, list_s_free, list_s_cmp);
   list_append(l, (elem_t)(void*)"ne");
   list_prepend(l, (elem_t)(void*)"hej");
   list_insert(l, 1, (elem_t)(void*)"hej");
   list_insert(l, 1, (elem_t)(void*)"hej");

   CU_ASSERT_TRUE(list_contains(l, (elem_t)(void*)"hej") != -1);
   CU_ASSERT_FALSE(list_contains(l, (elem_t)(void*)"hejhej") != -1);

   list_remove(l, 3, true);
   list_remove(l, 1, true);
   list_remove(l, -1, true);
   list_remove(l, 0, true);

   list_append(l, (elem_t)(void*)"ne");
   list_append(l, (elem_t)(void*)"ne");
   list_append(l, (elem_t)(void*)"ne");

   list_delete(l, true);
}

int main()
{
   CU_initialize_registry();

   CU_pSuite creation = CU_add_suite("Test creation", NULL, NULL);
   CU_add_test(creation, "Creation", test_list_new);
   CU_add_test(creation, "Adding elements to the list", test_list_append);
   CU_add_test(creation, "Adding elements to the list", test_list_prepend);

   CU_pSuite addAndRemove = CU_add_suite("Test adding and removing elements", NULL, NULL);
   CU_add_test(addAndRemove, "Testing index", test_list_contains);
   CU_add_test(addAndRemove, "Inserting elements at a given index", test_list_insert);
   CU_add_test(addAndRemove, "Removing elements at a very big and small index", test_list_remove_minmax);
   CU_add_test(addAndRemove, "Removing elements at a given index", test_list_remove);
   CU_add_test(addAndRemove, "strings", test_list_string);

   CU_pSuite getAndApply = CU_add_suite("Testing applying and getting elements from list", NULL, NULL);
   CU_add_test(getAndApply, "Testing getting an element", test_list_get);

   //CU_automated_run_tests();
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();
}
