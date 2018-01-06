#include <stdio.h>
#include "CUnit/Basic.h"
#include "database.h"
#include <stdlib.h>

static database_t *db = NULL;

int init_suite1(){
   if(db != NULL){
      return -1;
   }else{
      db = db_new();
      return 0;
   }
}

int clean_suite1(){
   if(db == NULL){
      return -1;
   }else{
      db_free(db);
      db = NULL;
      return 0;
   }
}

void test_validations(){
   CU_ASSERT_TRUE(db_valid_item_name("chokladpuffar"));
   CU_ASSERT_TRUE(db_valid_item_name("bananstänger i chokladsås!"));
   CU_ASSERT_FALSE(db_valid_item_name(""));

   CU_ASSERT_TRUE(db_valid_item_price(95));
   CU_ASSERT_TRUE(db_valid_item_price(0));
   CU_ASSERT_FALSE(db_valid_item_price(-1));

   CU_ASSERT_TRUE(db_is_valid_shelf_name("A23"));
   CU_ASSERT_TRUE(db_is_valid_shelf_name("b23918723981723"));
   CU_ASSERT_FALSE(db_is_valid_shelf_name("3a"));
   CU_ASSERT_FALSE(db_is_valid_shelf_name("3"));
   CU_ASSERT_FALSE(db_is_valid_shelf_name("a"));
   CU_ASSERT_FALSE(db_is_valid_shelf_name("a23a"));
   CU_ASSERT_FALSE(db_is_valid_shelf_name(""));
   CU_ASSERT_FALSE(db_is_valid_shelf_name("aaa"));
   CU_ASSERT_FALSE(db_is_valid_shelf_name("333"));

}

void test_add(){
   enum db_error err;

   err = db_add_item(db, "smör", "har på mackan", 13);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   err = db_add_item(db, "smör", "har på mackan", 13);
   CU_ASSERT_EQUAL(DB_ITEM_ALREADY_EXISTS, err);

   err = db_add_item(db, "", "har på mackan", 13);
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   err = db_add_item(db, "ost", "har också på mackan", -1);
   CU_ASSERT_EQUAL(DB_INVALID_PRICE, err);

   err = db_add_item(db, "ost", "har också på mackan", 0);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   err = db_add_item(db, "bröd", "kan ha ost och eller smör på", 1337);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   err = db_add_item(db, "bröd", "kan ha ost och eller smör på", 1337);
   CU_ASSERT_EQUAL(DB_ITEM_ALREADY_EXISTS, err);

   err = db_add_item(db, "brödrost", "kan ha mackan i", 1);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   //ska finnas smör, ost, bröd och brödrost i denna nu
   CU_ASSERT_TRUE(db_item_exists(db, "smör") &&
                  db_item_exists(db, "ost") &&
                  db_item_exists(db, "brödrost") &&
                  db_item_exists(db, "bröd") &&
                  !db_item_exists(db, "karamaller"));
}

void test_setting(){
   enum db_error err;

   //name
   err = db_set_item_name(db, "smör", "Smör");
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);
   CU_ASSERT_TRUE(db_item_exists(db, "Smör"));
   CU_ASSERT_FALSE(db_item_exists(db, "smör"));

   err = db_set_item_name(db, "smör", "SSSSSSmör");
   CU_ASSERT_EQUAL(DB_ITEM_NO_EXIST, err);

   err = db_set_item_name(db, "Smör", "");
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   //description
   err = db_set_item_desc(db, "", "går int");
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   char *new_desc = "går ej att rosta";
   err = db_set_item_desc(db, "bröd", new_desc);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   const char *retrieved;
   err = db_get_item_desc(db, "", &retrieved);
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   err = db_get_item_desc(db, "superintelligent brödrost med AC", &retrieved);
   CU_ASSERT_EQUAL(DB_ITEM_NO_EXIST, err);

   err = db_get_item_desc(db, "bröd", &retrieved);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   CU_ASSERT_TRUE(strcmp(new_desc, retrieved) == 0);

   //price
   err = db_set_item_price(db, "", 23);
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   int new_price = 94875;
   err = db_set_item_price(db, "bröd", new_price);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   err = db_set_item_price(db, "bröd", -1);
   CU_ASSERT_EQUAL(DB_INVALID_PRICE, err);

   int getted;
   err = db_get_item_price(db, "", &getted);
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   err = db_get_item_price(db, "superintelligent brödrost med AC", &getted);
   CU_ASSERT_EQUAL(DB_ITEM_NO_EXIST, err);

   err = db_get_item_price(db, "bröd", &getted);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   CU_ASSERT_TRUE(new_price == getted);

   //all
   err = db_item_get_all(db, "", &retrieved, &getted);
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   err = db_item_get_all(db, "superintelligent brödrost med AC", &retrieved, &getted);
   CU_ASSERT_EQUAL(DB_ITEM_NO_EXIST, err);

   err = db_item_get_all(db, "bröd", &retrieved, &getted);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   CU_ASSERT_TRUE(new_price == getted);
   CU_ASSERT_TRUE(strcmp(new_desc, retrieved) == 0);
}

