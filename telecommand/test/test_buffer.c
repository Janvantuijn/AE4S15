#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "buffer.h"

void buffer_init_test (void ** state) {
    buffer_t buffer;
    uint8_t arr[BUFFER_SIZE];

    buffer_init(&buffer, arr);
    assert_int_equal(buffer.array,  arr);
    assert_int_equal(buffer.front,  0);
    assert_int_equal(buffer.size,  0);
    assert_int_equal(buffer.rear,  -1);
}

void buffer_peek_test(void ** state) {
    buffer_t buffer;
    uint8_t arr[BUFFER_SIZE] = {0x2};

    buffer_init(&buffer, arr);
    assert_int_equal(buffer_peek(&buffer), arr[0]);

    buffer_insert(&buffer, 0xFF);
    assert_int_equal(buffer_peek(&buffer), 0xFF);

    for (int i = 0; i < BUFFER_SIZE + 10; i++) {
        buffer_insert(&buffer, i);
    }
    assert_int_equal(buffer_peek(&buffer), 0xFF);
}

void buffer_is_empty_test(void ** state) {
    buffer_t buffer;
    uint8_t arr[BUFFER_SIZE];

    buffer_init(&buffer, arr);
    assert_true(buffer_is_empty(&buffer));

    buffer_insert(&buffer, 0xFF);
    assert_false(buffer_is_empty(&buffer));

    for (int i = 0; i < BUFFER_SIZE + 10; i++) {
        buffer_insert(&buffer, i);
    }

    assert_false(buffer_is_empty(&buffer));    

    for (int i = 0 ; i < BUFFER_SIZE; i++) {
        buffer_remove(&buffer);
    }
    assert_true(buffer_is_empty(&buffer));
}

void buffer_is_circular(void ** state) {
    buffer_t buffer;
    uint8_t arr[BUFFER_SIZE];

    buffer_init(&buffer, arr);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer_insert(&buffer, i % 255);
        assert_int_equal(buffer_remove(&buffer), i % 255);
    }

    assert_true(buffer_is_empty(&buffer));
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer_insert(&buffer, i % 255);
    }
    assert_true(buffer_is_full(&buffer));

    assert_int_equal(buffer_remove(&buffer), 0);

    buffer_insert(&buffer, 10);
    assert_true(buffer_is_full(&buffer));
}

void buffer_is_full_test(void ** state) {
    buffer_t buffer;
    uint8_t arr[BUFFER_SIZE];

    buffer_init(&buffer, arr);
    assert_false(buffer_is_full(&buffer));

    buffer_insert(&buffer, 0xFF);
    assert_false(buffer_is_full(&buffer));

    for (int i = 0; i < BUFFER_SIZE + 10; i++) {
        buffer_insert(&buffer, i);
    }

    assert_true(buffer_is_full(&buffer));    

    for (int i = 0 ; i < BUFFER_SIZE; i++) {
        buffer_remove(&buffer);
    }
    assert_false(buffer_is_full(&buffer));
}

void buffer_clear_test (void ** state) {
    buffer_t buffer;
    uint8_t arr[BUFFER_SIZE];

    buffer_init(&buffer, arr);
    assert_false(buffer_is_full(&buffer));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer_insert(&buffer, i);
    }
    assert_true(buffer_is_full(&buffer));    

    buffer_clear(&buffer);
    assert_true(buffer_is_empty(&buffer));
    assert_false(buffer_is_full(&buffer));

    assert_int_equal(buffer.size, 0);
    assert_int_equal(buffer.front, 0);
    assert_int_equal(buffer.rear, -1);

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
        cmocka_unit_test(buffer_init_test),
        cmocka_unit_test(buffer_peek_test),
        cmocka_unit_test(buffer_is_empty_test),
        cmocka_unit_test(buffer_is_full_test),
        cmocka_unit_test(buffer_is_circular),
        cmocka_unit_test(buffer_clear_test)
    };

    /* If setup and teardown functions are not
       needed, then NULL may be passed instead */

    int count_fail_tests =
        cmocka_run_group_tests (tests, setup, teardown);

    return count_fail_tests;
}
