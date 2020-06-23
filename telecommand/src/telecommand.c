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
#include "watchdog.h"
#include "system_config.h"
#include "physical_layer.h"
#include "coding_layer.h"
#include "transfer_layer.h"
#ifdef SATELLITE
	#include "monitor.h"
#else
	#include "ci.h"
#endif
// TODO: insert other definitions and declarations here

#ifdef SATELLITE
    uint8_t data[255];
    uint8_t length = 0;
#endif 

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

    watchdog_init();
    phy_init();
    coding_layer_init();
    transfer_layer_init();
    
#ifdef GROUNDSTATION    
    ci_init();
#else
    monitor_init();
#endif

    while(1) {
    	watchdog_reset();
#ifdef GROUNDSTATION
#warning "Build for ground station"
    	ci_run();

    	//        const uint8_t data[12] = "Hello World!";
//        int16_t frame_number;
//
//    	// Send message
//     	frame_number = transfer_layer_send_message(data, 12);
//
//  	// Check acknowledge
//     	ack_response_t ack = CLCW_NOT_UPDATED;
//    	while (ack == CLCW_NOT_UPDATED) {
//    		ack = transfer_layer_check_ack(frame_number);
//    	}

#else
#warning "Build for satellite"
    	coding_layer_run();
    	transfer_layer_run();

    	int16_t ret = transfer_layer_receive_message(data, &length);
    	if (ret >= 0) {
    		monitor_send(data, length);
    	}
#endif
    }
    return 0 ;
}
