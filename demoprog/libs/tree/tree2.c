#include <stdio.h>
#include "tree.h"
#include <stdlib.h>
#include <string.h>

enum tree_rotate_dir {ROTATE_LEFT, ROTATE_RIGHT};

typedef struct tree_node tree_node;
struct tree_node {
   int height;
   elem_t data;
   tree_key_t key;
   union {
      struct{
         tree_node* left;
         tree_node* right;
      };
      tree_node* child[2];
   };
};

struct tree{
   tree_node *root;
   element_copy_fun ele_copy;
   key_copy_fun key_copy;
   key_free_fun key_free;
   element_free_fun ele_free;
   element_comp_fun compare;
};

static
elem_t default_copy(elem_t ele){
   return ele;
}

static
void default_free(elem_t ele){
   ele = ele;
}

static
int default_compare(elem_t a, elem_t b){
   return a.i - b.i;
}

tree_t *tree_new(element_copy_fun elem_copy, key_copy_fun key_copy, element_free_fun elem_free, key_free_fun key_free, element_comp_fun compare){
   tree_t *new = malloc(sizeof(tree_t));

   if (new != NULL){
      new->root = NULL;
      new->ele_copy = elem_copy ? elem_copy : default_copy;
      new->key_copy = key_copy  ? key_copy  : default_copy;
      new->key_free = key_free  ? key_free  : default_free;
      new->ele_free = elem_free ? elem_free : default_free;
      new->compare  = compare   ? compare   : default_compare;
   }

   return new;
}

tree_node* new_node(tree_key_t key, elem_t data){
   tree_node* new = malloc(sizeof(tree_node));

   if (new != NULL) {
      new->height = 1;
      new->key = key;
      new->data = data;
      new->left = NULL;
      new->right = NULL;
   }

   return new;
}

void tree_node_delete(tree_t *tree, tree_node *node, bool dk, bool de){
   if(node == NULL) return;

   tree_node_delete(tree, node->left, dk, de);
   tree_node_delete(tree, node->right, dk, de);

   if (dk) tree->key_free(node->key);
   if (de) tree->ele_free(node->data);

   free(node);
}

void tree_delete(tree_t* tree, bool delete_keys, bool delete_elements){
   if(tree->root != NULL){
      tree_node_delete(tree, tree->root, delete_keys, delete_elements);
   }
   free(tree);
}

bool tree_empty(tree_t* t){
   return t->root == NULL;
}

// bool is_root(tree_node* t){
//    return t->parent == NULL;
// }

bool is_leaf(tree_node * n){
   return n->left == NULL && n->right == NULL;
}

// finds where key should be placed in a tree_t tree
// find_node(tree, &tree->root, &par, key)
// saves parent in par and returns pointer to
// a pointer in parent that points on the found item.
// if root is NULL, then tree is empty and &tree->root is returned
tree_node **find_node(tree_t *tree, tree_node **t, tree_node **parent, tree_key_t key){
   if(*t == NULL){ //empty tree
      return t;
   }

   if(parent != NULL) *parent = *t;

   int comp = tree->compare(key, (*t)->key);

   if(comp < 0){
      return find_node(tree, &(*t)->left, parent, key);
   }else if(comp > 0){
      return find_node(tree, &(*t)->right, parent, key);
   }else{
      return t;
   }
}

// tree_node **find_node(tree_t *tree, tree_node **t, tree_node **parent, tree_key_t key){

//    tree_node **cur = t;
//    while(*cur != NULL){
//       int comp = tree->compare(key, (*cur)->key);

//       if(parent != NULL) *parent = *cur;

//       if(comp < 0){
//          cur = &(*cur)->left;
//       }else if(comp > 0){
//          cur = &(*cur)->right;
//       }else{
//          break;
//       }
//    }

//    return cur;
// }

int tree_node_size(tree_node *t){
   if(t == NULL){
      return 0;
   }else{
      return tree_node_size(t->left) + tree_node_size(t->right) + 1;
   }
}

int tree_size(tree_t *tree){
   return tree_node_size(tree->root);
}

int node_height(tree_node *n){
   if(n == NULL) return 0;
   return n->height;
}

int tree_depth(tree_t *tree){
   return node_height(tree->root);
}

// -1 -> left heavy
// 0  -> balanced
// 1  -> right heavy
int node_balance(tree_node *n){
   return node_height(n->right) - node_height(n->left);
}

// assumes n is not NULL and n's children have the correct height.
void update_height(tree_node *n){
   int lh = node_height(n->left);
   int rh = node_height(n->right);
   int max = lh > rh ? lh : rh;
   n->height = 1 + max;
}

/// Switch key and data for two nodes
void node_switch(tree_node *a, tree_node *b){
   elem_t temp = a->key;
   a->key = b->key;
   b->key = temp;

   temp = a->data;
   a->data = b->data;
   b->data = temp;
}

