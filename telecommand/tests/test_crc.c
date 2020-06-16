#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "crc.h"

void crc_test(void ** state) {

    uint8_t data[10] = {0xAA, 0x00, 0xFD, 0xDE, 0xE0, 0x0A, 0xB0, 0x1D, 0x0B, 0xFF};
    assert_int_equal(crc(data, 10), 0xA2D9); 

    uint8_t data2[10] = {0x01, 0x02, 0x03, 0x04, 0xa0, 0xb0, 0xc0, 0xd0, 0x11, 0x22};
    assert_int_equal(crc(data2, 10), 0xBDD5); 
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
        cmocka_unit_test (crc_test),
    };

    /* If setup and teardown functions are not
       needed, then NULL may be passed instead */

    int count_fail_tests =
        cmocka_run_group_tests (tests, setup, teardown);

    return count_fail_tests;
}
