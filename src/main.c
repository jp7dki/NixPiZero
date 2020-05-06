/*
 * main.c
 *
 * Created: 2020/04/28 19:22:47
 * Author : jp7dki
 */ 

#define F_CPU 8000000UL		// CPU Freq = 8MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "NixPiZero.h"

//---- variable ----
extern uint8_t dispdigit;
extern uint8_t dispnum[4];
extern uint8_t next_num[4];
extern uint8_t dispdigit;
extern uint8_t change_num;
extern uint8_t status;
extern uint8_t digit[4];

/*
 * int main(void)
 * main function
 */
int main(void)
{
	uint8_t i;
	
	initIO();		// initialize IO ports
	initTimer();	// initialize Timer1
	initI2C();		// initialize TWI(I2C)
	
	sei();			// enable interrupt
	
    while (1) 
    {
		// loop
		// High-voltage ciruit switch
		if((status&0x01)!=0){
			PORTB |= 0x04;
		}else{
			PORTB &= ~0x04;
		}
		
		// Display refresh
		if((status&0x04)!=0){
			if((status&0x02)!=0){
				for(i=0;i<4;i++){
					next_num[i] = digit[i];
					change_num=1;
				}
			}else{
				for(i=0;i<4;i++){
					dispnum[i] = digit[i];
				}
			}
			status &= ~0x04;
		}
    }
	
	return 0;
}

