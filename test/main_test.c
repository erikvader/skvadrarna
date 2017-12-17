#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

int main(int argc, char **argv) {
    CU_initialize_registry();

    // Add some test suites

    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
