#define F_CPU 16000000L // Specify oscillator frequency
#define BAUD 9600
#define BLINK_TIMES 1 // How many times to blink the built in led

//#include "uart.h"
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
//#include <util/setbaud.h>
//#include <stdio.h>
 
int main1(void)
{
    //uart_init();
    //stdout = &uart_output;
    //stdin  = &uart_input;
	//unsigned char i;
	
	/* Define pull-ups and set outputs high */
	/* Define directions for port pins */
	
	//PORTB = (1<<PB7)|(1<<PB6)|(1<<PB1)|(1<<PB0);
	//DDRB = (1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0);

	int count;
    //char input;
	
	// Each port pin consists of three register bits: DDxn, PORTxn, and PINxn.
	// The DDxn bit in the DDRx register selects the direction of this pin.
    // DDRB = 0b00010000; // configure pin 6 of PORTB as output (digital pin 17 on the Arduino) 1 output 0 input
	// ali šestnajstiško DDRB = 0x1F;

	// SEt direction == 1 = output (we are going to put pin to VCC = HIGH). So its output pin

    DDRB = (1 << 5); //(  0b 0010 0000 )
    
	for (count = 0; count < BLINK_TIMES; count++)
	{
		PORTB &= ~(1 << 5); // 0b00000000; //set to low
		
		_delay_ms(500);
		
		PORTB |= (1<<5); // 0b00100000; // set high
		
		_delay_ms(500);
	}

	// Finally turn off led
	PORTB &= ~(1 << 5); 

	// write and read from serial
	//puts("Hello world from C language!");
	//input = getchar();
	//printf("I just wrote %c\n", input);
    
	return 0;
}