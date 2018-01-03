#ifndef _DB_UNDO_H_
#define _DB_UNDO_H_

#include "../stack/stack.h"
#include "db_types.h"

enum undo_type {UNDO_ADD, UNDO_REMOVE, UNDO_EDIT};

/// \typedef undo_action_t
/// hold information for necessary for an undo action
typedef struct undo_action undo_action_t;

typedef stack_t undo_stack_t;

//creates a new undo_stack
undo_stack_t* undo_stack_new();

// records that \p added was added. copies the name of \p added
void undo_stack_add(undo_stack_t *undo_s, item_t *added);

// records that \p copy was changed. Copies \p copy
// \p new_name is the new name of \p copy if it was changed.
void undo_stack_edit(undo_stack_t *undo_s, item_t *copy, char *new_name);

// records that \p removed was removed. Does NOT copy \p removed
void undo_stack_remove(undo_stack_t *undo_s, item_t *removed);

// reveals the latest undo.
// returns true if something could be undone. false if nothing happened
// UNDO_REMOVE:
//   \p changed was removed and wants to be reinserted. changed_name = NULL
// UNDO_ADD:
//   \p changed_name was added and wants to be removed. \p changed = NULL
// UNDO_EDIT:
//   \p changed turned into something with name \p changed_name.
//   \p changed_name wants to be removed and be replaced by \p changed
bool undo_stack_undo(undo_stack_t *undo_s, item_t **changed, enum undo_type *type, char **changed_name);

// frees the memory \p undo_s is using. Including all items.
void undo_stack_delete(undo_stack_t *undo_s);

#endif
