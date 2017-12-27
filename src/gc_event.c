#include "include/gc_event.h"

// 1: hitta alla chunks som är används
// 2: gå genom stacken och för varje valid pointer:
//   3: markera chunken som unsafe (om vi kom från stacken)
//   4: är det en forwarding-adress (objekt kopierat)?
//     ja: ändra pointern till fwd-adress (om det inte är en unsafe stack-pointer)
//     nej: kopiera objektet och sätt en fwd-adress (om vi inte är i en unsafe chunk)
//          gå till 2, men byt ut 'stacken' mot 'objektet'
// 5: rensa alla chunks som användes innan samlingen (om de inte är unsafe)


void gce_run_gc_event(heap_t *heap){
   
}

