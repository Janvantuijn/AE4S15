#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "cltu.h"

void cltu_init_test (void ** state) {
    cltu_t cltu;
    
    cltu_init(&cltu);    
    assert_int_equal(cltu.start_seq, CLTU_START_SEQUENCE);
}

void cltu_start_seq_check_succ_test (void ** state) {
    cltu_t cltu;
    
    cltu_init(&cltu);
    assert_true(cltu_start_seq_check(&cltu));
}

void cltu_start_seq_check_fail_test (void ** state) {
    cltu_t cltu;
    
    cltu.start_seq = 0xF0F0;
    assert_false(cltu_start_seq_check(&cltu));
}

/* These functions will be used to initialize
   and clean resources up after each test run */
int setup (void ** state)
{
    return 0;
}

int teardown (void ** state)
{
    return 0;
}


int main (void)
{
    const struct CMUnitTest tests [] =
    {
        cmocka_unit_test (cltu_init_test),
        cmocka_unit_test (cltu_start_seq_check_succ_test),
        cmocka_unit_test (cltu_start_seq_check_fail_test),
       
    };

    /* If setup and teardown functions are not
       needed, then NULL may be passed instead */

    int count_fail_tests =
        cmocka_run_group_tests (tests, setup, teardown);

    return count_fail_tests;
}
