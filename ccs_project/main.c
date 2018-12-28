#include <msp430.h> 
#include <stdint.h>

#include "uart.h"

#define SWITCH BIT3

static uint8_t last_state = 0xff;

static void update_state();

/**
 * main.c
 */
int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	DCOCTL = CALDCO_1MHZ;
	BCSCTL1 = CALBC1_1MHZ;

	uart_init();

	P1DIR &= ~SWITCH;
	P1OUT |= SWITCH;
	P1REN |= SWITCH;

	__enable_interrupt();

	uart_tx_start("XXXX", 4);
	_delay_cycles(100000L);

	update_state();

	while (1) {
		while (!(uart_flags & UART_TXD_FINISHED))
			;
		update_state();
	}
}

static void update_state() {
	uint8_t current_state = (P1IN & SWITCH) ? 1 : 0;

	if (current_state != last_state) {
		last_state = current_state;

		if (current_state == 0) {
			uart_tx_start("OUT0", 4);
		} else {
			uart_tx_start("OUT1", 4);
		}
	}
}
