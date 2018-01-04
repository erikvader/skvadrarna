#ifndef __gc_event_h__
#define __gc_event_h__

#include "gc.h"

/// runs the GC cleanup process
///
/// If hm_is_unsafe() is true, then potential pointers from the stack
/// are not changed (conservative GC). If it is true, then everything
/// that looks like a pointer from the stack can be modified. <p>
/// Warning: <p>
/// this can change unwanted values that aren't even pointers, use
/// unsafe with care. <p>
/// <p>
/// If this step would for some reason fail, the whole program crashes
/// with error code 1. The main reason for failing is if there isn't
/// enough memory to copy objects to. This can happen if there are too
/// many live objects and/or the threshold for triggering this event
/// is too large.
///
/// \param heap the heap to run on
void gce_run_gc_event(heap_t *heap);

#endif
