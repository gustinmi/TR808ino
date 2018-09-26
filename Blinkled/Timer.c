/************************************************************************/
/* LED Blinking with timer counter value overflow interrupt             */
/************************************************************************/
//#define F_CPU 16000000L
//#define BAUD 9600
#define PRODUCTION 1

#define TIMER_ITR_VAL 15625 //31250 // 16000000L / 256 / 2  // CLKio / prescaler / changes per cycle

#include <avr/io.h>
#include <avr/interrupt.h>
//#include "uart.h"
//#include <util/delay.h>
//#include <util/setbaud.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/sleep.h>

char snum[16];

int main11(void)
{
	//uart_init();
	//stdout = &uart_output;
	//stdin  = &uart_input;
	
	DDRB = (1 << 5); //output direction 5th pin 
	//PORTB ^= (1 << 5); // flip pin
	
	cli(); /* Disable interrupts because 16bit access of timer 1 registers */
	
	// TIMER Control register TCCR1A TCCR1B
	//     A			B
	//     0000 0000	0000 0000
	TCCR1A = 0; // clear Timer control H register
	TCCR1B = 0; // clear Timer control L register
	
	// Set bit in interrupt register (ISR will clear the bit, when entered)	
	TIMSK1 |= (1 << TOIE1);   // this will triger Interrupt Service routine ISR for timer1
	
	// Preload counter with 2HZ per second value (na vsak clock se zmanjaš/poveèa. Ko doseže to cifro overflow, se resetira)
	// 16 000 000 / 256 / 4 (prižgi(ugasni*2/sec)
	// set the value you want to count to
	TCNT1 = TIMER_ITR_VAL;
	
	#ifdef DEBUG
	// write and read from serial
	//puts("Started");		
	#endif
	
	
	// Clock select bit. Timer clock = 62500 ticks / second
	// Prescaler clock selection - select the 256 prescaler in CS12 bit of Timer control B register
	// Frequency CPU = 16 000 000 / 256 = 62500 ticks per second . This will be "resolution" of one second of our timer
	// Method will strat timer !!
	TCCR1B |= (1 << CS12);
	
	sei(); /* enable interrupts back */
	 
	/*while (1)      // main loop (arduiono loop cpp method)
	{
		// app code of "LOOP"
	} */
	
	for (;;)                    /* loop forever, the interrupts are doing the rest */
		sleep_mode();

	return (0);
	
	
}

/*ISR(TIMER1_OVF_vect)        // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	//puts("Timer value ");
	//puts(itoa(OCR1A, snum, 10));
	
    PORTB ^= (1 << 5); // flip pin
} */