#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "interface.h"
#include "../tree/tree.h"
#include "../list/list.h"
#include "../database/database.h"
#include "../list/list_iterator.h"
#include "../utils/utils.h"
#include <string.h>

void print_menu(void)
{
    fprintf(stdout, "\
\n\
[A]dd an item\n\
[R]emove an item\n\
[E]dit an item\n\
[U]ndo last change\n\
[P]rint database\n\
[S]ave to file\n\
[L]oad from file\n\
E[x]it\n\
\n");
}

void edit_menu(void) {

    fprintf(stdout, "\
\n\
[N]ame\n\
[D]escription\n\
[P]rice\n\
[C]hange shelf\n\
[A]bort\n\
");

}

bool valid_choice(char c, char *valid){
    while(*valid != '\0'){
        if(*valid == c){
            return true;
        }
        valid++;
    }
    return false;
}

char ask_question_menu(void){

    char *input;
    print_menu();
    bool valid = false;
    do
    {
        input = ask_question_string("Enter a valid input: ");
        valid = valid_choice(*input, "AaRrEeUuPpXxSsLl");

        if(!valid){
            printf("%s was wrong input. Try again!\n", input);
        }
    }
    while (!valid);
    char first = *input;
    return first;
}

void ask_desc(char **desc){
    *desc = ask_question_string("Description of product: ");
}

void ask_price(int *price){
    while(true) {
        *price = ask_question_int("Price of product: ");
        if(*price < 0){
            printf("Enter a positive price.\n");
        }
        else{
            break;
        }
    }
}

void ask_name(char **name){
    *name = ask_question_string("Name of product: ");
}

void ask_shelf(char **shelf, int *amount){
    bool running = true;
    while(running){
        *shelf = ask_question("Shelf: ", is_shelf, (convert_func) h_strdup_g).s;
        if(db_is_valid_shelf_name(*shelf) == false){
            printf("Invalid shelf name.\n");
        }
        while(true){
            if(*amount > 0){
                break;
            }
            else{
                *amount = ask_question_int("Enter how many: ");
                if(*amount < 0){
                    printf("Invalid amount.\n");
                }
                else{
                    break;
                }
            }
        }
        running = false;
    }
}

void ask_shelf_delta(char **shelf, int *amount){
   bool running = true;
   while(running){
      *shelf = ask_question("Shelf: ", is_shelf, (convert_func) h_strdup_g).s;
      if(db_is_valid_shelf_name(*shelf) == false){
         printf("Invalid shelf name.\n");
      }
      *amount = ask_question_int("Enter delta: ");
      running = false;
   }
}

void add_item(heap_t *heap, database_t* database){
    char *name;
    char *description;
    int price = 0;
    int amount = 0;
    char *shelf;

    ask_name(&name);
    if(db_item_exists(database, name))
    {
        printf("Item already exists, using old description and price\n");
    }
    else
    {
        ask_desc(&description);
        ask_price(&price);

        db_add_item(heap, database, name, description, price);
    }

    db_undo_enable(database, false);
    bool running = true;
    while(running){
       ask_shelf(&shelf, &amount);
       enum db_error err = db_put_item_on_shelf(heap, database, name, shelf, amount);
       if(err == DB_SHELF_BUSY){
          printf("Det ligger redan något på %s, försök igen ty\n", shelf);
          amount = 0;
       }else{
          running = false;
       }
    }

    db_undo_enable(database, true);

}

void print_shelves(elem_t *shelves, int amount){

    for (int i = 0; i < amount; i++){
        printf("Shelf: %s, Amount: %d\n", db_shelf_get_name(shelves[i]), db_shelf_get_amount(shelves[i]));
    }
}

void print_product(heap_t *heap, database_t *database, const char* item){
    printf("Name: %s\n", item);
    const char *desc;
    db_get_item_desc(database, item, &desc);
    printf("Description: %s\n", &(*desc));
    int price = 0;
    db_get_item_price(database, item, &price);
    printf("Price: %d\n", price);
    elem_t *shelves;
    int amount = 0;
    db_get_shelves(heap, database, item, &shelves, &amount);
    print_shelves(shelves, amount);
}

int list_lines_prompt(bool end, int size){
    char buf[3];
    while(true){
        printf("A: abort,%s 1-%d : ", end ? "" : " n: next,", size);
        //fflush(stdout);
        read_string(buf, 3);
        if(is_number(buf)){
            int a = atoi(buf);
            if(a < 0 || a > size){
                printf("Invalid choice, pick a number between 1 and %d\n", size);
            }else{
                return a;
            }
        }else if(is_one_char(buf)){
            int c = buf[0];
            c = toupper(c);
            if(c == 'A'){
                return -1;
            }else if(c == 'N' && !end){
                return -2;
            }else{
                printf("Invalid choice, %sa, not %c\n", end ? "only " : "n or ", c);
            }
        }else{
            printf("Invalid choice. Enter a number between 1 and %d\n, or a single character [A] or [N].", size);
        }
    }
}

