#ifndef UART_H_
#define UART_H_

#define UART_TXD_FINISHED BIT0

extern uint8_t uart_flags;

void uart_init();
void uart_tx_start(char data[], uint8_t len);

#endif /* UART_H_ */
