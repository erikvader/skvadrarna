#ifndef __object_metadata_h__
#define __object_metadata_h__

#include <stdlib.h>
#include <stdbool.h>

// allt är allignerat till 4 bytes (word)
// minsta storleken för ett objekt är 16 bytes

const size_t header_siz;

/// build_pointerless
///
/// builds a metadata object at where the pointer header is pointing
///
/// \param object place where the object is stored
/// \param size the size of the object to be stored.
void om_build_pointerless(void *object, size_t size);


// builds a metadata given a format string.
void om_build(void *object, char *format);

// returns the size of the object
size_t om_size(void *object);

// return the amount of pointers stored in the object
int om_amount_pointers(void *object);

// gets all the adresses for where the pointers are stored. pointers param is where to store this information
void om_get_pointers(void *object, void **pointers);

// --------------------

// returns true when object has been moved, otherwise false
bool om_has_forwarding(void *object);

// converts the header to a forwarding adress
void om_set_forwarding(void *object, void *adress);

// gets the adress of the forwarding adress
void *om_get_forwarding(void *object);

#endif
