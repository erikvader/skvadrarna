#include <stdio.h>
#include "libs/utils/utils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "libs/tree/tree.h"
#include "libs/list/list.h"
#include "libs/list/list_iterator.h"



char *main_menu = "\n\
[L]ägga till en vara \n\
[T]a bort en vara \n\
[R]edigera en vara \n\
Ån[g]ra senaste ändringen \n\
Lista [h]ela varukatalogen \n\
[V]isa en vara\n\
[A]vsluta\n\n\
";

char *edit_menu = "\n\
[B]eskrivning \n\
[P]ris\n\
[L]ägg till/redigera lagerhylla\n\
[R]adera lagerhylla\n\n\
";



//the undo action to perform. Upgrade this to a stack to have a longer
//undo history
undo_action_t *last_action = NULL;

// checks whether c is a valid choice for a prompt. 'valid' is a
// string where every character is a valid choice.
bool valid_choice(char c, char *valid){
   while(*valid != '\0'){
      if(*valid == c){
         return true;
      }
      valid++;
   }
   return false;
}

// prompts for a choice in 'menu'.
// 'menu' is a string of a menu that is printed once (before the first prompt).
// 'prompt' is a string for the actual prompt.
// 'valid' is a string of valid choices
//
// EXAMPLE:
// menu = "[d]o stuff\n[o]ther stuff\n[e]xit\n\n"
// prompt = "your choice >> "
// valid = "DdOoEe"
//
// results in:
// 
// [d]o stuff
// [o]ther stuff
// [e]xit
//
// your choice >> |
char normal_prompt(char *menu, char *prompt, char *valid){
   printf("%s", menu);
   char c;
   while(true){
      c = ask_question_char(prompt);
      if(!valid_choice(c, valid)){
         printf("/)_-) det står ju i menyn vad du kan göra...\n");
      }else{
         break;
      }
   }
   return c;
}

// the same as 'list_lines_prompt' except you cant choose a number.
// Good for just listing a bunch of stuff without needing to select
// anything.
int list_lines_prompt_no_num(bool end, int num){
   num++; //pallade inte varningen
   char valid[10];
   char prompt[20];
   sprintf(valid, "qQ%s", end? "" : "nN");
   sprintf(prompt, "q: quit%s :> ", end? "" : " ,n:next");
   char c = normal_prompt("", prompt, valid);
   if(toupper(c) == 'Q'){
      return -1;
   }else if (toupper(c) == 'N') {
      return -2;
   }
   return 0; //borde aldrig komma hit
}

// asks what to do after a printout in list_lines
// -1 -> q
// -2 -> n
// 1-num -> 1-num
//
// num: upper bound. num == 2 means ask for a number between [1, 2]
// end: are there any more items to be printed out? Disables
//      the n (next) option.
int list_lines_prompt(bool end, int num){
   char buf[3];
   while(true){
      printf("q: quit,%s 1-%d :> ", end ? "" : " n: next,", num);
      //fflush(stdout);
      read_string(buf, 3);
      if(is_number(buf)){
         int a = atoi(buf);
         if(a < 0 || a > num){
            printf("n ∊ [1, %d], n ∊ ℕ\n", num);
         }else{
            return a;
         }
      }else if(is_one_char(buf)){
         int c = buf[0];
         if(c == 'q'){
            return -1;
         }else if(c == 'n' && !end){
            return -2;
         }else{
            printf("asså... %sq, inte %c\n", end ? "bara " : "n eller ", c);
         }
      }else{
         printf("Det ska vara en enstaka char eller en siffra mellan 1 och %d\n", num);
      }
   }
}

// prints 20 things at a time from the array t
// returns index to the selected string
// returns -1 if aborted
// Amount of strings CANNOT be 0, check for this beforehand
//
//           t: array of strings to be printed
//       t_len: length of t
// prompt_func: either 'list_lines_prompt' or 'list_lines_prompt_no_num'
int list_lines(char **t, int t_len, int(*prompt_func)(bool end, int num)){
   char *end_p = "======end of stuff======\n";
   int i = 0;
   bool end = false;
   bool running = true;
   while(running){
      printf("\n");
      int j;
      for(j = 0; j < 20; j++){
         if(i >= t_len){
            break;
         }else{
            printf("%d: %s\n", j+1, t[i]);
         }

         if(i == t_len -1){
            end = true;
            printf("%s", end_p);
         }
         i++;
      }

      int ans = prompt_func(end, j);
      if(ans == -1){ //q
         running = false;
      }else if(ans == -2){ //n
         if(end){
            printf("%s", end_p);
         }
      }else{
         return i-(j-ans+1);
      }
   }
   return -1;
}


