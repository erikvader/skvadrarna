#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>
#include "tree.h"
#include "../common.h"
#include <limits.h>
#include <stdlib.h>

typedef struct tree tree_t;

void test_tree_new(void)
{
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);

   CU_ASSERT_FALSE(t==NULL);
   CU_ASSERT_EQUAL(tree_size(t), 0);

   tree_delete(t, false, false);
}

void test_tree_insert(void)
{
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);

   int x = INT_MAX;
   int y = INT_MIN;

   CU_ASSERT_EQUAL(tree_depth(t), 0);

   CU_ASSERT_TRUE(tree_insert(t,(tree_key_t) x,(elem_t) x)); //måste testa returnvärdena från funktionerna
   CU_ASSERT_EQUAL(tree_depth(t), 1); //lite onödig, går inte att predicta (tror jag) om man inte sätter in samma saker så att man vet vad höjden hela tiden är.
   CU_ASSERT_EQUAL(tree_size(t), 1); //bättre att testa

   tree_insert(t,(tree_key_t) y,(elem_t) y);
   CU_ASSERT_EQUAL(tree_depth(t), 2);

   CU_ASSERT_FALSE(tree_insert(t,(tree_key_t) y,(elem_t) y));
   CU_ASSERT_EQUAL(tree_depth(t), 2);

   tree_insert(t,(tree_key_t) -1,(elem_t) -1);
   CU_ASSERT_EQUAL(tree_depth(t), 2);

   tree_insert(t,(tree_key_t) 0,(elem_t) 0);
   CU_ASSERT_EQUAL(tree_depth(t), 3);

   tree_insert(t,(tree_key_t) 12,(elem_t) 12);
   CU_ASSERT_EQUAL(tree_depth(t), 3);

   tree_insert(t,(tree_key_t) 11,(elem_t) 11);
   CU_ASSERT_EQUAL(tree_depth(t), 3);

   tree_insert(t,(tree_key_t) 9,(elem_t) 9);
   CU_ASSERT_EQUAL(tree_depth(t), 4);

   tree_insert(t,(tree_key_t) (x+1),(elem_t) (x+1));
   CU_ASSERT_EQUAL(tree_depth(t), 4);

   CU_ASSERT_EQUAL(tree_size(t), 7);

   tree_delete(t, false, false);
}

void test_tree_remove(void)
{
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);

   const int TEST_VALUES[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
   const int LEN = sizeof(TEST_VALUES)/sizeof(int);
   for (int i = LEN -1 ; i >= 0; --i)
   {
      tree_insert(t,(tree_key_t) TEST_VALUES[i],(elem_t) TEST_VALUES[LEN-1 - i]);
   }

   for (int i = 0; i<sizeof(TEST_VALUES)/sizeof(int); ++i)
   {
      elem_t ta_bort[2];
      tree_remove(t, (tree_key_t) i, ta_bort);
      CU_ASSERT_TRUE(ta_bort[0].i == TEST_VALUES[i]); 
      CU_ASSERT_TRUE(ta_bort[1].i == TEST_VALUES[LEN-1 - i]); //för att verkligen se att key och value kommer ut korrekt
   }

   tree_delete(t, false, false);
}

void test_tree_has_key(void)
{
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);

   int x = INT_MAX;

   CU_ASSERT_FALSE(tree_has_key(t, (tree_key_t) 1));
   
   tree_insert(t, (tree_key_t) 1, (elem_t) 1); 
   CU_ASSERT_TRUE(tree_has_key(t, (tree_key_t) 1));
   
   tree_insert(t, (tree_key_t) 0, (elem_t) 0);
   CU_ASSERT_TRUE(tree_has_key(t, (tree_key_t) 0));
   
   tree_insert(t, (tree_key_t) -1, (elem_t) -1);
   CU_ASSERT_TRUE(tree_has_key(t, (tree_key_t) -1));
   
   tree_insert(t, (tree_key_t) x, (elem_t) x);
   CU_ASSERT_TRUE(tree_has_key(t, (tree_key_t) x));
   
   elem_t ta_bort[2];
   tree_remove(t, (tree_key_t) x, ta_bort);
   CU_ASSERT_FALSE(tree_has_key(t, (tree_key_t) x));

   tree_delete(t, false, false);
}

