#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "si_test.h"
#include "gc_test.h"
#include "hm_test.h"


int main(int argc, char **argv) {
    CU_initialize_registry();

    // Add some test suites
    add_si_test_suites();
    add_gc_test_suites();
    add_hm_test_suites();

    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
