#include "../tree/tree.h"
#include "../list/list.h"
#include "../list/list_iterator.h"
#include "db_file_save.h"
#include "db_types.h"
#include "database.h"
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "db_undo.h"

struct database{
   tree_t *tree;
   undo_stack_t *undo;
   bool undo_enabled;
};

int tree_key_compare(elem_t a, elem_t b){
   return strcmp(a.p, b.p);
}

database_t* db_new(){
   database_t *new = malloc(sizeof(database_t));
   new->tree = tree_new(NULL, NULL, delete_item, NULL, tree_key_compare);
   new->undo = undo_stack_new();
   new->undo_enabled = true;
   return new;
}

void db_free(database_t *db){
   tree_delete(db->tree, false, true);
   undo_stack_delete(db->undo);
   free(db);
}

bool get_item(const database_t *db, const char *item_name, item_t **item){
   //cache??
   elem_t ele = {.p = NULL};
   bool success = tree_get(db->tree, key_char_to_elem_t(item_name), &ele);
   *item = ele.p;
   return success;
}

bool db_item_exists(const database_t *db, const char *item){
   return tree_has_key(db->tree, key_char_to_elem_t(item));
}

enum db_error db_add_item(database_t *db, const char *name, const char *description, const int price){
   if(!db_valid_item_name(name)){
      return DB_INVALID_ITEM_NAME;
   }
   if(!db_valid_item_price(price)){
      return DB_INVALID_PRICE;
   }
   if(db_item_exists(db, name)){
      return DB_ITEM_ALREADY_EXISTS;
   }

   item_t *new = make_item(strdup(name), strdup(description), price);
   tree_insert(db->tree, key_char_to_elem_t(new->name), item_to_elem_t(new));

   if(db->undo_enabled) undo_stack_add(db->undo, new);

   return DB_NO_ERROR;
}

enum db_error db_get_item_desc(const database_t *db, const char *item, const char **desc){
   return db_item_get_all(db, item, desc, NULL);
}

enum db_error db_get_item_price(const database_t *db, const char *item, int *price){
   return db_item_get_all(db, item, NULL, price);
}

enum db_error db_item_get_all(const database_t *db, const char *item, const char ** description, int *price){
   if(!db_valid_item_name(item)){
      return DB_INVALID_ITEM_NAME;
   }
   item_t *ite;
   if(!get_item(db, item, &ite)){
      return DB_ITEM_NO_EXIST;
   }
   if(price != NULL) *price = ite->price;
   if(description != NULL) *description = ite->desc;
   return DB_NO_ERROR;
}

enum db_error db_set_item_desc(database_t *db, const char *item, const char *new_desc){
   if(!db_valid_item_name(item)){
      return DB_INVALID_ITEM_NAME;
   }
   item_t *ite;
   if(!get_item(db, item, &ite)){
      return DB_ITEM_NO_EXIST;
   }
   if(db->undo_enabled) undo_stack_edit(db->undo, ite, ite->name);
   free(ite->desc);
   ite->desc = strdup(new_desc);
   return DB_NO_ERROR;
}

enum db_error db_set_item_price(database_t *db, const char *item, const int new_price){
   if(!db_valid_item_name(item)){
      return DB_INVALID_ITEM_NAME;
   }
   if(!db_valid_item_price(new_price)){
      return DB_INVALID_PRICE;
}
   item_t *ite;
   if(!get_item(db, item, &ite)){
      return DB_ITEM_NO_EXIST;
   }
   if(db->undo_enabled) undo_stack_edit(db->undo, ite, ite->name);
   ite->price = new_price;
   return DB_NO_ERROR;
}

enum db_error db_set_item_name(database_t *db, const char *item, const char *new_name){
   if(!db_valid_item_name(new_name)){
      return DB_INVALID_ITEM_NAME;
   }
   item_t *ite;
   if(!get_item(db, item, &ite)){
      return DB_ITEM_NO_EXIST;
   }

   elem_t info[2];
   tree_remove(db->tree, key_char_to_elem_t(item), info);
   item_t *removed = info[1].p;

   if(db->undo_enabled) undo_stack_edit(db->undo, info[1].p, (char*) new_name);
   free(removed->name);
   removed->name = strdup(new_name);

