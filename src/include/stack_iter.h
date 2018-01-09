#ifndef __stack_iter_h__
#define __stack_iter_h__

/// \file stack_iter.h
/// \authors skvadrarna

#include <stdbool.h>
#include "gc.h"

//extern char **environ;

/// Finds the next pointer in the stack that points on an object in the heap.
/// It cannot disguinguish between a pointer and an 8-byte block that looks like a pointer.
///
/// Normal use-case: start by pointing to a pointer in the current stack frame. The
/// function will then try to move toward "environ", until it finds a memory block that
/// looks like a pointer or it reaches "environ". It can then the same double pointer
/// as argument to find the next pointer
///
/// Returns NULL if it cannot find one and it reaches variable "environ"
///
/// \param heap the heap wherin the pointer might point to
/// \param top the initial value where to search for a pointer
/// \return a pointer to a pointer in the stack that points at an object in the heap.
void **si_next_pointer(heap_t *heap,void **top);


/// Function used in si_next_pointer_dbg. It will check to see if a pointer is in the "heap" or not.
typedef bool(*si_pointer_check_fun)(heap_t *,void *);


/// A more general case for si_next_pointer. Used for debugging reasons.
///
/// returns NULL if it reaches "environ"
///
/// \param heap the heap wherein the pointer might point to
/// \param top the initial value from where to search for a pointer
/// \param fun a function that determens if the pointer is in the heap or not.
/// \param alignment the alignment for where the for the pointers. Assumed 8 for most 64-based machines.
/// \return a pointer to a pointer in the stack that points at an object in the heap.
void **si_next_pointer_dbg(heap_t *heap, void **top, si_pointer_check_fun fun, size_t alignment);




#endif
