; AssemblerApplication1.asm
; Author : gustin
;
; SEGMENT
; .dseg  - data segment
; .ESEG  - eeprom segment
; .cseg  - code segment
;
;
; .org  - originate (where segment starts )
;
; DEF - Set a symbolic name on a register
; EQU - Set a symbol equal to an expression
; SET - Set a symbol equal to an expression
;
; ================================================
; PINOUT ATMega 328P
; =========================================================================
;           TIME  ADC  FUNC   PO               PO  FUNC ADC TIME
;                                  ATMega328P
;                                 ____   ____
;                      _____     |   |__|    |   
;                      RESET  PC6| 1      28 | PC5 SCL ADC5
;                        RXD  PD0| 2      27 | PC4 SDA ADC4
;                        TXD  PD1| 3      26 | PC3     ADC3
;                       INT0  PD2| 4      25 | PC2     ADC2
;                       INT1  PD3| 5      24 | PC1     ADC1
;                             PD4| 6      23 | PC0     ADC0
;                        VCC     | 7      22 |     GND 
;                        GND     | 8      21 |     AREF 
;                      XTAL1  PB6| 9      20 |     AVCC  
;                      XTAL2  PB7| 10     19 | PB5 SCK  
;            OC0B             PD5| 11     18 | PB4 MISO 
;            OC0A AIN0        PD6| 12     17 | PB3 MOSI 
;                 AIN1        PD7| 13     16 | PB2          OC1B 
;            ICP1       CLKO  PB0| 14     15 | PB1          OC1A 
;                                |___________|   
;
;============================================================================
; **********************************
;  P O R T S   A N D   P I N S
; **********************************
.nolist
.include "m328pdef.inc" ; Define device ATmega328P
.list  
;
.equ debug = 0 ; for conditional ASM directives
;
; ================================================
; Hardware depending stuff
; ================================================
;
.equ cFreq = 16000000 ; Clock frequency processor in cycles/s
;
; **********************************
;       R E G I S T E R S
; **********************************
;
; free: R1 to R14
; free: R17 to R29
.def RSREG = R15 ; Save/Restore status port
.def RMP = R16 ; Define multipurpose register

.SET cPre2 = (1<<CS21) ; counter prescaler = 8
.IF cFreq>2040000
	.SET cCmp2 = cFreq/32000
	.SET cPre2 = (1<<CS21)|(1<<CS20) ; counter prescaler = 32
.ENDIF

; **********************************
;           S R A M	 (reserve variables in SRAM)
; **********************************
;
.dseg
.org SRAM_START
sLabel1:
	.byte 16 ; Reserve 16 bytes
sTMeas: ; ms per measuring interval (default: 250)
	.byte 1 ; reserve 1 byte
table:
	.DB 0b00111111, 0b00000100, 0b01011011
spi7SEG:
	.byte 0b00000100 ; 7 segment number 1