void test_remove(){
   enum db_error err;

   err = db_remove_item(db, "");
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   err = db_remove_item(db, "ultravadderad BH");
   CU_ASSERT_EQUAL(DB_ITEM_NO_EXIST, err);

   err = db_remove_item(db, "ost");
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);
   CU_ASSERT_FALSE(db_item_exists(db, "ost"));
}

void test_put_shelf(){
   enum db_error err;

   err = db_put_item_on_shelf(db, "", "A25", 22);
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   err = db_put_item_on_shelf(db, "vibrator", "A25", 22);
   CU_ASSERT_EQUAL(DB_ITEM_NO_EXIST, err);

   err = db_put_item_on_shelf(db, "Smör", "25", 22);
   CU_ASSERT_EQUAL(DB_INVALID_SHELF_NAME, err);

   err = db_put_item_on_shelf(db, "Smör", "A25", -100);
   CU_ASSERT_EQUAL(DB_INVALID_AMOUNT, err);

   err = db_put_item_on_shelf(db, "Smör", "A25", 22);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   err = db_put_item_on_shelf(db, "Smör", "A26", 22);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   err = db_put_item_on_shelf(db, "Smör", "A25", 22);
   CU_ASSERT_EQUAL(DB_SHELF_ALREADY_EXISTS, err);

   err = db_put_item_on_shelf(db, "bröd", "A25", 10);
   CU_ASSERT_EQUAL(DB_SHELF_BUSY, err);

   err = db_put_item_on_shelf(db, "bröd", "B25", 10);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);
}

void test_shelf_remove(){
   enum db_error err;

   err = db_remove_item_from_shelf(db, "A25", 4);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   err = db_remove_item_from_shelf(db, "B25", 11);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   err = db_remove_item_from_shelf(db, "B25", 11);
   CU_ASSERT_EQUAL(DB_SHELF_NO_EXIST, err);

   err = db_remove_item_from_shelf(db, "A25", -11);
   CU_ASSERT_EQUAL(DB_INVALID_AMOUNT, err);

   err = db_remove_item_from_shelf(db, "", 11);
   CU_ASSERT_EQUAL(DB_INVALID_SHELF_NAME, err);
}

void test_shelf_state(){
   enum db_error err;

   const char *owner;
   int owner_amount;
   err = db_shelf_is_in_use(db, "A25", &owner, &owner_amount);
   CU_ASSERT_EQUAL(DB_SHELF_BUSY, err);
   CU_ASSERT_EQUAL(owner_amount, 40);
   CU_ASSERT_TRUE(strcmp("Smör", owner) == 0);

   err = db_shelf_is_in_use(db, "E45", &owner, &owner_amount);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);
   CU_ASSERT_EQUAL(owner_amount, 40);
   CU_ASSERT_TRUE(strcmp("Smör", owner) == 0);

   err = db_shelf_is_in_use(db, "kjh345", &owner, &owner_amount);
   CU_ASSERT_EQUAL(DB_INVALID_SHELF_NAME, err);
}

void test_iterate_shelf(){
   enum db_error err;

   elem_t *shelves;
   int num;
   err = db_get_shelves(db, "", &shelves, &num);
   CU_ASSERT_EQUAL(DB_INVALID_ITEM_NAME, err);

   err = db_get_shelves(db, "smörgostårta", &shelves, &num);
   CU_ASSERT_EQUAL(DB_ITEM_NO_EXIST, err);

   err = db_get_shelves(db, "Smör", &shelves, &num);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);

   CU_ASSERT_EQUAL(num, 2);

   CU_ASSERT_TRUE(db_shelf_get_amount(shelves[0]) == 40);
   CU_ASSERT_TRUE(strcmp(db_shelf_get_name(shelves[0]), "A25") == 0);

   CU_ASSERT_TRUE(db_shelf_get_amount(shelves[1]) == 22);
   CU_ASSERT_TRUE(strcmp(db_shelf_get_name(shelves[1]), "A26") == 0);

   free(shelves);

   err = db_get_shelves(db, "brödrost", &shelves, &num);
   CU_ASSERT_EQUAL(DB_NO_ERROR, err);
   CU_ASSERT_EQUAL(num, 0);
}

void test_file(){
   enum db_error err;

   err = db_save(db, "databas");
   if(err == DB_FILE_ERROR){
      CU_ASSERT_TRUE(false);
      return;
   }

   db_free(db);
   err = db_load(&db, "databas");
   if(err == DB_FILE_ERROR){
      CU_ASSERT_TRUE(false);
      return;
   }

   CU_ASSERT_TRUE(db_item_exists(db, "Smör") &&
                  db_item_exists(db, "brödrost") &&
                  db_item_exists(db, "bröd") &&
                  !db_item_exists(db, "karamaller"));
}

