cmake_minimum_required(VERSION 3.13)
project("unit tests")

set(MAIN_PROJECT ${CMAKE_SOURCE_DIR}/../)
include_directories(${MAIN_PROJECT}/src/coding_layer)


SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

# Find cmocka
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/cmocka/include)
set(CMOCKA_SOURCE
	 ${CMAKE_SOURCE_DIR}/cmocka/src/cmocka.c
	 ${CMAKE_SOURCE_DIR}/cmocka/src/cmocka.def)

set(TEST_SOURCES 
    ${MAIN_PROJECT}/src/coding_layer/cltu.c
    )

# ADD YOUR TEST HERE!!!
LIST(APPEND tests_names "coding_layer_test")

# Declare all tests targets
ENABLE_TESTING()
LIST(LENGTH tests_names count)
MATH(EXPR count "${count} - 1")
FOREACH(i RANGE ${count})
    LIST(GET tests_names ${i} test_name)
    LIST(GET tests_flags ${i} test_flags)
    ADD_EXECUTABLE(${test_name} ${test_name}.c ${CMOCKA_SOURCE} ${TEST_SOURCES})
    ADD_TEST(${test_name} ${test_name})
ENDFOREACH()


# Coverage settings

include(${CMAKE_SOURCE_DIR}/scripts/cmake/CodeCoverage.cmake)
set(Coverage_BASE_DIRECTORY ${MAIN_PROJECT})
setup_target_for_coverage_gcovr_html(
    NAME test_coverage                          # New target name
    EXECUTABLE ctest      # Executable in PROJECT_BINARY_DIR
    BASE_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDENCIES coding_layer_test
    EXCLUDE ${CMAKE_SOURCE_DIR}/cmocka/*
    )