void test_tree_get(void)
{
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);
   elem_t result;

   int y = INT_MIN;

   CU_ASSERT_FALSE(tree_get(t, (tree_key_t) 1, &result));
   
   tree_insert(t, (tree_key_t) y, (elem_t) y);
   CU_ASSERT_TRUE(tree_get(t, (tree_key_t) y, &result));
   
   tree_insert(t, (tree_key_t) 0, (elem_t) 0);
   CU_ASSERT_TRUE(tree_get(t, (tree_key_t) 0, &result));
   
   tree_insert(t, (tree_key_t) -1, (elem_t) -1);
   CU_ASSERT_TRUE(tree_get(t, (tree_key_t) -1, &result));
   
   tree_insert(t, (tree_key_t) 1000, (elem_t) 1000);
   CU_ASSERT_TRUE(tree_get(t, (tree_key_t) 1000, &result));
   
   elem_t ta_bort[2];
   tree_remove(t, (tree_key_t) 1000, ta_bort);
   CU_ASSERT_FALSE(tree_get(t, (tree_key_t) 1000, &result));

   tree_delete(t, false, false);
}

void test_tree_keys(void)
{
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);
   const int TEST_VALUES[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

   for (int i = sizeof(TEST_VALUES)/sizeof(int) -1 ; i >= 0; --i)
   {
      tree_insert(t,(tree_key_t) TEST_VALUES[i],(elem_t) TEST_VALUES[i]);
   }

   tree_key_t *lista = tree_keys(t);
   for (int i = 0; i<sizeof(TEST_VALUES)/sizeof(int); ++i)
   {
      CU_ASSERT_TRUE(lista[i].i == TEST_VALUES[i]);
   }

   free(lista);
   tree_delete(t, false, false);
}

void test_tree_elements(void)
{
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);
   const int TEST_VALUES[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

   for (int i = sizeof(TEST_VALUES)/sizeof(int) -1 ; i >= 0; --i)
   {
      tree_insert(t,(tree_key_t) TEST_VALUES[i],(elem_t) TEST_VALUES[i]);
   }

   elem_t *elemen = tree_elements(t);
   for (int i = 0; i<sizeof(TEST_VALUES)/sizeof(int); ++i)
   {
      CU_ASSERT_TRUE(elemen[i].i == TEST_VALUES[i]);
   }
   free(elemen);
   tree_delete(t, false, false);
}

void test_tree_check_all(void)
{
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);
   CU_ASSERT_TRUE(tree_check_all(t));

   const int TEST_VALUES[] = {18,9,19,12,20,16,3,13,14,15,11,17,10,0,4,2,6,7,1,8,5}; //så att det är lite random (hittade en jättestor bugg i delete för detta :) )
   const int LEN = sizeof(TEST_VALUES)/sizeof(int);

   CU_ASSERT_TRUE(tree_check_all(t));

   for (int i = LEN -1 ; i >= 0; --i)
   {
      tree_insert(t,(tree_key_t) TEST_VALUES[i],(elem_t) TEST_VALUES[i]);
      CU_ASSERT_TRUE(tree_check_all(t));
   }

   for (int i = 0; i<LEN; i+=2) //testa ta bort lite random
   {
      elem_t ta_bort[2];
      tree_remove(t, (tree_key_t) i, ta_bort);
      CU_ASSERT_TRUE(tree_check_all(t));
   }

   for (int i = LEN -1 ; i >= 0; --i) //testa lägga till saker igen efter att vi har tagit bort.
   {
      tree_insert(t,(tree_key_t) TEST_VALUES[i],(elem_t) TEST_VALUES[i]);
      CU_ASSERT_TRUE(tree_check_all(t));
   }

   tree_delete(t, false, false);
}



int main()
{
   CU_initialize_registry();

   CU_pSuite creation = CU_add_suite("Test creation", NULL, NULL);
   CU_add_test(creation, "Creation", test_tree_new);
   CU_add_test(creation, "Height", test_tree_insert);

   CU_pSuite addAndRemove = CU_add_suite("Test adding and removing elements", NULL, NULL);
   CU_add_test(addAndRemove, "Add and remove", test_tree_remove);
   CU_add_test(addAndRemove, "Check if key is in tree", test_tree_has_key);
   CU_add_test(addAndRemove, "Check if key is in tree", test_tree_get);

   CU_pSuite list = CU_add_suite("Test listing the whole tree", NULL, NULL);
   CU_add_test(list, "Listing tree", test_tree_keys);
   CU_add_test(list, "Listing elements", test_tree_elements);

   CU_pSuite internalIntegrity = CU_add_suite("Test internal integrity", NULL, NULL);
   CU_add_test(internalIntegrity, "Internal integrity", test_tree_check_all);

   //CU_automated_run_tests();
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();
}
