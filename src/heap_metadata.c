#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef bool bitarr_t; 

typedef struct heap_header{
  // the start of where the free space is at init
  void *heap_start;
  // size of the usabe heap, that is, heap_size - header_size
  size_t heap_siz;
  // should always be 2048
  size_t chunk_siz;
  // if the stack is safe or not
  bool unsafe_stack;
  // the threashold for when a gc_event should trigger
  float gc_threshold;
  // the bit_array for which space is used
  bitarr_t *used_arr;
  // arr of pointers fore where the free space begins in every chunk
  void **free_pointers;
} heap_header_t;


 

// creates the metadata object
// creates 3 different structs: heap_header, used_arr and free_pointers
void hm_init(heap_t *heap,size_t size,bool unsafe_stack, float gc_threshold) {
	heap_header_t *heap = (heap_header_t *) calloc(1, heap_header);
	heap -> heap_start = heap;
   	heap -> heap_siz = size;
	heap -> chunk_siz = 2048;
	heap -> unsafe_stack = unsafe_stack;
	heap -> gc_threshhold = gc_threshhold; 	
	//heap -> used_arr;
	//heap -> free_pointers;
}



void *hm_get_free_space(heap_t *heap,size_t obj_siz) //TODO: Must work with mutiple objects in same chunk.
{
	void *free_space = heap;
	size_t head_size = sizeof(heap_header);
	free_space = heap + head_size; //Moves pointer to the start of the first chunk;
	heap_header_t *head = (heap_header_t*) heap;
	for (int i = 0; i < hm_get_amount_chunks(heap); i ++) 
	{
		if ((head -> free_pointers)[i] == free_space) 
	  	{
		  return free_space;
		}
		free_space = free_space + (heap -> chunk_siz);
	}
	puts("No free space available :(\n");
	return NULL;
}


void *hm_alloc_spec_chunk(heap_t *heap, size_t obj_siz, chunk_t index)
{
	void *free_space = heap;
	size_t head_size = sizeof(heap_header);
	free_space = heap + head_size; //Moves pointer to the start of the first chunk
	heap_header_t *head = (heap_header_t*) heap;//So we're able to use header metadata
	free_space = free_space + (head -> chunk_siz)*index;
	if (free_space == (head -> free_pointers)[index - 1])
	{
	  return free_space;
	}
	puts("Chunk is not free :( \n");
	return NULL;
}

bool hm_over_threshold(heap_t *heap)
{
	return true;		
}

size_t hm_size_available(heap_t *heap) //
{
	size_t free_space;
	heap_header_t *head = (heap_header_t*) heap;
	free_space = (head -> heap_siz) - hm_get_amount_chunks(heap) * (heap -> chunk_siz); //2048 should be replaced with size of object allocated
   	return free_space;
}	

size_t hm_size_used(heap_t *heap);
{
	size_t used_space;
	heap_header_t *head = (heap_header_t*) heap;
	used_space = hm_get_amount_chunks(heap) * (heap -> chunk_siz);
	return used_space;
}

bool hm_pointer_exists(heap_t *heap, void *pointer);

	
	

int hm_get_amount_chunks(heap_t *heap);

chunk_t hm_get_pointer_chunk(heap_t *heap, void *pointer);

void hm_reset_chunk(heap_t *heap, chunk_t index);

void hm_get_used_chunks(heap_t *heap, bool *data);
