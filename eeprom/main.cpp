#include <avr/io.h>
#include <stdint.h>

#define MAX_EEPROM_CELL_NUM 4096
#define INS_TRACK_LOCATIONS 4
#define MAX_TRACK (4096 / 4)  // 1024
#define EEPROM_BUFF_SIZE 64 // EEPROM buffer size

typedef enum Switches {StepSwitch, FunctionSwitch, TempoSwitch, InstrumentSwitch, PartSwitch } Switches; // Hardware Zo Software VHDL mapping
typedef enum Part { 
	Basic = 0,
	First = 1,
	Second = 2
} Part; // AB,  A First part / B Second Part

typedef enum Accent { ACC, NOACC } Acc; // Is there an Accent on step switch
					  	
typedef enum Instrument { // Number of the instrument (voices gating)
	BASS = 0,
	COWBELL = 1,
	HIHAT = 2,
	SNARE =  3	
} Instrument;

typedef struct EE_Track { // For saving data in eeprom memory
	char metadata;
	uint16_t dataHigh;
	uint16_t dataLow;
} EE_Track;

typedef struct Ins_Pattern { // program type
	
	uint16_t eeAddress;
	
	Instrument instrument;
	Part patternPart;
	Acc accent;
	
	uint16_t dataHigh;
	uint16_t dataLow;
	
} Ins_Pattern;

/* Get the address location translation. Instrument tracks are laid down into eeprom */
uint16_t getEepromAddress(char trackNum, Instrument ins, Part part, Accent acc){
	

}



int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

