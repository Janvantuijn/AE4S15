/*
===============================================================================
 Name        : telecommand.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "physical_layer.h"
// TODO: insert other definitions and declarations here

cltu_t cltu;

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    phy_init();
    cltu_init(&cltu);
    uint8_t data[2] = {0xBE, 0xEF};
    cltu_insert(&cltu, data, 2);
    request_id_t id = 0;
    while(1) {
        phy_transmit_request(&cltu, id++);
    	phy_run();
        for (volatile unsigned int i = 0; i < 100000; i++) {

        }
    }
    return 0 ;
}
