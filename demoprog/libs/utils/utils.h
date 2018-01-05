#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

typedef union{
   int i;
   float f;
   char* s;
} answer_t;

typedef bool(*check_func)(char*);
typedef answer_t(*convert_func)(char*);

bool is_number(char *str);
bool is_float(char *str);
bool is_str_not_empty(char*);
bool is_one_char(char*);
bool is_shelf(char*);

// Reads string from stdin into buf with size buf_siz.
// Input that doesn't fit in buf will be discarded.
// buf will be null-terminated
int read_string(char *buf, int buf_siz);

// prints question to stdout and read input from stdin.
// 'check' is used to check whether the read string is valid.
// if it is valid, then it is converted with 'convert'.
answer_t ask_question(char *question, check_func check, convert_func convert);

// special forms of ask_question
int ask_question_int(char *question);
float ask_question_float(char *question);
char *ask_question_string(char *question);
char ask_question_char(char*);

// trims whitespace
// returns a pointer to a new string string on the heap
char* trim(char*);
#endif
