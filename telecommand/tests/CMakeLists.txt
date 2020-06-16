cmake_minimum_required(VERSION 3.13)
project("unit tests")

set(MAIN_PROJECT ${CMAKE_SOURCE_DIR}/../)
include_directories(${MAIN_PROJECT}/inc)
include_directories(${MAIN_PROJECT}/src)
include_directories(${MAIN_PROJECT}/../lpc_chip_11cxx_lib/inc)
include_directories(${MAIN_PROJECT}/../nxp_lpcxpresso_11c24_board_lib/inc)
add_definitions(-DCORE_M0 -D__USE_LPCOPEN)

SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

# Find cmocka
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/cmocka/include)
set(CMOCKA_SOURCE
	 ${CMAKE_SOURCE_DIR}/cmocka/src/cmocka.c
	 ${CMAKE_SOURCE_DIR}/cmocka/src/cmocka.def)

set(TEST_SOURCES 
    ${MAIN_PROJECT}/../lpc_chip_11cxx_lib/src/i2c_11xx.c
    ${MAIN_PROJECT}/../lpc_chip_11cxx_lib/src/clock_11xx.c
    ${MAIN_PROJECT}/../lpc_chip_11cxx_lib/src/uart_11xx.c
    ${MAIN_PROJECT}/../lpc_chip_11cxx_lib/src/gpio_11xx_1.c
    ${MAIN_PROJECT}/../lpc_chip_11cxx_lib/src/gpio_11xx_2.c
    ${MAIN_PROJECT}/../lpc_chip_11cxx_lib/src/ring_buffer.c
    ${MAIN_PROJECT}/../nxp_lpcxpresso_11c24_board_lib/src/board.c
    ${MAIN_PROJECT}/src/coding_layer/cltu.c
    ${MAIN_PROJECT}/src/coding_layer/crc.c
    ${MAIN_PROJECT}/src/buffer.c
    )

# ADD YOUR TEST HERE!!!
LIST(APPEND tests_names "test_cltu")
LIST(APPEND tests_names "test_code_block")
LIST(APPEND tests_names "test_buffer")
LIST(APPEND tests_names "test_phy_master")
LIST(APPEND tests_names "test_phy_slave")
LIST(APPEND tests_names "test_crc")

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
    DEPENDENCIES test_cltu test_code_block test_buffer test_phy_master test_phy_slave
    EXCLUDE ${CMAKE_SOURCE_DIR}/cmocka/*
    )