void test_add_undo(){
   enum db_error err;
   char *name;

   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(err, DB_NOTHING_TO_UNDO);

   db_add_item(db, "ost", "ostar", 22);
   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(err, DB_UNDO_ADD);
   CU_ASSERT_TRUE(strcmp(name, "ost") == 0);
   CU_ASSERT_FALSE(db_item_exists(db, "ost"));
   free(name);

   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(err, DB_NOTHING_TO_UNDO);

   db_add_item(db, "ost", "ostar", 22);
   db_add_item(db, "ostost", "ostar", 22);

   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(err, DB_UNDO_ADD);
   CU_ASSERT_TRUE(strcmp(name, "ostost") == 0);
   CU_ASSERT_FALSE(db_item_exists(db, "ostost"));
   free(name);

   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(err, DB_UNDO_ADD);
   CU_ASSERT_TRUE(strcmp(name, "ost") == 0);
   CU_ASSERT_FALSE(db_item_exists(db, "ost"));
   free(name);

   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(err, DB_NOTHING_TO_UNDO);

   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(err, DB_NOTHING_TO_UNDO);
}

void test_undo_remove(){
   enum db_error err;
   char *name;

   db_add_item(db, "ost", "ostar", 22);
   db_add_item(db, "ostost", "ostar", 22);

   db_remove_item(db, "ost");
   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(DB_UNDO_REMOVE, err);
   CU_ASSERT_TRUE(strcmp(name, "ost") == 0);
   CU_ASSERT_TRUE(db_item_exists(db, "ost"));
   free(name);
}

void test_undo_edit(){
   enum db_error err;
   char *name;

   db_set_item_name(db, "ost", "OOOSSSTTT");

   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(DB_UNDO_EDIT, err);
   CU_ASSERT_TRUE(strcmp(name, "OOOSSSTTT") == 0);
   CU_ASSERT_TRUE(db_item_exists(db, "ost"));
   free(name);

   db_set_item_desc(db, "ost", "ojojoj");
   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(DB_UNDO_EDIT, err);
   CU_ASSERT_TRUE(strcmp(name, "ost") == 0);
   const char *desc;
   db_get_item_desc(db, "ost", &desc);
   CU_ASSERT_TRUE(strcmp(desc, "ostar") == 0);
   free(name);

   db_set_item_price(db, "ost", 123);
   err = db_undo(db, &name);
   CU_ASSERT_EQUAL(DB_UNDO_EDIT, err);
   CU_ASSERT_TRUE(strcmp(name, "ost") == 0);
   int p;
   db_get_item_price(db, "ost", &p);
   CU_ASSERT_TRUE(p == 22);
   free(name);
}

int main(){
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   pSuite = CU_add_suite("Suite_0", NULL, NULL);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite, "test validation functions", test_validations)){
         CU_cleanup_registry();
         return CU_get_error();
      }



   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test of adding",            test_add)) ||
       (NULL == CU_add_test(pSuite, "test of setting",           test_setting)) ||
       (NULL == CU_add_test(pSuite, "test of removing",          test_remove)) ||
       (NULL == CU_add_test(pSuite, "test adding shelves",       test_put_shelf)) ||
       (NULL == CU_add_test(pSuite, "test of removing shelves",  test_shelf_remove)) ||
       (NULL == CU_add_test(pSuite, "test of shelf state",       test_shelf_state)) ||
       (NULL == CU_add_test(pSuite, "test of iterating shelves", test_iterate_shelf)))
      {
         CU_cleanup_registry();
         return CU_get_error();
      }




   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_2", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test of adding",                   test_add)) ||
       (NULL == CU_add_test(pSuite, "test of setting",                  test_setting)) ||
       (NULL == CU_add_test(pSuite, "test of removing",                 test_remove)) ||
       (NULL == CU_add_test(pSuite, "test adding shelves",              test_put_shelf)) ||
       (NULL == CU_add_test(pSuite, "test of removing shelves",         test_shelf_remove)) ||
       (NULL == CU_add_test(pSuite, "test writing and reading to file", test_file)) ||
       (NULL == CU_add_test(pSuite, "test of shelf state",              test_shelf_state)) ||
       (NULL == CU_add_test(pSuite, "test of iterating shelves",        test_iterate_shelf)))
      {
         CU_cleanup_registry();
         return CU_get_error();
      }

   pSuite = CU_add_suite("Suite_3", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test undoing add",                 test_add_undo)) ||
       (NULL == CU_add_test(pSuite, "test undoing remove", test_undo_remove)) ||
       (NULL == CU_add_test(pSuite, "test undoing editing", test_undo_edit)))
      {
         CU_cleanup_registry();
         return CU_get_error();
      }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