   tree_insert(db->tree, key_char_to_elem_t(removed->name), item_to_elem_t(removed));

   return DB_NO_ERROR;
}

bool db_valid_item_price(const int price){
   return price >= 0;
}

bool db_valid_item_name(const char *name){
   return strlen(name) > 0;
}

enum db_error db_remove_item(database_t *db, const char* item){
   if(!db_valid_item_name(item)){
      return DB_INVALID_ITEM_NAME;
   }
   elem_t info[2];
   bool success = tree_remove(db->tree, key_char_to_elem_t(item), info);
   if(!success) return DB_ITEM_NO_EXIST;

   if(db->undo_enabled){
      undo_stack_remove(db->undo, info[1].p);
   }else{
      delete_item(info[1]);
   }
   return DB_NO_ERROR;
}

const char** db_get_all_items(const database_t *db, int *size){
   *size = tree_size(db->tree);
   if(*size == 0){
      return NULL;
   }
   return (const char**) tree_keys(db->tree); //kanske funkar
}

bool db_is_valid_shelf_name(const char *name){
   assert(name);
   int i = 0;
   bool has_c = false, has_num = false;
   while(name[i] != '\0'){
      if(isalpha(name[i])){
         if(i == 0){
            has_c = true;
         }else{
            return false;
         }
      }else if(isdigit(name[i])){
         has_num = true;
      }else{
         return false;
      }
      i++;
   }

   return has_c && has_num;
}

bool get_shelf(const database_t *db, const char *shelf_name, item_t **item, shelf_t **shelf){
   elem_t *elements = tree_elements(db->tree);
   int size = tree_size(db->tree);
   bool success = false;
   for(int i = 0; i < size; i++){
      item_t *cur = elements[i].p;
      list_iterator_t *ite = list_get_iterator(cur->shelves);
      while(list_iterator_has_next(ite)){
         shelf_t *she = (*list_iterator_next(ite)).p;
         if(strcmp(she->name, shelf_name) == 0){
            if(item != NULL) *item = cur;
            if(shelf != NULL) *shelf = she;
            i = size; //to end the outer loop
            success = true;
            break;
         }
      }
      free(ite);
   }
   free(elements);
   return success;
}

enum db_error db_put_item_on_shelf(database_t *db, const char *item, const char *shelf_name, const int amount){
   if(!db_is_valid_shelf_name(shelf_name)){
      return DB_INVALID_SHELF_NAME;
   }
   if(!db_valid_item_name(item)){
      return DB_INVALID_ITEM_NAME;
   }
   if(amount < 0){
      return DB_INVALID_AMOUNT;
   }

   item_t *user;
   shelf_t *shelf;
   bool shelf_in_use = get_shelf(db, shelf_name, &user, &shelf);

   if(shelf_in_use){
      bool same_item = strcmp(user->name, item) == 0;
      if(!same_item){
         return DB_SHELF_BUSY;
      }else{
         if(db->undo_enabled) undo_stack_edit(db->undo, user, user->name);
         shelf->num += amount;
         return DB_SHELF_ALREADY_EXISTS;
      }
   }else{
      bool exists = get_item(db, item, &user);
      if(!exists){
         return DB_ITEM_NO_EXIST;
      }else{
         if(db->undo_enabled) undo_stack_edit(db->undo, user, user->name);
         elem_t new = (elem_t) (void*) make_shelf(strdup(shelf_name), amount);
         list_append(user->shelves, new);
         return DB_NO_ERROR;
      }
   }
}

enum db_error db_remove_item_from_shelf(database_t *db, const char *shelf, const int amount){
   if(amount < 0){
      return DB_INVALID_AMOUNT;
   }
   if(!db_is_valid_shelf_name(shelf)){
      return DB_INVALID_SHELF_NAME;
   }

   item_t *item;
   shelf_t *shelf_point;
   bool shelf_in_use = get_shelf(db, shelf, &item, &shelf_point);

   if(!shelf_in_use){
      return DB_SHELF_NO_EXIST;
   }

   if(db->undo_enabled) undo_stack_edit(db->undo, item, item->name);

   shelf_point->num -= amount;

