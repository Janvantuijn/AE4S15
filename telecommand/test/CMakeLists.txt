cmake_minimum_required(VERSION 3.13)

# Find cmocka
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/cmocka/include)
set(CMOCKA_SOURCE
	 ${CMAKE_SOURCE_DIR}/cmocka/src/cmocka.c
	 ${CMAKE_SOURCE_DIR}/cmocka/src/cmocka.def)
	 
# ADD YOUR TEST HERE!!!
LIST(APPEND tests_names "coding_layer_test")


# Declare all tests targets
ENABLE_TESTING()
LIST(LENGTH tests_names count)
MATH(EXPR count "${count} - 1")
FOREACH(i RANGE ${count})
    LIST(GET tests_names ${i} test_name)
    LIST(GET tests_flags ${i} test_flags)
    ADD_EXECUTABLE(${test_name} ${test_name}.c ${CMOCKA_SOURCE})
    ADD_TEST(${test_name} ${test_name})
ENDFOREACH()