int list_lines(heap_t *heap, database_t *database, bool end, int length){

  const char **item_names = db_get_all_items(heap, database, &length);

    char *end_p = "======end of stuff======\n";
    int i = 0;
    bool running = true;
    end = false;
    while(running){
        int j;
        for(j = 0; j < length; j++){
            if(i >= length){
                break;
            }
            else{
                printf("%d. %s\n",j + 1,&(*item_names[i]));
            }

            if(i == length - 1){
                end = true;
                printf("%s", end_p);
            }
            i++;
        }
        int answer = list_lines_prompt(end, length);
        if(answer == -1){
            running = false;
        }
        else if(answer == -2){
            if(end){
                printf("%s", end_p);
            }
        }
        else{
            return i-(j- answer + 1);
        }
    }
    return -1;
}

char* print_items(heap_t *heap, database_t *database){
    int length = 0;
    const char **item_names = db_get_all_items(heap, database, &length);
    int result = 0;

    if(length <= 0){
        printf("There are no items in the database.\n");
        return NULL;
    }
    else{
      result = list_lines(heap, database, false, length);
        char *ret;
        if(result == -1){
           ret = NULL;
        }else{
            ret = h_strdup(heap, item_names[result]);
           print_product(heap, database, ret);
        }
        return ret;
    }
}

void edit_items(heap_t *heap, database_t *database){
   char *item;
   item = print_items(heap, database);

   bool running = true;
   if(item != NULL){
      edit_menu();
      while(running){
         char answer = ask_question_char("\nWhat would you like to do?");

         if(valid_choice(answer, "CcPpDdAaNn") == false){
            printf("Invalid choice of input. Try again!");
         }
         else{
            if(toupper(answer) == 'D'){
               char *desc = ask_question_string("Enter a new description: ");
               db_set_item_desc(heap, database, item, desc);
               running = false;
            }
            else if(toupper(answer) == 'P'){
               int price = ask_question_int("Enter a new price: ");
               db_set_item_price(heap, database, item, price);
               running = false;
            }
            else if(toupper(answer) == 'N'){
               char *new_name = ask_question_string("Enter a new name: ");
               db_set_item_name(heap, database, item, new_name);
               running = false;
            }
            else if(toupper(answer) == 'A'){
               printf("Abort. Returning to the menu!");
               return;
            }
            else if(toupper(answer) == 'C'){

               int new_delta;
               char *shelf;
               ask_shelf_delta(&shelf, &new_delta);
               if(new_delta > 0){
                 db_put_item_on_shelf(heap, database, item, shelf, new_delta);
               }else if(new_delta < 0){
                 db_remove_item_from_shelf(heap, database, shelf, -new_delta);
               }

               running = false;
            }
            else{
               return; // Should not get here.
            }
         }
      }
   }
   else{
      return;
   }
}

void remove_item(heap_t *heap, database_t *database){
    char *item;
    item = print_items(heap, database);
    if(item == NULL){
       return;
    }
    enum db_error error_message = db_remove_item(heap, database, item);

    if(error_message == DB_INVALID_ITEM_NAME){
        printf("Invalid item name! Returning to the menu.");
        return;
    }
    else if(error_message == DB_ITEM_NO_EXIST){
        printf("The item does not exist! Returning to the menu.");
        return;
    }
    else{
      db_remove_item(heap, database, item);
        return;
    }
}

void undo_item(heap_t *heap, database_t *database){
    char *name;
    enum db_error error_message = db_undo(heap, database, &name);

    if(error_message == DB_NOTHING_TO_UNDO){
        printf("There is nothing to undo!\n");
        return;
    }
    else if(error_message == DB_UNDO_ADD){
        printf("%s was just taken away from the database.", name);
    }
    else if(error_message == DB_UNDO_EDIT){
        printf("The latest change on %s was just regretted.", name);
    }
    else if(error_message == DB_UNDO_REMOVE){
        printf("You regret deleting %s, this action is undone.", name);
    }

}

void save_file(heap_t *heap, database_t *database){
    char *file_name = ask_question_string("Enter a filename: ");
    enum db_error error_message = db_save(heap, database, file_name);

    if(error_message == DB_FILE_ERROR){
    printf("Could not save to file.");
    return;
    }
    else{
    printf("Save: Successfull.");
    return;
    }
}

database_t *load_file(heap_t *heap, database_t *database){
    char *file_name = ask_question_string("Enter a file to load: ");
    database_t *new_db;
    enum db_error error_message = db_load(heap, &new_db, file_name);

    if(error_message == DB_FILE_ERROR){
    printf("Could not load file.");
    return database;
    }
    else{
    printf("Load: Successfull.");
    db_free(database);
    return new_db;
    }
}

void event_loop(heap_t *heap){
    database_t *database = db_new(heap);

    printf("\n");
    printf("Welcome to the warehouse\n");
    printf("==============================\n");
    bool running = true;
    while(running){
        char input = ask_question_menu();
        // char *trash;

        switch(toupper(input)){
            case 'A':
              add_item(heap, database);
                break;
            case 'R':
              remove_item(heap, database);
                break;
            case 'U':
              undo_item(heap, database);
                break;
            case 'E':
              edit_items(heap, database);
                break;
            case 'P':
              print_items(heap, database);
                // if(trash != NULL)
              break;
        case 'S':
          save_file(heap, database);
        break;
        case 'L':
          database = load_file(heap, database);
        break;
            case 'X':
                running = false;
                break;
            default:
                break;
        }
    }
    db_free(database);
}


int main(){
  heap_t *heap = h_init(100*2048, true, 0.15);
  global_heap = heap;
  
    event_loop(heap);

    h_delete(heap);
    return 0;
}