/// physically switch two subtrees
///
/// One of the trees CAN'T be a child of the other\n
/// al, ar, bl and br can be NULL\n
///
/// \verbatim
/// +-----+    +-----+
/// |  a  |    |  b  |
/// +-----+    +-----+
/// |al|ar|    |bl|br|
/// +-----+    +-----+
/// \endverbatim
///
/// example usage to switch al and br;\n
/// \code
/// node_cut(&a->l, a, &b->r, b);
/// \endcode
///
/// \param a pointer to a node-pointer to switch with b
/// \param a_par the parent of subtree a
/// \param b pointer to a node-pointer to switch with a
/// \param b_par parent to b
void node_cut(tree_node **a, tree_node **b){
   tree_node *temp = *a;
   *a = *b;
   *b = temp;
   if(*a != NULL){
      // (*a)->parent = a_par;
   }
   if(*b != NULL){
      // (*b)->parent = b_par;
   }
}

/// physically swaps the children of \p n
///
/// special case of node_cut()
/// \param n the node to swap children on
void node_swap_children(tree_node *n){
   node_cut(&n->left, &n->right);
}

/// rotate a subtree either left or right
///
/// \p n MUST have a right child if rotating left, or it MUST have a left child if rotating right.\n
/// The heights will only be updated up to \p n.\n
/// After the rotation, \p n will still point to the root where the rotation originally started.\n
/// \n
/// Because of the usage of node_switch() some nodes might contain different information than before.
///
/// \param n the subtree to rotate
/// \param dir ROTATE_LEFT or ROTATE_RIGHT
void tree_rotate(tree_node *n, enum tree_rotate_dir dir){
   tree_node *a = n;
   tree_node *b = dir == ROTATE_LEFT ? n->right : n->left;
   node_switch(a, b);
   node_swap_children(a);
   node_swap_children(b);
   if(dir == ROTATE_LEFT){
      node_cut(&a->right, &b->left);
   }else{
      node_cut(&a->left, &b->right);
   }
   update_height(b);
   update_height(a);
}

// balance and update heights all the way to the root
void balance(tree_node *n){
   update_height(n);
   int bf = node_balance(n);
   if(bf < -1){
      if(node_balance(n->left) > 0){
         tree_rotate(n->left, ROTATE_LEFT);
      }
      tree_rotate(n, ROTATE_RIGHT);
   }else if(bf > 1){
      if(node_balance(n->right) < 0){
         tree_rotate(n->right, ROTATE_RIGHT);
      }
      tree_rotate(n, ROTATE_LEFT);
   }
   // if(n->parent != NULL) balance(n->parent);
}

// bool tree_insert(tree_t* tree, tree_key_t key, elem_t elem){
//    tree_node *parent = NULL;
//    tree_node **spot = find_node(tree, &(tree->root), &parent, key);
//    if(*spot == NULL){
//       *spot = new_node(tree->key_copy(key), tree->ele_copy(elem));
//       balance(*spot);
//       return true;
//    }else{
//       return false;
//    }
// }

bool tree_insert_aux(tree_t *tree, tree_node **t, tree_key_t key, elem_t elem){
   if(*t == NULL){
      *t = new_node(tree->key_copy(key), tree->ele_copy(elem));
      balance(*t);
      return true;
   }

   int comp = tree->compare(key, (*t)->key);
   bool back = false;

   if(comp < 0){
      back = tree_insert_aux(tree, &(*t)->left, key, elem);
   }else if(comp > 0){
      back = tree_insert_aux(tree, &(*t)->right, key, elem);
   }else{
      return false;
   }

   if(back){
      balance(*t);
   }
   return back;
}

bool tree_insert(tree_t* tree, tree_key_t key, elem_t elem){
   return tree_insert_aux(tree, &tree->root, key, elem);
}

void tree_node_keys(tree_node *n, tree_key_t *arr, int *i){
   if(n == NULL) return;
   tree_node_keys(n->left, arr, i);
   arr[*i] = n->key;
   (*i)++;
   tree_node_keys(n->right, arr, i);
}


tree_key_t* tree_keys(tree_t *tree){
   int size = tree_size(tree);
   if(size == 0) return NULL;
   tree_key_t *arr = malloc(sizeof(tree_key_t) * size);
   int i = 0;
   tree_node_keys(tree->root, arr, &i);
   return arr;
}

void tree_node_elements(tree_node *n, tree_key_t *arr, int *i){
   if(n == NULL) return;
   tree_node_elements(n->left, arr, i);
   arr[*i] = n->data;
   (*i)++;
   tree_node_elements(n->right, arr, i);
}


elem_t* tree_elements(tree_t *t){
   int size = tree_size(t);
   if(size == 0) return NULL;
   elem_t *arr = malloc(sizeof(elem_t) * size);
   int i = 0;
   tree_node_elements(t->root, arr, &i);
   return arr;
}

bool tree_get(tree_t *tree, tree_key_t key, elem_t *result){
   tree_node **spot = find_node(tree, &(tree->root), NULL, key);
   if(*spot == NULL){
      return false;
   }else{
      *result = (*spot)->data;
      return true;
   }
}

bool tree_has_key(tree_t *tree, tree_key_t key){
   return *find_node(tree, &(tree->root), NULL, key) != NULL;
}

