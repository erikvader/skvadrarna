#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>
#include "../common.h"
#include "../list/list.h"

typedef list_t stack_t;

stack_t *stack_new(element_copy_fun copy, element_free_fun free);

void stack_push(stack_t* stack, elem_t ele);

bool stack_pop(stack_t* stack, elem_t *ele);

bool stack_top(stack_t *stack, elem_t *ele);

bool stack_is_empty(stack_t* stack);

void stack_delete(stack_t *stack, bool delete);

#endif
