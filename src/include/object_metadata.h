#ifndef __object_metadata_h__
#define __object_metadata_h__

#include "gc.h"

// allt är allignerat till 4 bytes (word)
// minsta storleken för ett objekt är 16 bytes

const size_t header_siz;

/// Builds a metadata object at where the pointer header is pointing.
/// The object isnt allowed to contain pointers
///
/// \param object place where the object is stored
/// \param size the size of the object to be stored.
void om_build_pointerless(void *object, const size_t size);

/// builds a metadata given a format string.
///
/// \param heap Adress to the heap metadata.
/// \param object Adress to the object, metadata will be stored in the HEADER_SIZ bytes before the object.
/// \param format Format string describing the object according to specification in assignment.
/// \returns false if build didn't succed, probably because format string is big and there is not enough space to alloc a object to store the format string in.

bool om_build(heap_t* heap, void* object, const char* format);

/// returns the size of the object
///
/// \param object Adress to an object
/// \returns size of the object

size_t om_size(const void *object);


/// returns the size of a format string
///
/// \param format char* specified in the assignment description.
/// \returns size of the format char*

size_t om_size_format(const char *format);


/// Returns the amount of pointers stored in the object.
/// Help for building arrays of pointers on the Stack.
///
/// \param object Object with unknown number of pointers inside it.
/// \returns Number of pointers inside the object.

int om_amount_pointers(const void *object);

/// Gets pointers to the suppoused pointers in the object according to the format string when the object header was builded.
/// Some of them might be NULL or CORRUPTED
///
/// \param object Object to get pointers from.
/// \param pointers Array to store the pointers to pointer in. Get size of array in om_amount_pointers.

void om_get_pointers(const void *object, void ***pointers);

// --------------------

/// returns true when object has been moved, otherwise false
///
/// \param object The Object
/// \return If the object has been moved.

bool om_has_forwarding(const void *object);

/// Void for setting header to forwarding mode (when moving during GC- event)
/// Doesn't move the object, just sets the header to redirect everything to new place.
///
/// \param object Object to set forwarding
/// \param adress New home

void om_set_forwarding(void *object, const void *adress);

/// gets the forwarding adress if the header is in forwarding mode.
///
/// \object The object that might have a forwarding adress.
/// \returns The forwarding adress if it has one, else NULL.

void *om_get_forwarding(const void *object);

#endif
