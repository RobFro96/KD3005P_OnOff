#include <msp430.h>
#include <stdint.h>
#include "uart.h"

extern uint8_t uart_flags = 0;

// Lokale Variablen
// Ausgangsbuffer
static char tx_buffer[10];

// Groesse des Ausgangsbuffer
static uint8_t tx_buffer_len;

// Anzahl der gesendeten Bytes
static volatile uint8_t tx_counter;

// Initialisieren des UART-Moduls
void uart_init() {
	// Verbinden der beiden Pins mit der seriellen Schnittstelle
	P1SEL |= BIT1 + BIT2;
	P1SEL2 |= BIT1 + BIT2;

	// Reset waehrend der Konfiguaration der seriellen Schnittstelle
	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 |= UCSSEL_2; // SMCLK

	// 1 Mhz, 9600 Baud
	UCA0BR0 = 0x68;
	UCA0BR1 = 0x00;
	UCA0MCTL = 0x04;

	// Loslassen des Resets
	UCA0CTL1 &= ~UCSWRST;
}

// Senden einer Zeichenkette ueber die serielle Schnittstelle
// char data[] : zu sendende Zeichenkette
// uint8_t len : Anzahl der zu sendenden Bytes
void uart_tx_start(char data[], uint8_t len) {
	// Ueberpruefen der Laenge
	if (len == 0)
		return;

	// Uerbernehmen der Laenge
	tx_buffer_len = len;

	// Kopieren in Zeichenkette in den Buffer
	uint8_t i = 0;
	for (; i < tx_buffer_len; i++) {
		tx_buffer[i] = data[i];
	}

	// Zuruecksetzen des Zaehlers
	tx_counter = 0;

	// Freigeben des Interrupts
	UC0IE |= UCA0TXIE;
	IFG2 &= ~UCA0TXIFG;

	uart_flags &= ~ UART_TXD_FINISHED;

	// Senden des ersten Bytes
	UCA0TXBUF = tx_buffer[tx_counter++];
}

// ISR bei Datenausgang
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
	// Ueberpruefen der Interrupt-Flag
	if (IFG2 & UCA0TXIFG) {
		IFG2 &= ~UCA0TXIFG;
		if (tx_counter >= tx_buffer_len) {
			// Ende der Nachricht, Blockieren des Interrupts
			UC0IE &= ~UCA0TXIE;
			uart_flags |= UART_TXD_FINISHED;
		} else {
			// Naechstes Byte senden
			UCA0TXBUF = tx_buffer[tx_counter++];
		}
	}
}
