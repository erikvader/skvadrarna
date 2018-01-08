#include "tree.h"
#include <stdlib.h>
#include <time.h>

int main(){
   tree_t *t = tree_new(NULL, NULL, NULL, NULL, NULL);

   srand(1337);

   for(int i = 0; i < 1000000; i++){
      int r = rand();
      tree_insert(t, (elem_t)r, (elem_t)r);
   }

   for(int i = 0; i < 1000000; i++){
      int r = rand();
      tree_has_key(t, (elem_t)r);
   }

   // tree_delete(t, false, false);

   return 0;
}

