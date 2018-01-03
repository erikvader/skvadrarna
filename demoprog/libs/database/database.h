/// \file database.h
/// \brief Database for storing shoping items
///
/// This database stores a bunch of items. Every items has:
/// a name, a description and a price. Every item can be stored on
/// any number of shelves, but only one item per shelf.\n
/// \n
/// ALL strings are copied (strdup) when relevant and are always compared by value (strcmp).\n
/// \n
/// (eller?) Exact representation of ::item_t and ::shelf_t defined in db_types.h
///

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <stdbool.h>
#include "../common.h"

enum db_error{
   DB_NO_ERROR,             ///< No error occured
   DB_INVALID_SHELF_NAME,   ///< the name for a shelf is invalid. db_is_valid_shelf_name()
   DB_INVALID_ITEM_NAME,    ///< the name for an item is invalid. db_valid_item_name()
   DB_INVALID_PRICE,        ///< the price is invalid. db_valid_item_price()
   DB_INVALID_AMOUNT,       ///< some kind of amount is invalid. An negative amount is often invalid.
   DB_SHELF_BUSY,           ///< The shelf we are trying to act on is busy (some other item is already on it db_put_item_on_shelf())
   DB_ITEM_NO_EXIST,        ///< the item we are trying to act on doesn't exist
   DB_SHELF_NO_EXIST,       ///< the shelf we are trying to act on doesn't exist
   DB_ITEM_ALREADY_EXISTS,  ///< an item already exists
   DB_SHELF_ALREADY_EXISTS, ///< and shelf already exists
   DB_FILE_ERROR,           ///< some kind of file error occured
   DB_NOTHING_TO_UNDO,      ///< nothing to be undone
   DB_UNDO_REMOVE,          ///< a remove action was undone (something was added)
   DB_UNDO_EDIT,            ///< an edit was undone
   DB_UNDO_ADD,             ///< an add action was undone (something got removed)
};

/// the database
typedef struct database database_t;

/////////////////////////////////// general ///////////////////////////////////

/// create an empty database
///
/// need to be properly deleted with db_free()
///
/// \returns an empty newly created database
/// \see db_free
database_t* db_new();

/// frees and deletes the database
///
/// freeing all memory used by \p db and deletes all
/// items and shelves.
///
/// \param db database to delete
/// \see db_new
void db_free(database_t *db);

/// undo the last undoable action on \p db
///
/// DB_UNDO_ADD:
///   \p name is the name of the thing that got removed
/// DB_UNDO_REMOVE:
///   \p name is the name of the thing that got added
/// DB_UNDO_EDIT:
///   \p name is the name of the thing that got modified. (rename from pasta to yoghurt will return yoghurt)
/// 
/// \param db database to undo the last action on
/// \param name the name of the thing that got undone. Needs to be freed
/// \returns ::DB_UNDO_ADD, ::DB_UNDO_EDIT, ::DB_UNDO_REMOVE, ::DB_NOTHING_TO_UNDO
enum db_error db_undo(database_t *db, char **name);

/// turns recording of undo history on or off
///
/// If this is disabled, then every function that
/// records to the undo history doesn't do it anymore.
///
/// \param db database to use
/// \returns enable if undo recording should be on or off
void db_undo_enable(database_t *db, bool enable);

///////////////////////////////////// items ///////////////////////////////////

/// adds a new item to \p db
///
/// Adds a new item to \p db with the name \p name,
/// the description \p description and price \p price.\n
/// Both \p description and \p name are copied with strdup().\n
/// \p name needs to fulfill db_valid_name() and \p price needs
/// to fulfill db_valid_price(). If they are not, the item will not be
/// added and this function will return false.\n
/// \n
/// An item with name \p name can not already exist.\n
///\n
/// possible errors: ::DB_INVALID_ITEM_NAME, ::DB_INVALID_PRICE, ::DB_ITEM_ALREADY_EXISTS\n
/// \n
/// This function is undoable
///
/// \param db database to add to
/// \param name name for the new item, copied with strdup
/// \param description description for the new item, copied with strdup
/// \param price price for the new item
/// \returns ::DB_NO_ERROR if successfully added and all preconditions true
/// \see db_remove_item, db_valid_name, db_valid_price, db_undo, db_item_exists
enum db_error db_add_item(database_t *db, const char *name, const char *description, const int price);

/// Checks if an item already exists in \p db
///
/// returns true if an item with name \p item exists in \p db
///
/// \param db database to check existence in
/// \param item name of and item to check
/// \returns true if \p item exists in \p db
bool db_item_exists(const database_t *db, const char *item);

/// change the name for an item
///
/// \p new_name is copied with strdup\n
/// \item needs to exist.\n
/// \n
/// possible errors: ::DB_INVALID_ITEM_NAME, ::DB_ITEM_NO_EXIST\n
/// \n
/// This function is undoable
///
/// \param db database to use
/// \param item old name of an item to change
/// \param new_name new name for \p item
/// \returns ::DB_NO_ERROR if successfull
enum db_error db_set_item_name(database_t *db, const char *item, const char *new_name);

