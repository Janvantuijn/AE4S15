#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "physical_layer/physical_layer.c"

void phy_transmit_request_test(void ** state) {
    cltu_t cltu1, cltu2;
    cltu_init(&cltu1);
    cltu_init(&cltu2);

    request_resp_t resp = phy_transmit_request(&cltu1, 0);
    assert_int_equal(resp, CLTU_ACCEPTED);
    assert_int_equal(phy_cltu_transmitting, &cltu1);
    assert_int_equal(phy_id_transmitting, 0);

    resp = phy_transmit_request(&cltu2, 1);
    assert_int_equal(resp, CLTU_ACCEPTED);
    assert_int_equal(phy_cltu_transmitting, &cltu1);
    assert_int_equal(phy_cltu_pending, &cltu2);
    assert_int_equal(phy_id_transmitting, 0);
    assert_int_equal(phy_id_pending, 1);

    resp = phy_transmit_request(&cltu1, 2);
    assert_int_equal(resp, CLTU_REJECTED);
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
        cmocka_unit_test (phy_transmit_request_test)       
    };

    /* If setup and teardown functions are not
       needed, then NULL may be passed instead */

    int count_fail_tests =
        cmocka_run_group_tests (tests, setup, teardown);

    return count_fail_tests;
}
