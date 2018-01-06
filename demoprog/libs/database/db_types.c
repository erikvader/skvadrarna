#include "db_types.h"
#include <stdlib.h>
#include "../list/list_iterator.h"
#include <string.h>

//frees a shelf
void delete_shelf(elem_t s){
   shelf_t *shelf = s.p;
}

// Frees everything in the item_t item
void delete_item(elem_t item){
   item_t *itemp = item.p;
   list_delete(itemp->shelves, true);
}

//The shelves list_t is allocated but empty.
item_t* make_item(heap_t *heap, char *name, char *desc, int price){
  item_t *new = h_alloc_struct(heap, "**i*");
   new->name = name;
   new->desc = desc;
   new->price = price;

   new->shelves = list_new(heap, NULL, delete_shelf, NULL);
   return new;
}

shelf_t* make_shelf(heap_t *heap, char *name, int num){
  shelf_t *new = h_alloc_struct(heap, "*i");
   new->name = name;
   new->num = num;
   return new;
}

elem_t key_char_to_elem_t(const char *key){
   return (elem_t) { .p = (char*) key };
}

elem_t item_to_elem_t(item_t *item){
   return (elem_t) { .p = item };
}

item_t* copy_item(heap_t *heap, item_t *item){
  item_t *copy = make_item(heap, strdup(item->name), strdup(item->desc), item->price);
  list_iterator_t *ite = list_get_iterator(heap, item->shelves);
   while(list_iterator_has_next(ite)){
     elem_t shelf_c = (elem_t)(void*)copy_shelf(heap, (*list_iterator_next(ite)).p);
      list_append(heap, copy->shelves, shelf_c);
   }
   return copy;
}

shelf_t* copy_shelf(heap_t *heap, shelf_t *s){
  return make_shelf(heap, strdup(s->name), s->num);
}