/// change the description for an item
///
/// \p new_desc is copied with strdup\n
/// \item needs to exist\n
/// \n
/// possible errors: ::DB_INVALID_ITEM_NAME, ::DB_ITEM_NO_EXIST\n
/// \n
/// This function is undoable
///
/// \param db database to use
/// \param item name of item to modify
/// \param new_desc new description for \p item
/// \returns ::DB_NO_ERROR if successfull
enum db_error db_set_item_desc(database_t *db, const char *item, const char *new_desc);

/// change the price for an item
///
/// \item needs to exist\n
/// \n
/// possible errors: ::DB_INVALID_PRICE, ::DB_ITEM_NO_EXIST, ::DB_INVALID_ITEM_NAME\n
/// \n
/// This function is undoable
///
/// \param db database to use
/// \param item name of item to modify
/// \param price new price for \p item
/// \returns ::DB_NO_ERROR if successfull
enum db_error db_set_item_price(database_t *db, const char *item, const int new_price);

/// retrieve the description of an item
///
/// \p items needs to exists
/// \code
/// char *desc;
/// db_get_item_desc(db, "smör", &desc);
/// \endcode
///
/// \p desc should NEVER be modified or freed, ONLY read.\n
/// \n
/// possible errors: ::DB_INVALID_ITEM_NAME, ::DB_ITEM_NO_EXIST
///
/// \param db database to use
/// \param item name of item to retrieve from
/// \param desc place to store the retrieved description
/// \returns ::DB_NO_ERROR of successful
enum db_error db_get_item_desc(const database_t *db, const char *item, const char **desc);

/// retrieve the price of an item
///
/// \p items needs to exists
/// \code
/// int *price;
/// db_get_item_price(db, "smör", &price);
/// \endcode
/// \n
/// \n
/// possible errors: ::DB_INVALID_ITEM_NAME, ::DB_ITEM_NO_EXIST
///
/// \param db database to use
/// \param item name of item to retrieve from
/// \param price place to store the retrieved price
/// \returns ::DB_NO_ERROR of successful
enum db_error db_get_item_price(const database_t *db, const char *item, int *price);

/// retrieve the description and price of an item
///
/// \p items needs to exists
/// \code
/// int *price;
/// char *desc;
/// db_get_item_desc(db, "smör", &desc, &price);
/// \endcode
/// \n
/// \n
/// possible errors: ::DB_INVALID_ITEM_NAME, ::DB_ITEM_NO_EXIST
///
/// \p desc should NEVER be modified or freed, ONLY read.
///
/// \param db database to use
/// \param item name of item to retrieve from
/// \param desc place to store the retrieved description
/// \param price place to store the retrieved price
/// \returns ::DB_NO_ERROR of successful
enum db_error db_item_get_all(const database_t *db, const char *item, const char ** description, int *price);

/// returns all item names in lexicographical order
///
/// Returns a pointer of strings. The strings should NOT be modified
/// or freed, ONLY read. \p size is the length of the return value.
/// The returned array should be freed, but not the elements.\n
/// \n
/// If the database is empty, a NULL-pointer is returned and *size will
/// be set to 0.
///
/// \param db database to use
/// \param size pointer to an int where the size will be written
/// \returns an array with all item names
const char** db_get_all_items(const database_t *db, int *size);

/// removes and item with name \p item from \p db
///
/// \p item must exist in \p db.\n
/// \p item will be completely removed from the database
/// and all shelves containing \p item. \n
/// This action is undoable.\n
/// \n
/// possible errors: ::DB_INVALID_ITEM_NAME, ::DB_ITEM_NO_EXIST\n
/// \n
/// This function is undoable
///
/// \param db database to remove from
/// \param item item name to remove from \p db
/// \returns ::DB_NO_ERROR if successful
/// \see db_undo
enum db_error db_remove_item(database_t *db, const char* item);

/// checks if an name is an valid name for an item
///
/// a valid name is any null-terminated string that is not empty.
/// In other words, the length of the string must be strictly greater
/// than zero.
///
/// \param name name to check
/// \returns true if \p name is a valid name for an item
bool db_valid_item_name(const char *name);

/// check if \p price is an valid price
///
/// A valid price is an non-negative integer.
///
/// \param price price to check
/// \returns true if valid
bool db_valid_item_price(const int price);

/////////////////////////////////// shelves ///////////////////////////////////

/// checks whether \p name is an valid name for a shelf
///
/// A correctly formatted name begins with one character followed by any number if digits.\n
/// example: A23
/// 
/// \param name the name to check
/// \returns true if it is valid
bool db_is_valid_shelf_name(const char *name);

