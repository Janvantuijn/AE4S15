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

void cltu_tail_sequence_test( void ** test) {
    cltu_t cltu;
    
    cltu_init(&cltu);

    assert_int_equal(cltu.tail_seq, );
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

void cltu_clear_test (void ** state) {
    cltu_t cltu;

    cltu_init(&cltu);

    cltu_insert(&cltu, "Hallo", 5);
    cltu_clear(&cltu);
    assert_int_equal(cltu.size, 0);
    for (int i = 0; i < MAX_CODE_BLOCKS; i++) {
        cltu.code_blocks[i].size = 0;
    }
}

void cltu_insert_test (void ** state) {
    cltu_t cltu;

    cltu_init(&cltu);
    cltu_insert(&cltu, "", 0);
    assert_int_equal(cltu.size, 0);
    assert_int_equal(cltu_size(&cltu), 3);
    assert_int_equal(cltu_code_block_size(&cltu), 0);

    cltu_insert(&cltu, "Hoi", 3);
    assert_int_equal(cltu.size, 1);
    assert_int_equal(cltu_size(&cltu), 7);
    assert_int_equal(cltu_code_block_size(&cltu), 4);

    cltu_clear(&cltu);

    cltu_insert(&cltu, "Hallo", 5);
    assert_int_equal(cltu.size, 2);
    assert_int_equal(cltu_size(&cltu), 10);
    assert_int_equal(cltu_code_block_size(&cltu), 7);
}

void cltu_get_data_test (void ** state) {
    cltu_t cltu;
    uint8_t data;

    cltu_init(&cltu);
    cltu_insert(&cltu, "", 0);
    assert_false(cltu_get_data(&cltu, 0, &data));
    assert_false(cltu_get_data(&cltu, 1, &data));
    assert_false(cltu_get_data(&cltu, 7, &data));

    cltu_insert(&cltu, "Hoi", 3);
    assert_true(cltu_get_data(&cltu, 0, &data));
    assert_int_equal(data, 'H');
    assert_true(cltu_get_data(&cltu, 1, &data));
    assert_int_equal(data, 'o');
    assert_true(cltu_get_data(&cltu, 2, &data));
    assert_int_equal(data, 'i');
    assert_false(cltu_get_data(&cltu, 3, &data));
    assert_false(cltu_get_data(&cltu, 7, &data));

    cltu_clear(&cltu);

    cltu_insert(&cltu, "Hello World!", 12);
    assert_true(cltu_get_data(&cltu, 0, &data));
    assert_int_equal(data, 'H');
    assert_true(cltu_get_data(&cltu, 1, &data));
    assert_int_equal(data, 'e');
    assert_true(cltu_get_data(&cltu, 2, &data));
    assert_int_equal(data, 'l');
    assert_true(cltu_get_data(&cltu, 3, &data));
    assert_int_equal(data, 'l');
    assert_true(cltu_get_data(&cltu, 4, &data));
    assert_int_equal(data, 'o'); 
    assert_true(cltu_get_data(&cltu, 5, &data));
    assert_int_equal(data, ' ');     
    assert_true(cltu_get_data(&cltu, 6, &data));
    assert_int_equal(data, 'W'); 
    assert_true(cltu_get_data(&cltu, 7, &data));
    assert_int_equal(data, 'o');               
    assert_true(cltu_get_data(&cltu, 8, &data));
    assert_int_equal(data, 'r');                
    assert_true(cltu_get_data(&cltu, 9, &data));
    assert_int_equal(data, 'l');                
    assert_true(cltu_get_data(&cltu, 10, &data));
    assert_int_equal(data, 'd');                            
    assert_true(cltu_get_data(&cltu, 11, &data));
    assert_int_equal(data, '!');                                
    assert_false(cltu_get_data(&cltu, 12, &data));
    assert_false(cltu_get_data(&cltu, 24, &data));
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
        cmocka_unit_test (cltu_clear_test),
        cmocka_unit_test (cltu_insert_test),
        cmocka_unit_test (cltu_get_data_test),
        cmocka_unit_test (cltu_tail_sequence_test),
       
    };

    /* If setup and teardown functions are not
       needed, then NULL may be passed instead */

    int count_fail_tests =
        cmocka_run_group_tests (tests, setup, teardown);

    return count_fail_tests;
}