   if(shelf_point->num <= 0){
      int i = 0; //superfult sätt att göra detta på
      list_iterator_t *ite = list_get_iterator(item->shelves);
      while(list_iterator_has_next(ite)){
         if((*list_iterator_next(ite)).p == shelf_point){ //borde vara samma
            break;
         }
         i++;
      }
      free(ite);
      list_remove(item->shelves, i, true); //borde ha hittat grejen
   }

   return DB_NO_ERROR;
}

enum db_error db_shelf_is_in_use(const database_t *db, const char *shelf_name, char const **owner, int *owner_amount){
   if(!db_is_valid_shelf_name(shelf_name)){
      return DB_INVALID_SHELF_NAME;
   }
   item_t *item;
   shelf_t *shelf;
   bool success = get_shelf(db, shelf_name, &item, &shelf);
   if(!success) return DB_NO_ERROR;
   if(owner != NULL) *owner = item->name;
   if(owner_amount != NULL) *owner_amount = shelf->num;
   return DB_SHELF_BUSY;
}

enum db_error db_get_shelves(const database_t *db, const char *item, elem_t **shelves, int *amount){
   if(!db_valid_item_name(item)){
      return DB_INVALID_ITEM_NAME;
   }
   item_t *ite;
   if(!get_item(db, item, &ite)){
      return DB_ITEM_NO_EXIST;
   }

   *amount = list_length(ite->shelves);
   if(*amount == 0) return DB_NO_ERROR;

   *shelves = malloc(sizeof(elem_t) * (*amount));

   list_iterator_t *iterator = list_get_iterator(ite->shelves);
   int i = 0;
   while(list_iterator_has_next(iterator)){
      (*shelves)[i] = *list_iterator_next(iterator);
      i++;
   }
   free(iterator);

   return DB_NO_ERROR;
}

int db_shelf_get_amount(const elem_t shelf){
   return ((shelf_t*) shelf.p)->num;
}

const char* db_shelf_get_name(const elem_t shelf){
   return ((shelf_t*) shelf.p)->name;
}

//////////////////////////////////// files ////////////////////////////////////

enum db_error db_save(const database_t *db, const char *filename){
   const item_t **items = (const item_t**) tree_elements(db->tree);
   bool success = db_file_save(items, tree_size(db->tree), filename);
   free(items);
   if(!success) return DB_FILE_ERROR;
   return DB_NO_ERROR;
}

enum db_error db_load(database_t **db, const char *filename){
   item_t **items;
   int items_len;
   bool success = db_file_get(&items, &items_len, filename);
   if(success){
      *db = db_new();
      for(int i = 0; i < items_len; i++){
         tree_insert((*db)->tree, key_char_to_elem_t(items[i]->name), item_to_elem_t(items[i]));
      }
   }
   free(items);
   if(!success) return DB_FILE_ERROR;
   else return DB_NO_ERROR;
}

///////////////////////////////////// undo ////////////////////////////////////

void db_undo_enable(database_t *db, bool enable){
   db->undo_enabled = enable;
}

enum db_error db_undo(database_t *db, char **name){
   enum undo_type type;
   item_t *changed;
   char *changed_name;
   bool something_happened = undo_stack_undo(db->undo, &changed, &type, &changed_name);

   if(!something_happened){
      return DB_NOTHING_TO_UNDO;
   }

   if(type == UNDO_REMOVE){
      tree_insert(db->tree, key_char_to_elem_t(changed->name), item_to_elem_t(changed));
      *name = strdup(changed->name);
      return DB_UNDO_REMOVE;
   }else if(type == UNDO_ADD){
      elem_t info[2];
      tree_remove(db->tree, key_char_to_elem_t(changed_name), info);
      *name = strdup(changed_name);
      free(changed_name);
      delete_item(info[1]);
      return DB_UNDO_ADD;
   }else if(type == UNDO_EDIT){
      elem_t info[2];
      tree_remove(db->tree, key_char_to_elem_t(changed_name), info);
      delete_item(info[1]);
      *name = changed_name;
      tree_insert(db->tree, key_char_to_elem_t(changed->name), item_to_elem_t(changed));
      return DB_UNDO_EDIT;
   }

   return DB_NOTHING_TO_UNDO;
}