// prints all shelves in l in a nice way
void print_shelves(list_t *l){
   list_iterator_t *ite = list_get_iterator(l);
   printf("hyllor: ");
   bool first = true;
   while(list_iterator_has_next(ite)){
      if(first){
         first = false;
      }else{
         printf(", ");
      }
      shelf_t *s = (shelf_t*) *list_iterator_next(ite);
      printf("(hylla: %s, antal: %d)", s->name, s->num);
   }
   printf("\n");
   free(ite);
}

//prints item in a neat fancy way
void print_item(item_t *i){
   printf("Namn: %s\nBeskrivning: %s\nPris: %d.%d kr\n",
          i->name,
          i->desc,
          i->price/100,
          i->price%100);
   print_shelves(i->shelves);
}



// fråga hyllplats -> kolla ifall den används -> om den gör det, fråga ny eller avbryt
// returnar true om den ändrade på något.
bool add_shelf(item_t *item, tree_t *tree){
   bool running = true;
   while(running){
      char *h = ask_question("Hyllplats: ", is_shelf, (convert_func) strdup).s;
      char *item_using = shelf_in_use(tree, h);
      if(item_using == NULL){
         int num;
         printf("Det finns ingenting på %s!\n", h);
         printf("Hur mycket vill du lägga där?\n");
         while(true){
            num = ask_question_int("Antal : ");
            if(num < 0){
               printf("Är %s gjord av antimateria eller?\n", item->name);
            }else{
               break;
            }
         }
         list_append(item->shelves, make_shelf(h, num));
         running = false;
         return true;
      }else if(strcmp(item_using, item->name) == 0){ //borde kunna jämföra pekare bara
         printf("Du har redan %s här\n", item->name);
         printf("--Man skulle kunna ändra antalet %s här, men palla implementera det!--\n", item->name);
         running = false;
         free(h);
      }else{
         printf("Det finns %s på %s redan\n", item_using, h);
         char choice = normal_prompt("\n", "[A]vbryt, [T]esta igen : ", "AaTt");
         if(toupper(choice) == 'A'){
            running = false;
         }
         free(h);
      }
   }
   return false;
}

// Lists all shelves in item, prompts the user for one to be removed.
void remove_shelf(item_t *item){
   list_t *shelves = item->shelves;
   if(list_length(shelves) == 0){
      printf("Finns inget att ta bort ju\n");
      return;
   }
   char** names = malloc(sizeof(char*)*(list_length(shelves)));
   int i = 0;
   list_iterator_t *ite = list_get_iterator(shelves);
   while(list_iterator_has_next(ite)){
      names[i] = ((shelf_t*) *list_iterator_next(ite))->name;
      i++;
   }

   int chosen = list_lines(names, list_length(shelves), list_lines_prompt);
   if(chosen != -1){
      void *removed;
      list_remove(shelves, chosen, &removed);
      delete_shelf((shelf_t*) removed);
   }

   free(ite);
   free(names);
}

// Prompts the user to directly edit 'item'.
// track_undo: whether or not to add an undo_action.
void edit_chosen_item(item_t *item, tree_t *tree, bool track_undo){
   bool made_change = false;
   item_t *original = NULL;
   if(track_undo) original = copy_item(item);
   bool asking = true;
   while(asking){
      printf("\n");
      print_item(item);
      char input = normal_prompt(edit_menu, "välj rad eller [a]vbryt: ", "aABbPpLlRr");
      switch(toupper(input)){
      case 'B':
         printf("\nNuvarande beskrivning: %s\n-----------------------------\n", item->desc);
         free(item->desc);
         item->desc = ask_question_string("New beskrivning: ");
         made_change = true;
         break;
      case 'P':
         printf("\nNuvarande pris: %d\n-----------------------------\n", item->price);
         item->price = ask_question_int("new Pris i ören: ");
         made_change = true;
         break;
      case 'R':
         remove_shelf(item);
         made_change = true;
         break;
      case 'L':
         made_change = add_shelf(item, tree) || made_change;
         break;
      case 'A':
         asking = false;
         break;
      }
   }

   if(made_change && track_undo){
      undo_add_edit(original);
   }
}

