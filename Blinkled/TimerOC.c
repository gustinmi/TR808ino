#define F_CPU 16000000L
#define BAUD 9600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "uart.h"
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/eeprom.h>

typedef enum FM_PRESCALER {
	ONE,
	FIVE
} FM_PRESCALER;

volatile int prescaler = 0;
volatile FM_PRESCALER fmPreType = ONE;
unsigned int eeprom_address=46;
//int n EEMEM;
//uint8_t EEMEM NonVolatileChar = 8; // variable will be saved to eeprom
/**
		Clear timer registers, clear timer counter
		Set timer control registers	(mode, interrupt)
		Set output compare register 
		Enable interrupt
		Start timer / Select clock
*/
void setupTimer0()
{
	TCCR0A = 0; // clear Timer control H register
	TCCR0B = 0; // clear Timer control L register
	TCNT0 = 0; // clear timer0 value 
	
	TIMSK0 |= (1 << TOIE0); // enable overflow interrupt
	TCCR0B |= (1 << CS02); // 256 prescaler	   (CPU clock = 16MHZ / 256 = 62500 cycles / second = 16us clock)
	
}

ISR(TIMER0_OVF_vect)  // overflow interrupt handler called by system
{
	prescaler++;				 
}


int main(void)
{
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;
	DDRB = (1 << 5); //output direction 5th pin 
	
	setupTimer0();
	sei(); // enable system interrupts
	
	int fmOneSecond = 0; // counter for blinking 1x per second ( ON/OF) = 
	int fmFiveSecond = 0; // counter for blinking 5x per second
	
	for (;;) /* loop forever, the interrupts are doing the rest */
	{
		uint8_t	tempVal;
		if (fmPreType == ONE){ // 1x second	ON / OFF
			if (prescaler >= 125) {	
				PORTB ^= (1 << 5); // toggles / flip pin
				prescaler = 0; // clear prescaler
				fmOneSecond++;
				if (fmOneSecond>=2){
					fmPreType = FIVE;
					enToStr(fmPreType, &tempVal);
					eeprom_write_byte((uint8_t*)eeprom_address, tempVal);
					puts("eeprom write" );
					//eeprom_update_byte(&n, 2);
					fmOneSecond = 0;
				}
		
				//puts("Changed to 5");	
			}
		} 
		else if (fmPreType == FIVE){
			if (prescaler >= 25) {				  // 5x second	ON / OFF
				PORTB ^= (1 << 5); // toggles / flip pin
				prescaler = 0; // clear prescaler
				fmFiveSecond++;
				if (fmFiveSecond >= 10){
					fmPreType = ONE;	
					enToStr(fmPreType, &tempVal);
					eeprom_write_byte ((uint8_t*)eeprom_address,  tempVal);
					puts("eeprom write  XXX" );
					fmFiveSecond = 0;
				}
			}
		}
		if (prescaler >= 20) {	   /* preber na vsake toliko */
			char buffer [10];
			
			
			
			uint8_t stanje =  eeprom_read_byte ((uint8_t*)eeprom_address);
			if (stanje != 255){
				puts("Prebran eeprom: " );
				puts(itoa(stanje, buffer, 10));	//itoa(buffer, stanje, 10)	
			}
			
		}
		
	}
	
	return (0);
}

void timer1()
{
	cli(); /* atomic read / write to 16 bit registers - Disable interrupts because 16bit access of timer 1 registers */
	// Clear timer counter, timner control register
	TCCR1A = 0; // clear Timer control H register
	TCCR1B = 0; // clear Timer control L register
	TCNT1 = 0;	// clear timer value

	TCCR1B |= (1 << WGM12); // CTC waveform mode
	OCR1A = 16000000L/256/2; // prefill compare match register for 2HZ (16MHz/256/2Hz)
	TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
	
	// Start timer !!
	TCCR1B |= (1 << CS12);  // 256 prescaler - Clock select bit. Timer clock = 62500 ticks / second
	sei(); /* enable interrupts */	
}

void enToStr(FM_PRESCALER en1, uint8_t* val){
	
	switch(en1)
	{
 		case ONE  : *val = 1;
 		case FIVE : *val = 5;
	}
	return;
}


ISR(TIMER1_COMPA_vect) // timer1 compare interrupt service routine
{
	PORTB ^= (1 << 5); // flip pin
}								  