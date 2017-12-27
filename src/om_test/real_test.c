#include <stdio.h>
#include "../include/object_metadata.h"
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

static char* format;
static void* object;
static size_t true_size;
static uint64_t data;
/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init(void)
{
  object = calloc(101,sizeof(void*))+sizeof(void*);
  uint64_t* number = object;
  for (int i = 1; i <= 100; ++i) {
    number[i-1]= i;
  }
    
  om_build(NULL, object,format);
  return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean(void)
{
  free(object-sizeof(void*));
  return 0;
}

/* Simple test of fprintf().
 * Writes test data to the temporary file and checks
 * whether the expected number of bytes were written.
 */
bool test_size(void)
{
  if (om_size(object) == true_size && om_size_format(format) == true_size){
    return true;
  }
  else{
    printf("true_size: %d om_size: %d om_size_format: %d\n", (int)true_size ,(int)om_size(object), (int)om_size_format(format)); 
    return false;
  }
}
bool test_data(void){
  uint64_t* object_data = (object-8);
  if(*object_data == data){
    return true;
  }
  else{
    printf("Expected: %" PRIu64  " Found: %" PRIu64 "\n", data, *object_data);
    return false;
  }
}

void test_pointers(void){
  int amount_pointers = om_amount_pointers(object);
  void** pointerarray[amount_pointers];
  om_get_pointers(object,pointerarray);
  printf("Format_string: %s \n",format);
  printf("Amount_pointers: %d \n", amount_pointers);
  for (int i = 0; i < amount_pointers; ++i) {
    printf("Pointer %d: %"PRIu64"\n",(i+1), *(uint64_t *)pointerarray[i]);
  }

}

bool test_redirect(){
  bool returnvalue = true;
  if(om_has_forwarding(object)){
    printf("New object wrongly has forwarding");
    returnvalue = false;
  }
  if(om_get_forwarding(object)!= NULL){
    printf("No forwarding != NULL");
    returnvalue = false;
  }
  
  uint64_t fakeadress = 1024;
  om_set_forwarding(object, (void*)fakeadress);
  if(!om_has_forwarding(object)){
    printf("After forwarding dont have forwarding");
    returnvalue = false;
  }
  if(((uint64_t)om_get_forwarding(object))!=fakeadress){
    printf("Wrong forwarding adress: %"PRIu64"\n", ((uint64_t)om_get_forwarding(object)) );
    returnvalue = false;
  }
  
  
  return returnvalue;
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main(void)
{


  FILE* fp = fopen("om_size_and_build_tests.txt", "r");
  true_size =0;
  data = 0;
  int testnr =1;
  while(!feof(fp)){
    char buf[512];
    fscanf(fp, "%s %"PRIu64" %"PRIu64"\n", buf, &true_size, &data);
    format = buf;
    init();
    bool sizetest = test_size();
    bool datatest = test_data();
    if(sizetest && datatest){
      printf("Sizetest och datatest %d: OK!\n", testnr);
      
    }
    else{
      printf("Sizetest och datatest %d: FEL!\n", testnr);
    }
    test_pointers();
    if(test_redirect()){
      printf("Redirecttest %d: OK!\n", testnr);
    }else{
      printf("Redirecttest %d: FEL!\n", testnr);
    }
    testnr++;
    clean();
  }
  fclose(fp);

  
  
  return 0;
}