// prompts the user to select an item to edit
void edit_items(tree_t *t){
   int size = tree_size(t);
   if (size == 0) {
      printf("Det finns absolut ingenting här!\n");
   }else{
      K *keys = tree_keys(t);
      int choice = list_lines(keys, size, list_lines_prompt);
      if(choice != -1){
         printf("\n");
         edit_chosen_item(tree_get(t, keys[choice]), t, true);
      }
      free(keys);
   }
}

// Prompts the user to add an item.
// asks for name and stuff.
void add_items(tree_t *tree){
   char *name = ask_question_string("Namn på varan: ");
   if(tree_has_key(tree, name)){
      printf("%s finns redan!\navbryter...\n", name);
      //printf("Använder den\n");
      //add_shelf(tree_get(tree, name), tree);
      //alt lägga till ngt här med undo
      free(name);
   }else{
      char *desc = ask_question_string("Beskrivning: ");
      int price;
      while(true){
         price = ask_question_int("Pris i ören: ");
         if(price < 0){
            printf("Är dina pengar gjorda av antimeteria eller?\nknasiga pengar\n");
         }else{
            break;
         }
      }
      item_t *new = make_item(name, desc, price);
      add_shelf(new, tree);

      tree_insert(tree, new->name, new);

      while(true){
         printf("\n");
         print_item(new);
         char input = toupper(normal_prompt("\n", "Are you sure? (Yes/No/Redigera): ", "YyNnRr"));

         if(input == 'N'){
            tree_remove(tree, new->name);
            delete_item(new);
            break;
         }else if(input == 'Y'){
            undo_add_add(name);
            break;
         }else if(input == 'R'){
            edit_chosen_item(new, tree, false);
         }
      }
   }
}

// the same as 'visa_item' except you cant select an item to get extra
// information
void list_all(tree_t *t){
   int size = tree_size(t);
   if (size == 0) {
      printf("Det finns absolut ingenting här!\n");
   }else{
      K *keys = tree_keys(t);
      list_lines(keys, size, list_lines_prompt_no_num);
      free(keys);
   }
}

// prints all items and prompts the user to see extra information
// about a specific item
void visa_item(tree_t *t){
   int size = tree_size(t);
   if (size == 0) {
      printf("Det finns absolut ingenting här!\n");
   }else{
      K *keys = tree_keys(t);
      int choice = list_lines(keys, size, list_lines_prompt);
      if(choice != -1){
         printf("\n");
         print_item(tree_get(t, keys[choice]));
      }
      free(keys);
   }
}

// prompts the user for an item to remove
void remove_items(tree_t *t){
   int size = tree_size(t);
   if (size == 0) {
      printf("Det finns absolut ingenting här!\n");
   }else{
      K *keys = tree_keys(t);
      int choice = list_lines(keys, size, list_lines_prompt);
      if(choice != -1){
         printf("\nDu tog nyss bort %s, nice!\n", keys[choice]);
         T data = tree_remove(t, keys[choice]);
         undo_add_remove(data);
         //delete_item(tree_remove(t, keys[choice]));
      }
      free(keys);
   }
}

// undo the last action.
void undo(tree_t *tree){
   if(last_action == NULL){
      printf("Det finns inget att undoa\n");
   }else{
      undo_undo(tree);
   }
}

// function to be called on every key-element pair, with the purpose
// to free them.
void tree_cleanup(K key, T elem){
   key = key;
   delete_item(elem);
}

// main loop of the program
void event_loop(){
   tree_t *tree = tree_new();

   printf("\n");
   printf("Välkommen till lagerhantering 1.0\n");
   printf("==============================\n");
   bool running = true;
   while(running){
      char input = normal_prompt(main_menu, ">> ", "LlTtRrGgHhAaVv");

      switch(toupper(input)){
      case 'L':
         add_items(tree);
         break;
      case 'H':
         list_all(tree);
         break;
      case 'V':
         visa_item(tree);
         break;
      case 'R':
         edit_items(tree);
         break;
      case 'T':
         remove_items(tree);
         break;
      case 'G':
         undo(tree);
         break;
      case 'A':
         running = false;
         break;
      }
   }

   tree_delete(tree, tree_cleanup);
   if(last_action != NULL){
      undo_free_action(last_action);
   }
}

int main(void){
   event_loop();
   return 0;
}
