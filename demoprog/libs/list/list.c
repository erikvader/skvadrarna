#include <stdlib.h>
#include "list.h"
#include <stdio.h>
#include "list_iterator.h"

struct node {
    elem_t value;
    node_t *next;
};

struct list {
    node_t *first, *last;
    int length;
    element_copy_fun elem_copy;
    element_comp_fun elem_comp;
    element_free_fun elem_free;
};

struct list_iterator{
    node_t **curr_node;
};

static
elem_t default_copy(elem_t elem){
    return elem;
}

static
void default_free(elem_t elem){
    elem = elem;
}

static
int default_compare(elem_t elem1, elem_t elem2){
    return elem1.i - elem2.i;
}

list_t *list_new(element_copy_fun copy, element_free_fun free, element_comp_fun compare){
    list_t *newList = malloc(sizeof(list_t));

    if(newList != NULL){
        newList->first = NULL;
        newList->last = NULL;
        newList->length = 0;
        newList->elem_comp = compare ? compare : default_compare;
        newList->elem_copy = copy ? copy : default_copy;
        newList->elem_free = free ? free : default_free;
    }

    return newList;
}

node_t *node_new(elem_t value){
    node_t *newNode = malloc(sizeof(node_t));

    if(newNode != NULL){
        newNode->value = value;
        newNode->next = NULL;
    }

    return newNode;
}

//converts negative index to corresponding positive index
//list_len > 0
//  0  1  2
// -3 -2 -1
int actual_index(int list_len, int index){
    if(index < 0){
        index = list_len + index;
    }
    if(index < 0){
        index = 0;
    }else if(index >= list_len){
        index = list_len-1;
    }
    return index;
}


void list_insert(list_t *list, int index, elem_t elem){
    index = actual_index(list->length+1, index);

    node_t *newNode = node_new(list->elem_copy(elem));

    if(list->length == 0){
        list->first =  newNode;
        list->last = newNode;
    }
    else if(index == list->length){
        list->last->next = newNode;
        list->last = newNode;
    }
    else{
        node_t **curr_node = &list->first;
        for(int i = 0; i < index; i++){
            curr_node = &((*curr_node)->next);
        }
        newNode->next = *curr_node;
        *curr_node = newNode;
        if(newNode->next == NULL){
            list->last = newNode;
        }
    }
    list->length += 1;
}

void list_append(list_t *list, elem_t elem){
    list_insert(list, -1, elem);
}

void list_prepend(list_t *list, elem_t elem){
    list_insert(list, 0, elem);
}

void list_remove(list_t *list, int index, bool delete){
    if(list->length == 0){
        return;
    }

    index = actual_index(list->length, index);

    node_t **cur = &list->first;
    node_t *prev = NULL;
    for(int i = 0; i < index; i++){
        prev = *cur;
        cur = &((*cur)->next);
    }

    node_t *removed = *cur;
    *cur = (*cur)->next;

    if(list->last == removed){
        list->last = prev;
    }
    if(delete) list->elem_free(removed->value);
    free(removed);
    list->length--;

}

bool list_get(list_t *list, int index, elem_t *result){
    int list_length = list->length;
    if(list_length < 1){
        return false;
    }
    index = actual_index(list_length, index);
    node_t *current = list->first;

    if(index == 0)
    {
        *result = list->first->value;
        return true;
    }
    else if(index == -1 || index == list_length)
    {
        *result = list->last->value;
        return true;
    }

    for(int i = 0; i < list_length; i++){
        if(i == index){
            *result = current->value;
            return true;
        }
        current = current->next;
    }
    //printf("Invalid index!\n");
    return false;
}

bool list_first(list_t *list, elem_t *result){
    return list_get(list, 0, result);
}

bool list_last(list_t *list, elem_t *result){
    return list_get(list, list_length(list) - 1, result);
}

int list_length(list_t *list){
    return list->length;
}

void list_delete(list_t *list, bool delete){
    int list_length = list->length;
    for(int i = 0; i < list_length; i++){
        list_remove(list, 0, delete);
    }
    free(list);
}

bool list_apply(list_t *list, elem_apply_fun fun, void *data){
    if(list->first == NULL){
        return false;
    }
    else{
        node_t *current = list->first;
        bool or_fun = false;
        for(; current != NULL; current = current->next){
            or_fun = fun(current->value, data) || or_fun;
        }
        return or_fun;
    }
}

int list_contains(list_t *list, elem_t elem){
    int index = 0;
    node_t *current = list->first;

    for(; current != NULL; current = current->next){
        if(list->elem_comp(current->value, elem) == 0){
            return index;
        }
        index += 1;
    }
    return -1;
}

// void display_list(list_t *list){
//     node_t *temp = list->first;

//     for(;temp != NULL; temp = temp->next){
//         printf("%d\n", temp->value);
//     }
// }

// ----------------------- ITERATOR --------

list_iterator_t *list_get_iterator(list_t *list){
    list_iterator_t* new_iterator = malloc(sizeof(list_iterator_t));

    new_iterator->curr_node = &list->first;

    return new_iterator;
}

elem_t *list_iterator_next(list_iterator_t *iter){
    elem_t *elem = &((*iter->curr_node)->value);
    iter->curr_node = &((*iter->curr_node)->next);
    return elem;
}

bool list_iterator_has_next(list_iterator_t *iter){
    return *iter->curr_node != NULL;
}

//int main(){
//    union element e;
//
//    list_t *x = list_new(NULL, NULL, NULL);
//
//    list_remove(x, -125151, true);
//
//    int indexList = 0;
//    e.i = 0;
//    list_insert(x, 0, e);
//    indexList = list_contains(x, e);
//    printf("%d\n", indexList);
//    e.i = 123010;
//    list_insert(x, 123010, e);
//    indexList = list_contains(x, e);
//    printf("%d\n", indexList);
//    e.i = 0;
//    list_insert(x, 0, e);
//    indexList = list_contains(x, e);
//    printf("%d\n", indexList);
//    e.i = 2;
//    list_insert(x, 2, e);
//    indexList = list_contains(x, e);
//    printf("%d\n", indexList);
//    e.i = 3;
//    list_insert(x, 2000, e);
//    indexList = list_contains(x, e);
//    printf("%d\n", indexList);
//    e.i = 4;
//    list_insert(x, -5, e);
//    indexList = list_contains(x, e);
//    printf("%d\n", indexList);
//    e.i = 5;
//    list_insert(x, 0, e);
//    indexList = list_contains(x, e);
//    printf("%d\n", indexList);
//    e.i = 6;
//    list_insert(x, -1023010, e);
//    indexList = list_contains(x, e);
//    printf("%d\n", indexList);
//
//    int y = list_length(x);
//    printf("List length: %d\n", y);
//
//    e.p = NULL;
//    list_get(x, -12315, &e);
//    printf("First element: %d\n", e);
//
//    e.p = NULL;
//    list_get(x, 12312314, &e);
//    printf("Last element: %d\n", e);
//
//    display_list(x);
//
//    list_delete(x, x->first != NULL);
//
//    if(x->length == 0) {
//        printf("LIST IS DELETED\n");
//    }
//
//
//    return 0;
//}