#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "utils.h"

void clean_input_buffer(){
   int c;
   do{
      c = getchar();
   }while (c != '\n' && c != EOF);
}

int read_string(char* buf, int buf_size){
   int c;
   int i = 0;
   do{
      c = getchar();
      if(i < buf_size-1 && c != '\n'){
         buf[i] = c;
         i++;
      }
   }while(c != '\n' && c != EOF);
   buf[i] = '\0';
   return i;
}

bool is_number(char *str){
   int len = strlen(str);
   bool num = false;
   for(int i = 0; i < len; i++){
      if(str[i] == '-'){
         if(i != 0){
            return false;
         }
      }else if (isdigit(str[i])){
         num = true;
      }else{
         return false;
      }
   }
   return num;
}

bool is_float(char *str){
   int len = strlen(str);
   int dot = false;
   bool num = false;
   for(int i = 0; i < len; i++){
      if(str[i] == '.'){
         if(dot){
            return false;
         }else{
            dot = true;
         }
      }else if(str[i] == '-'){
         if(i != 0){
            return false;
         }
      }else if(isdigit(str[i])){
         num = true;
      }else{
         return false;
      }
   }
   return num;
}

bool is_str_not_empty(char* s){
   return strlen(s) > 0;
}

answer_t ask_question(char *question, check_func check, convert_func convert){
   char buf[255];
   bool running = true;
   while(running){
      printf("%s", question);
      //fflush(stdout);
      read_string(buf, 255);
      if(!check(buf)){
         printf("Incorrect input, try again!\n");
      }else{
         running = false;
      }
   }
   return convert(buf);
}

int ask_question_int(char *question){
   return ask_question(question, is_number, (convert_func) atoi).i;
}

char* ask_question_string(char* question){
   return ask_question(question, is_str_not_empty, (convert_func) strdup).s;
}

float ask_question_float(char * question){
   return ask_question(question, is_float, (convert_func) atof).f;
}

answer_t leading_char(char *s){
   return (answer_t) (int) *s;
}

bool is_one_char(char *s){
   return strlen(s) == 1;
}

char ask_question_char(char *question){
   return (char) ask_question(question, is_one_char, leading_char).i;
}

char* trim(char* str){
   int i = 0;
   int b = -1, e = 0;
   while(str[i] != '\0'){
      if(b == -1 && !isspace(str[i])){
         b = i;
      }
      if(!isspace(str[i])){
         e = i;
      }
      i++;
   }

   int newlen;
   if(b == -1){
      newlen = 1;
   }else{
      newlen = (e-b) + 2;
   }
   char* r = (char*)malloc(sizeof(char) * newlen);
   for(int i = 0; i < newlen-1; i++){
      r[i] = str[b+i];
   }
   r[newlen-1] = '\0';
   return r;
}

bool is_shelf(char *str)
{
   int lengd = strlen(str);
   if (lengd < 2)
   {
      return false;
   }
   for (int i = 0; i <= lengd && str[i] != '\0'; i++)
   {
      if (i == 0)
      {
         if (!isalpha(str[i]))
         {
            return false;
         }
      }
      else
      {
         if (!isdigit(str[i]) && (str[i] != '-'))
         {
            return false;
         }
      }
   }
   return true;
}
