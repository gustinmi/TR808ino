#include <avr/io.h>
#include <stdio.h>
#include <avr/sfr_defs.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 9600
#endif
#include <util/setbaud.h>

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

void uart_init(void) {

	// use macros to set baud rate in USART Baud Rate 0 Register Low and High byte
	UBRR0H = UBRRH_VALUE; // UART speed high.
	UBRR0L = UBRRL_VALUE; // UART speed low.

	// FROM IO PORTS : configuring UART register
	//#define UCSR0A _SFR_MEM8(0xC0)
	//#define MPCM0 0
	//#define U2X0 1
	//#define UPE0 2
	//#define DOR0 3
	//#define FE0 4
	//#define UDRE0 5
	//#define TXC0 6
	//#define RXC0 7

	
	#if USE_2X  // if UART has to be configured to run in double speed mode with given baud rate.
	UCSR0A |= _BV(U2X0);
	#else
	UCSR0A &= ~(_BV(U2X0));
	#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* data size 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(char c, FILE *stream) {
	if (c == '\n') {
		uart_putchar('\r', stream);
	}
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

char uart_getchar(FILE *stream) {
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}

/* From book: Make: Avr programming */

void transmitByte(uint8_t data) {
	/* Wait for empty transmit buffer */
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = data; /* send data */
}
uint8_t receiveByte(void) {
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait for incoming data */
	return UDR0; /* return register value */
}

// Example of a useful printing command
/*
		printString("==[ Cap Sensor ]==\r\n\r\n");
*/
void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		transmitByte(myString[i]);
		i++;
	}
}

void printByte(uint8_t byte){
	/* Converts a byte to a string of decimal text, sends it */
	transmitByte('0'+ (byte/100)); /* Hundreds */
	transmitByte('0'+ ((byte/10) % 10)); /* Tens */
	transmitByte('0'+ (byte % 10)); /* Ones */
}

void printBinaryByte(uint8_t byte){
	/* Prints out a byte as a series of 1's and 0's */
	uint8_t bit;
	for (bit=7; bit < 255; bit--){
		if ( bit_is_set(byte, bit) )
		transmitByte('1');
		else
		transmitByte('0');
	}
}
