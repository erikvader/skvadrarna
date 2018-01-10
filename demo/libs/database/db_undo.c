#include <stdlib.h>
#include "../stack/stack.h"
#include "db_undo.h"
#include <string.h>
#include "../utils/utils.h"

struct undo_action{
   enum undo_type type; ///< type of action we did
   item_t *changed;     ///< item that got modified. old version
   char *changed_name;  ///< the new name of item that got modified.
};

// completely frees an undo_action_t and everything in it.
void undo_free_action(undo_action_t *undo_a){
   if(undo_a->changed != NULL) delete_item(item_to_elem_t(undo_a->changed));
   // if(undo_a->changed_name != NULL) free(undo_a->changed_name);
}

undo_stack_t* undo_stack_new(heap_t *heap){
  return stack_new(heap, NULL, (element_free_fun) undo_free_action);
}

// allocates a new undo_action.
undo_action_t* undo_make_action(heap_t *heap){
  return h_alloc_struct(heap, "***");
}

void undo_stack_push(heap_t *heap, undo_stack_t *undo_s, undo_action_t *undo_a){
  stack_push(heap, undo_s, (elem_t)(void*)undo_a);
}

void undo_stack_add(heap_t *heap, undo_stack_t *undo_s, item_t *added){
   undo_action_t *new = undo_make_action(heap);
   new->type = UNDO_ADD;
   new->changed = NULL;
   new->changed_name = h_strdup(heap, added->name);
   undo_stack_push(heap, undo_s, new);
}

void undo_stack_remove(heap_t *heap, undo_stack_t *undo_s, item_t *removed){
   undo_action_t *new = undo_make_action(heap);
   new->type = UNDO_REMOVE;
   new->changed = removed;
   new->changed_name = NULL;
   undo_stack_push(heap, undo_s, new);
}

void undo_stack_edit(heap_t *heap, undo_stack_t *undo_s, item_t *copy, char *new_name){
   undo_action_t *new = undo_make_action(heap);
   new->type = UNDO_EDIT;
   new->changed = copy_item(heap, copy);
   new->changed_name = h_strdup(heap, new_name);
   undo_stack_push(heap, undo_s, new);
}

// takes the value in 'last_action' and reverts what it did.
// Setting it to NULL afterwards.
bool undo_stack_undo(undo_stack_t *undo_s, item_t **changed, enum undo_type *type, char **changed_name){
   if(stack_is_empty(undo_s)){
      return false;
   }

   elem_t ele;
   stack_pop(undo_s, &ele);
   undo_action_t *popped = ele.p;

   *changed = popped->changed;
   *type = popped->type;
   *changed_name = popped->changed_name;
   return true;
}

void undo_stack_delete(undo_stack_t *undo_s){
   stack_delete(undo_s, true);
}
