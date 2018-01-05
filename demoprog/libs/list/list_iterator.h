/// \file list_iterator.h
/// \brief Iterator for list.h

#ifndef LIST__ITERATOR_H
#define LIST__ITERATOR_H

#include "list.h"
#include "../common.h"

/// Frees the list
/// applies the freeing function f on every element with type L
/// in the list. if f is NULL, then don't run any function on
/// the elements.
//void list_free(list_t *list, void (*f)(void*));

typedef struct list_iterator list_iterator_t;

/// Returns an iterator for the list.
///
/// The returned ::list_iterator_t* needs to be freed with a normal free()
/// 
/// \param list the list
/// \returns the list
list_iterator_t *list_get_iterator(list_t *list);

/// Returns true if the iterator has more than
/// inside it.
///
/// \param iterator iter
/// \returns true if it has more in it
bool list_iterator_has_next(list_iterator_t *iter);

/// Returns a pointer to the next element inside
/// the iterator. list_iterator_has_next(iter) must
/// be true to work.
/// The return pointer is a pointer to the actual
/// element in the list.
///
/// \param iterator iter
/// \returns a pointer to the next element
elem_t *list_iterator_next(list_iterator_t *iter);

/// removes the underlying entry that the last element from
/// last_iterator_next has returned from the list i.
/// The return value is a copy of the removed element.
//elem list_iterator_remove(list_iterator_t *iter);

#endif
