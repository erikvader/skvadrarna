#include "db_file_save.h"
#include "../list/list_iterator.h"
#include <string.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
//                                 save stuff                                //
///////////////////////////////////////////////////////////////////////////////

void save_int(blk_file bf, const int i){
   blk_write(bf, sizeof(int), &i);
}

void save_string(blk_file bf, const char *string){
   int slen = strlen(string)+1; // +1 for NULL char
   save_int(bf, slen);
   blk_write(bf, sizeof(char)*slen, string);
}

void save_shelf(blk_file bf, const shelf_t *she){
   save_string(bf, she->name);
   save_int(bf, she->num);
}

void save_list(heap_t *heap, blk_file bf, list_t *list){
   save_int(bf, list_length(list));
   list_iterator_t *ite = list_get_iterator(heap, list);
   while(list_iterator_has_next(ite)){
      shelf_t *she = (*list_iterator_next(ite)).p;
      save_shelf(bf, she);
   }
}

void save_item(heap_t *heap, blk_file bf, const item_t *item){
   save_string(bf, item->name);
   save_string(bf, item->desc);
   save_int(bf, item->price);
   save_list(heap, bf, item->shelves);
}

bool db_file_save(heap_t *heap, item_t const *const *items, const int items_len, const char *filename){
   blk_file bf = blk_open_save(filename);
   if(!blk_success_open(bf)){
      return false;
   }

   save_int(bf, items_len);
   for(int i = 0; i < items_len; i++){
     save_item(heap, bf, items[i]);
   }

   blk_close_file(bf);
   return true;
}

///////////////////////////////////////////////////////////////////////////////
//                                 read stuff                                //
///////////////////////////////////////////////////////////////////////////////

int read_int(blk_file bf){
   int a;
   blk_read(bf, &a);
   return a;
}

static
char* read_string(heap_t *heap, blk_file bf){
   int len = read_int(bf);
   char *str = h_alloc_data(heap, sizeof(char)*len);
   blk_read(bf, str);
   return str;
}

shelf_t* read_shelf(heap_t *heap, blk_file bf){
  shelf_t *she = h_alloc_struct(heap, "*i");
   she->name = read_string(heap, bf);
   she->num = read_int(bf);
   return she;
}

list_t* read_list(heap_t *heap, blk_file bf){
   int len = read_int(bf);
   list_t *lis = list_new(heap, NULL, delete_shelf, NULL);
   for(int i = 0; i < len; i++){
     shelf_t *she = read_shelf(heap, bf);
      list_append(heap, lis, (elem_t) { .p=she });
   }
   return lis;
}

item_t* read_item(heap_t *heap, blk_file bf){
  item_t * item = h_alloc_struct(heap, "**i*");
   item->name = read_string(heap, bf);
   item->desc = read_string(heap, bf);
   item->price = read_int(bf);
   item->shelves = read_list(heap, bf);
   return item;
}

bool db_file_get(heap_t *heap, item_t ***items, int *item_len, const char *filename){
   blk_file bf = blk_open_read(filename);
   if(!blk_success_open(bf)){
      return false;
   }

   *item_len = read_int(bf);
   
   char buf[*item_len + 1];

   for (int i = 0; i < *item_len; ++i) {
     buf[i] = '*';
   }

   buf[*item_len] = '\0';
   *items = h_alloc_struct(heap,buf );
   for(int i = 0; i < *item_len; i++){
     (*items)[i] = read_item(heap, bf);
   }

   blk_close_file(bf);
   return true;
}