; **********************************
;           EEPROM	 (reserve variables in EEPROM)
; **********************************
.ESEG 
const2: .DB 1,2,3
;
;
; **********************************
;         C O D E
; **********************************
;
.cseg
.org 0x0000
;
; **********************************
; R E S E T  &  I N T - V E C T O R S
; in Application Space - start of the Flash memory.
; Each Interrupt Vector occupies two instruction words 
; Reset Vector is affected by the BOOTRST fuse, and the Interrupt Vector start address is affected by
; the IVSEL bit in MCUCR
; <0x0000> Starts here in flash (could be moved to bootloader section)
; **********************************
;   Priority HIGHEST to LOWEST
;   COMMAND   ; address NAME description
	rjmp RESET ; <0x0000> RESET		External Pin Reset, Power-on Reset, Brown-out Reset and Watchdog System Reset
	rjmp INT0_INT ; <0x0002> INT0	External Interrupt Request 0
	reti ; <0x0004> INT1			External Interrupt Request 0
	reti ; <0x0006> PCINT0			Pin Change Interrupt Request 0
	reti ; <0x0008> PCINT1			Pin Change Interrupt Request 1
	reti ; <0x000A> PCINT2			Pin Change Interrupt Request 2
	reti ; <0x000C> WDT				Watchdog Time-out Interrupt
	reti ; <0x000E> TIMER2_COMPA	OC2A Timer/Counter2 Compare Match A
	reti ; <0x0010> TIMER2_COMPB	OC2B Timer/Coutner2 Compare Match B
	reti ; <0x0012> TIMER2_OVF		OVF2 Timer/Counter2 Overflow
	reti ; <0x0014> TIMER1_CAPT		ICP1 Timer/Counter1 Capture Event
	reti ; <0x0016> TIMER1_COMPA	OC1A Timer/Counter1 Compare Match A
	reti ; <0x0018> TIMER1_COMPB	OC1B Timer/Coutner1 Compare Match B
	reti ; <0x001A> TIMER1_OVF		OVF1 Timer/Counter1 Overflow
	reti ; <0x001C> TIMER0_COMPA	OC0A Timer/Counter0 Compare Match A
	reti ; <0x001E> TIMER0_COMPB	OC0B Timer/Coutner0 Compare Match B
	reti ; <0x0020> TIMER0_OVF		OVF0 Timer/Counter0 Overflow
	reti ; <0x0022> SPI STC			SPI Serial Transfer Complete
	reti ; <0x0024> USART_RX		URXC USART Rx Complete
	reti ; <0x0026> USART_UDRE		UDRE USART Data Register Empty
	reti ; <0x0028> USART_TX		UTXC USART Tx Complete
	reti ; <0x002A> ADC				ADC Conversion Complete
	reti ; <0x002C> EE READY		EEPROM Ready
	reti ; <0x002E> ANALOG COMP		ACI	Analog Comparator
	reti ; <0x0030> TWI				2-wire Serial Interface
	reti ; <0x0032> SPM READY		SPMR Store Program Memory Ready
;
; **********************************
;  I N T - S E R V I C E   R O U T .
; **********************************
;
; During interrupts and subroutine calls, the return address Program Counter (PC) is stored on the Stack.
; The Stack is effectively allocated in the general data SRAM. All user programs must initialize the SP in the
; Reset routine (before subroutines or interrupts are executed). The Stack Pointer (SP) is read/write
; accessible in the I/O space. The data SRAM can easily be accessed through the five different addressing
; modes supported in the AVR architecture.
;
RESET:
	;initialization of the stack pointer
	ldi r16, high(RAMEND) ; init stack pointer
	out SPH, r16 ; Set Stack Pointer to top of RAM
	ldi r16, low(RAMEND)
	out SPL, r16
	sbi EIMSK, INT0 ; enable INT0 pin
	sei ; Enable interrupts
	rjmp main
;
INT0_INT:
	in RSREG, SREG ; save SREG
	tst r20
    brne off_pin_10
    sbis PORTB, PB0 ; skip next instruction if bit is set
	rjmp set_pin_10
	out SREG, RSREG ; restore SREG
	reti
all_on:
    ldi r20, 1
	out SREG, RSREG ; restore SREG
    reti
all_off:
    ldi r20, 0
	out SREG, RSREG ; restore SREG
    reti
set_pin_10:
    sbi PORTB, PB0
	out SREG, RSREG ; restore SREG
    reti
off_pin_10:
    cbi PORTB, PB0
	out SREG, RSREG ; restore SREG
	reti 
;
; Main program start
main:
    SBI PORTB, PB0 ; The bit will be set to one
	in  r16, PORTB

	ldi r16, 0b00001111
	and r0, r16
	ldi ZL, low(table<<1)
	ldi ZH, high(table<<1)
	clr r1
	add ZL, r0
	adc ZH, r1
	lpm r17, Z
	
	;sleep;  interrupts do the rest
loop:
    rjmp loop
;