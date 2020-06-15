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
#include "coding_layer.h"
#include "transfer_layer.h"
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


    const uint8_t data[12] = "Hello World!";
    int16_t frame_number;

    phy_init();
    coding_layer_init();

    while(1) {
    	// Send message
     	frame_number = transfer_layer_send_message(data, 12);

//     	// Check acknowledge
//     	ack_response_t ack = CLCW_NOT_UPDATED;
//    	while (ack == CLCW_NOT_UPDATED) {
//    		ack = transfer_layer_check_ack(frame_number);
//    	}

    	// Wait some time
        for (volatile unsigned int i = 0; i < 100000; i++) {

        }
    }
    return 0 ;
}
