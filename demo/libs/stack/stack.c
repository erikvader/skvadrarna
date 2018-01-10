#include "stack.h"
#include <stdlib.h>


stack_t *stack_new(heap_t *heap, element_copy_fun copy, element_free_fun free){
  return list_new(heap, copy, free, NULL);
}

void stack_push(heap_t *heap, stack_t* stack, elem_t ele){
  list_prepend(heap, stack, ele);
}

bool stack_pop(stack_t *stack, elem_t *ele){
   bool success = list_first(stack, ele);
   if(success){
      list_remove(stack, 0, false);
      return true;
   }
   return false;
}

bool stack_top(stack_t *stack, elem_t *ele){
   return list_first(stack, ele);
}

bool stack_is_empty(stack_t *stack){
   return list_length(stack) <= 0;
}

void stack_delete(stack_t *stack, bool delete){
   list_delete(stack, delete);
}
