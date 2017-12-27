#include "include/gc_event.h"
#include "include/heap_metadata.h"
#include "include/stack_iter.h"

// 1: hitta alla chunks som är används
// 2: gå genom stacken och för varje valid pointer:
//   3: markera chunken som unsafe (om vi kom från stacken)
//   4: är det en forwarding-adress (objekt kopierat)?
//     ja: ändra pointern till fwd-adress (om det inte är en unsafe stack-pointer)
//     nej: kopiera objektet och sätt en fwd-adress (om vi inte är i en unsafe chunk)
//          gå till 2, men byt ut 'stacken' mot 'objektet'
// 5: rensa alla chunks som användes innan samlingen (om de inte är unsafe)

void explore(heap_t *heap, void* obj, bool unsafe, bool *unsafe_chunks){
   
}

void gce_run_gc_event(heap_t *heap){

   int num_chunks = hm_get_amount_chunks(heap);
   
   bool unsafe = hm_is_unsafe(heap);
   bool unsafe_chunks[num_chunks];

   //get locked chunks
   bool locked[num_chunks];
   hm_get_used_chunks(heap, locked);

   //loopa stack
   void* start;
   start = &start;

   while(start != environ){
      void **next = si_next_pointer(heap, &start);
      if(*next != environ){
         explore(heap, *next, unsafe, unsafe_chunks);
      }
   }

   //free chunks
   for(int i = 0; i < num_chunks; i++){
      if(!unsafe_chunks[i] && locked[i]){
         hm_reset_chunk(heap, i);
      }
   }

}

