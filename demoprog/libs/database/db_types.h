/**
 *   \file db_types.h
 *   \brief definitions of stuff to store in the database
 */

#ifndef _DB_TYPES_H_
#define _DB_TYPES_H_

#include "../list/list.h"
#include "../common.h"

/// \typedef shelf_t
/// holds information for a shelf
typedef struct shelf shelf_t;

/// \typedef item_t
/// holds information for an item
typedef struct item item_t;

struct item{
   char *name; ///< the name of the item
   char *desc; ///< the item's description
   int price; ///< the item's price in ören
   list_t *shelves; ///< list of ::shelf_t
};

struct shelf{
   char *name; ///< the name of the shelf with a special format defined in db_valid_shelf_name()
   int num; ///< amount of items on this shelf
};

// free a shelf
void delete_shelf(elem_t s);

// free an item
void delete_item(elem_t item);

// casts item_t* to a elem_t
elem_t item_to_elem_t(item_t *item);

// casts char* to elem_t
elem_t key_char_to_elem_t(const char *key);

//creates a shelf on the heap
shelf_t* make_shelf(char *name, int num);

//creates an item on the heap
item_t* make_item(char *name, char *desc, int price);

//returnes a copy of \p item on the heap
item_t* copy_item(item_t *item);

//returnes a copy of \p s on the heap
shelf_t* copy_shelf(shelf_t *s);

#endif