/// put an amount if \p item on \p shelf_name
///
/// Tries to put \p amount of \p item on \p shelf_name. \n
/// \n
/// prerequisites: \p amount >= 0, \p shelf_name is a valid shelf name. \p item must have been with by db_add_item().
/// \p shelf_name can't be occupied by another item.\n
/// \n
/// possible errors: ::DB_INVALID_SHELF_NAME, ::DB_INVALID_ITEM_NAME, ::DB_INVALID_AMOUNT, ::DB_SHELF_BUSY, ::DB_SHELF_ALREADY_EXISTS, ::DB_ITEM_NO_EXIST\n
/// \n
/// The error ::DB_SHELF_ALREADY_EXISTS is a special case in this function. It means that the item existed and \p amount was added to it's amount. It is NOT an error stating that something went wrong, in THIS case ONLY.\n
/// \n
/// This function is undoable
///
/// \param db database to use
/// \param item thing to put on a shelf
/// \param shelf_name name of a shelf to add \p item on
/// \param amount of \p item
/// \returns ::DB_NO_ERROR if successful
enum db_error db_put_item_on_shelf(database_t *db, const char *item, const char *shelf_name, const int amount);

/// removes an item from a shelf
///
/// removes \p amount of whatever item is placed on \p shelf.\n
/// \p amount >= 0. If amount is greater than or equal to the number of items stored
/// on \p shelf, then everything will be removed.\n
/// \n
/// possible errors: ::DB_INVALID_AMOUNT, ::DB_INVALID_SHELF_NAME, :: DB_SHELF_NO_EXIST\n
/// \n
/// This function is undoable
///
/// \param db database to use
/// \param shelf to act on
/// \param amount to remove
/// \returns ::DB_NO_ERROR if successful
enum db_error db_remove_item_from_shelf(database_t *db, const char *shelf, const int amount);

/// checks whether \p shelf_name is in use
///
/// returns: \n
/// \t DB_INVALID_SHELF_NAME: \p shelf_name invalid
/// \t DB_SHELF_BUSY: \p shelf_name is in use
/// \t DB_NO_ERROR: \p shelf_name is NOT in use
/// 
/// \param db database to use
/// \param shelf_name shelf to check status of. Needs to have a correct name, or else always false. db_is_valid_shelf_name()
/// \param owner place to store the name of the item who is using \p shelf_name. Can be NULL of this result isn't of interest
/// \param owner_amount place to store amount of \p owner on \p shelf_name. Can be NULL of this result isn't of interest
/// \returns an ::enum db_error describing the status of \p shelf_name
enum db_error db_shelf_is_in_use(const database_t *db, const char *shelf_name, const char **owner, int *owner_amount);

//////////////////////////////// shelf iterator ///////////////////////////////

/// returns an array with objects describing all shelfs for an item
///
/// \p amount is number of shelves returned in \p shelves. \p shelves is a pointer
/// to an array. This functions allocates memory for the array. If the size
/// of the array would be 0, then \p shelves will not be touched.
/// information from every array entry can be extracted with db_shelf_get_amount() and db_shelf_get_name().
/// \code
/// elem_t *arr;
/// int amount;
/// db_get_shelves(db, "smör", &arr, &amount);
/// \endcode
/// Possible errors: ::DB_ITEM_NO_EXIST, ::DB_INVALID_ITEM_NAME
///
/// \param db database to use
/// \param item name of item to fetch shelves for
/// \param shelves pointer to an array-pointer to store special shelf objects. DO NOT modify these. The array needs to be freed!
/// \param amount pointer to store the amount of shelves retrieved in \p shelves
/// \returns ::DB_NO_ERROR if retrieval was successful
/// \see db_shelf_get_amount db_shelf_get_name
enum db_error db_get_shelves(const database_t *db, const char *item, elem_t **shelves, int *amount);

/// get amount of items stored on this shelf
///
/// Retrieves information from a shelf token retrieved from db_get_shelves()
///
/// \param shelf special shelf token
/// \returns the amount of items on \p shelf
/// \see db_get_shelves
int db_shelf_get_amount(const elem_t shelf);

/// get the name of this shelf
///
/// Retrieves information from a shelf token retrieved from db_get_shelves()
///
/// \param shelf special shelf token
/// \returns name of the shelf. DO NOT modify it.
/// \see db_get_shelves
const char* db_shelf_get_name(const elem_t shelf);

///////////////////////////////// save to file ////////////////////////////////

/// saves the database to a file
///
/// \param db db to save
/// \param filename name of file to save in
/// \returns ::DB_FILE_ERROR if it could not be saved
enum db_error db_save(const database_t *db, const char *filename);

/// loads a database stored in a file
///
/// \param db pointer to save the newly allocated database in
/// \param filename name of file to read from
/// \returns ::DB_FILE_ERROR if not successful
enum db_error db_load(database_t **db, const char *filename);

#endif
