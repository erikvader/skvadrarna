/// \file db_file_save.h
/// \brief Database file handler
///
/// Saves/loads a bunch of ::item_t to/from a file
///

#ifndef _DB_FILE_SAVE_H_
#define _DB_FILE_SAVE_H_

#include "../file/block_writer.h"
#include "db_types.h"

/// saves an array of items to a file
///
/// \param items array of ::item_t pointers to save
/// \param items_len length of \p items
/// \param filename path to a file
/// \returns true if it was successfully saved
bool db_file_save(item_t const *const *items, const int items_len, const char *filename);

/// reads an array of items from a file
///
/// \param items pointer to an array where the items will be stored (this function allocated the array)
/// \param item_len pointer to an int where the amount of items read will be stored
/// \param filename path to a file to read from.
/// \returns true if the reading was successful
bool db_file_get(item_t ***items, int *item_len, const char *filename);

#endif