//╼├└│─
// void printTree(tree_node* t, linkedlist* p, int last){
//    char* pre = strbuilder_toString(p);
//    printf("%s", pre);
//    free(pre);
//    if(last){
//       printf("└──");
//    }else{
//       printf("├──");
//    }

//    if(t == NULL){
//       printf(">\n");
//    }else{
//       printf("%d\n", t->n);
//       if(last){
//          strbuilder_pushString(p, "   ");
//       }else{
//          strbuilder_pushString(p, "│  ");
//       }
//       printTree(t->right, p, 0);
//       printTree(t->left, p, 1);
//       for(int i = 0; i < 3; i++){
//          strbuilder_popChar(p);
//       }
//    }
// }

// void tree_node_printTree(tree_node* t){
//    linkedlist* prefix = linkedlist_new();
//    printTree(t, prefix, 1);
//    linkedlist_free(prefix, NULL);
// }

tree_node** findMax(tree_node** t){
   tree_node** cur = t;
   while((*cur)->right != NULL){
      cur = &(*cur)->right;
   }
   return cur;
}

//probably correct
tree_node* delete(tree_node** t){
   tree_node* node = *t;
   if(node->left != NULL && node->right != NULL){
      tree_node** ip = findMax(&(*t)->left); //inorder predecessor
      node_switch(*ip, node);
      node = delete(ip);
   }else if(node->left != NULL){
      *t = node->left;
      // (*t)->parent = node->parent;
      node->left = NULL;
      balance(node); //uppdaterar den borttagna också, men det gör inget
   }else if(node->right != NULL){
      *t = node->right;
      // (*t)->parent = node->parent;
      node->right = NULL;
      balance(node);
   }else{
      *t = NULL;
      balance(node);
   }
   // node->parent = NULL;
   return node;
}

bool tree_remove(tree_t* tree, tree_key_t key, elem_t *info){
   tree_node *par = NULL;
   tree_node **found = find_node(tree, &tree->root, &par, key);
   if(*found == NULL){
      return false;
   }
   tree_node* d = delete(found);
   if(info != NULL){
      info[0] = d->key;
      info[1] = d->data;
   }
   free(d);
   return true;
}

bool tree_apply_node(tree_node *node, enum tree_order order, key_elem_apply_fun fun, void *data){
   if(node == NULL){
      return false;
   }
   bool b = false;
   if(order == inorder){
      b = tree_apply_node(node->left, order, fun, data) || b;
      b = fun(node->key, node->data, data) || b;
      b = tree_apply_node(node->right, order, fun, data) || b;
   }else if(order == preorder){
      b = fun(node->key, node->data, data) || b;
      b = tree_apply_node(node->left, order, fun, data) || b;
      b = tree_apply_node(node->right, order, fun, data) || b;
   }else if(order == postorder){
      b = tree_apply_node(node->left, order, fun, data) || b;
      b = tree_apply_node(node->right, order, fun, data) || b;
      b = fun(node->key, node->data, data) || b;
   }
   return b;
}

bool tree_apply(tree_t *tree, enum tree_order order, key_elem_apply_fun fun, void *data){
   return tree_apply_node(tree->root, order, fun, data);
}

bool tree_node_check_heights(tree_node *n){
   if(n == NULL) return true;
   if(is_leaf(n)) return n->height == 1;
   int old = n->height;
   update_height(n);
   if(n->height != old){
      return false;
   }else{
      return tree_node_check_heights(n->left) && tree_node_check_heights(n->right);
   }
}

bool tree_node_check_pointers(tree_node *n){
   if(n == NULL) return true;
   bool res = true;
   if(n->left != NULL){
      // res = res && n->left->parent == n;
   }

   if(n->right != NULL){
      // res = res && n->right->parent == n;
   }

   return res && tree_node_check_pointers(n->right) && tree_node_check_pointers(n->left);
}

bool tree_node_check_balance(tree_node *n){
   if(n == NULL) return true;
   return abs(node_balance(n)) <= 1 && tree_node_check_balance(n->left) && tree_node_check_balance(n->right);
}

bool tree_node_check_keys(tree_t *t, tree_node *n){
   if(n == NULL) return true;
   bool res = true;
   if(n->left != NULL){
      res = res && t->compare(n->key, n->left->key) > 0;
   }
   if(n->right != NULL){
      res = res && t->compare(n->key, n->right->key) < 0;
   }
   return res && tree_node_check_keys(t, n->left) && tree_node_check_keys(t, n->right);
}

bool tree_check_all(tree_t *tree){
   bool b = tree_node_check_heights(tree->root);
   if(!b) {
      printf("heights\n");
      return b;
   }
   b = b && tree_node_check_pointers(tree->root);
   if(!b) {
      printf("pointers\n");
      return b;
   }
   b = b && tree_node_check_balance(tree->root);
   if(!b) {
      printf("balance\n");
      return b;
   }
   b = b && tree_node_check_keys(tree, tree->root);
   if(!b) {
      printf("keys\n");
      return b;
   }
   return b;
}
