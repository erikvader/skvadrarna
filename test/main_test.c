#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "om_test.h"
#include "si_test.h"
#include "gc_test.h"
#include "hm_test.h"
#include "om_hm_test.h"
#include "gc_event_test.h"

void **test_bot;

int main() {
  void *p = NULL;
  test_bot = &p;
    CU_initialize_registry();

    // Add some test suites
    add_si_test_suites();
    add_gc_test_suites();
    add_hm_test_suites();
    add_om_hm_test_suites();
    add_gc_event_test_suites();

    CU_basic_run_tests();
    CU_cleanup_registry();

    run_om_test();

    return CU_get_error();
}
