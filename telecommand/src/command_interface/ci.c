/*
 * @brief UART interrupt example with ring buffers
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#if defined (__USE_LPCOPEN)
	#if defined(NO_BOARD_LIB)
		#include "chip.h"
	#else
		#include "board.h"
	#endif
#endif
#include "string.h"
#include "ci.h"
#include "transfer_layer.h"
#include "crc.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static void Init_UART_PinMux(void);
void executeCommand(char *command, char *msg);
static void read_command(char* command);
static unsigned long hash(unsigned char *str);
static void authenticate(char* pw);
/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 128	/* Send */
#define UART_RRB_SIZE 32	/* Receive */
#define MAX_COMMAND_LEN 7

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

const char start_msg[] = "Telecommander CI\r\n";
const char inst2[] = "Use SEND, SEND10, CORRUPT as commands\r\n";
const char incorrect_msg[] = "\r\n Unknown command \r\n";
const char ack_msg[] = "\r\n ACK \r\n";
const char unauthenticated_msg[] = "\r\n Unauthenticated \r\n";
const char authenticated_msg[] = "\r\n Authenticated \r\n";
const char wrong_pw_msg[] = "\r\n Wrong Password \r\n";
static char command[MAX_COMMAND_LEN];
static uint8_t authenticated = 0;
static int index = 0;

/**
 * @brief	Main UART program body
 * @return	Always returns 1
 */
void ci_init(void)
{
	Init_UART_PinMux();

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART, 115200);
	Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART);

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_USART, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UART0_IRQn, 1);
	NVIC_EnableIRQ(UART0_IRQn);

	/* Send initial messages */
	Chip_UART_SendRB(LPC_USART, &txring, start_msg, sizeof(start_msg) - 1);
}

void ci_run(void) {
	// TODO: Add authentication method
	if (authenticated) {
		read_command(command);
	}

}

void read_command(char* command) {
	uint8_t key;
	int bytes;
	bytes = Chip_UART_ReadRB(LPC_USART, &rxring, &key, 1);
	if (bytes > 0) {
		char c = key;
		if(c != '\n' && c != '\r') {
			command[index++] = c;
			if(index > MAX_COMMAND_LEN)
			{
				index = 0;
			}
		}
		if(c == '\r')
		{
			command[index] = '\0';
			index = 0;
			executeCommand(command);
		}
		/* Wrap value back around */
		if (Chip_UART_SendRB(LPC_USART, &txring, (const uint8_t *) &key, 1) != 1) {
			Board_LED_Toggle(0);/* Toggle LED if the TX FIFO is full */
		}
	}
}

void executeCommand(char *command, char *msg)
{
	uint8_t toggle_ack = 0;
    if (authenticated) {
		if(strcmp(command, "SEND") == 0)
		{
			Board_LED_Toggle(0); //TODO: add function call
			transfer_layer_send_message(msg, sizeof(msg) - 1);
			toggle_ack = 1;

		}
		else if(strcmp(command, "SEND10") == 0)
		{
			Board_LED_Toggle(0); //TODO: add function call
			for (int i = 0; i < 10; i++) {
				transfer_layer_send_message(msg, sizeof(msg) - 1);
			}
			toggle_ack = 1;
		}
		else if(strcmp(command, "CORRUPT") == 0)
		{
			Board_LED_Toggle(0); //TODO: add function call
			crc_set_offset(1);
			transfer_layer_send_message(msg, sizeof(msg) - 1);
			crc_set_offset(0);
			toggle_ack = 1;

		}
		else
		{
			Chip_UART_SendRB(LPC_USART, &txring, incorrect_msg, sizeof(incorrect_msg) - 1);
		}
    } else if(strcmp(command, "AUTH") != 0) {
		Chip_UART_SendRB(LPC_USART, &txring, unauthenticated_msg, sizeof(unauthenticated_msg) - 1);
	}
    if (authenticated == 0) {
		if(strcmp(command, "AUTH") == 0)
		{
			authenticate(msg);
			Board_LED_Toggle(0); //TODO: add function call
			toggle_ack = 1;

		}
	}
    if (toggle_ack) {
    	Chip_UART_SendRB(LPC_USART, &txring, ack_msg, sizeof(ack_msg) - 1);
    }
}

int ci_teardown(void) {
	/* DeInitialize UART0 peripheral */
	NVIC_DisableIRQ(UART0_IRQn);
	Chip_UART_DeInit(LPC_USART);

	return 1;
}

static void Init_UART_PinMux(void)
{
#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, IOCON_FUNC1 | IOCON_MODE_INACT);	/* PIO0_18 used for RXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, IOCON_FUNC1 | IOCON_MODE_INACT);	/* PIO0_19 used for TXD */
#elif (defined(BOARD_NXP_XPRESSO_11C24) || defined(BOARD_MCORE48_1125))
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));/* RXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));/* TXD */
#else
#error "No Pin muxing defined for UART operation"
#endif
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	UART interrupt handler using ring buffers
 * @return	Nothing
 */
void UART_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */

	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(LPC_USART, &rxring, &txring);
}

void authenticate(char* pw) {
	unsigned long hashed = 402054200; // password
	unsigned long t = hash(pw);
	if (t == hashed) {
		authenticated = 1;
    	Chip_UART_SendRB(LPC_USART, &txring, authenticated_msg, sizeof(authenticated_msg) - 1);
	} else {
    	Chip_UART_SendRB(LPC_USART, &txring, wrong_pw_msg, sizeof(wrong_pw_msg) - 1);
	}
}

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
