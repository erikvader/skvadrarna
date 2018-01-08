#ifndef LAGER2_INTERFACE_H
#define LAGER2_INTERFACE_H

#include <stdbool.h>
#include "../database/database.h"

void print_menu(void);

void edit_menu(void);

bool valid_choice(char c, char *valid);

char ask_question_menu(void);

void ask_desc(char **desc);

void ask_price(int *price);

void ask_name(char **name);

void ask_shelf(char **shelf, int *amount);

void add_item(database_t* database);

void print_shelves(elem_t *shelves, int amount);

void print_product(database_t *database, const char* item);

int list_lines_prompt(bool end, int size);

int list_lines(database_t *database, bool end, int length);

char* print_items(database_t *database);

void edit_items(database_t *database);

void remove_item(database_t *database);

void undo_item(database_t *database);

void event_loop();

#endif //LAGER2_INTERFACE_H
