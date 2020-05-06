/*
 * NixPiZero.h
 *
 * Created: 2020/04/28 19:22:47
 * Author : jp7dki
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "NixPiZero.h"

//---- Variable ----
uint8_t dispdigit;
uint8_t dispnum[4];
uint8_t next_num[4];
uint8_t dispdigit = 0;

uint8_t change_num=0;			// Cross-Fade flag
uint16_t nixie_duty = NIXIE_DUTY_MAX_1;	// Duty
uint16_t next_nixie_duty;

uint8_t received_reg_addr;
uint8_t received_data;
uint8_t state_count;

//---- Register ----
uint8_t status = 0x00;				// STATUS register
uint8_t digit[4];					// DIGIT register

//---- Interrupt Vectors ----
/*
 * ISR(TIMER1_CAPT_vect)
 * Timer1 Input-capture interrupt
 */
ISR(TIMER1_CAPT_vect)
{
	uint8_t i;
	
	sei();			// Enable interrupt
	
	dispdigit++;
	if(dispdigit > 3){
		dispdigit=0;
		if(change_num==1){
			nixie_duty -= NIXIE_FADE_DELTA_1;
	
			if(nixie_duty < 1000){
				nixie_duty = next_nixie_duty;
				change_num = 0;
				for(i=0;i<4;i++){
					dispnum[i] = next_num[i];
				}
			}
		}
	}

	dispNumber(dispdigit, dispnum[dispdigit]);
	
	if(change_num==1  && ((next_num[dispdigit]^dispnum[dispdigit])!=0)){
		OCR1B = nixie_duty;
	}else{
		OCR1B = next_nixie_duty;
	}
}

/*
 * ISR(TIMER1_COMPA_vect)
 * Timer1 Compare-match A interrupt
 */
ISR(TIMER1_COMPA_vect)
{
	sei();
	
	dispNumber(A_ALL_OFF,K_ALL_ON);			// Nixie-Tube OFF (all cathode on)
}

/*
 * ISR(TIMER1_COMPB_vect)
 * Timer1 Compare-match B interrupt
 */
ISR(TIMER1_COMPB_vect)
{
	sei();
	
	if(change_num == 1){
		if((next_num[dispdigit]^dispnum[dispdigit]) != 0){
			dispNumber(dispdigit, next_num[dispdigit]);
		}
	}
}

/*
 * ISR(TWI_vect)
 * TWI Interrupt
 */
ISR(TWI_vect)
{	
	switch(TWSR & 0xF8){
		// Master Write
		case 0x60:
		case 0x68:
		TWCR |= 1<<TWINT;
		state_count = 0;
		break;
		
		case 0x80:
		case 0x90:
		if(state_count == 0){
			received_reg_addr = TWDR;
			state_count++;
		}else{
			received_data = TWDR;
			write_register(received_reg_addr-1+state_count, received_data);
			state_count++;
		}
		TWCR |= 1<<TWINT;
		break;
		
		case 0xA0:
		TWCR  |= 1<<TWINT;
		break;
		
		// Master Read
		case 0xA8:
		case 0xB0:
		TWDR = read_register(received_reg_addr);
		TWCR |= 1<<TWINT;
		state_count = 0;
		break;
		
		case 0xB8:
		TWCR |= 1<<TWINT;
		break;
		
		case 0xC0:
		TWCR |= 1<<TWINT;
		break;
	
		default:
		break;
	}
}

/*
 * void initIO(void)
 * Initialize Input/Output
 */
void initIO(void)
{
	uint8_t i,j;
	
	for(i=0;i<4;i++){
		dispnum[i] = 0;
		next_num[i] = 0;
	}
	
	DDRB = 0b11101111;
	DDRC = 0b11111111;
	DDRD = 0b11111111;
	
	// all zero 
	PORTB = 0b00000000;
	PORTC = 0b00000000;
	PORTD = 0b00000000;
}

/*
 * void initTimer(void)
 * Initialize Timer Settingnext_nixie_duty
 */
void initTimer(void)
{
	// Timer1: Dynamic lighting
	TCCR1A = 0x00;			// WGM11:10 = 0b00 : CTC mode
	TCCR1B = 0x19;			// WGM13:12 = 0b11 : Fast CTC mode, CS12:10 = 0b001 : clkio/1
	TCCR1C = 0x00;
	TIMSK1 = 0x26;
	ICR1 = 25000;								// 25000 x 0.125us = 3.125ms
	OCR1A = NIXIE_DUTY_MAX_1;					// 21600 x 0.125us = 2.7ms (blank time 0.1ms)
	OCR1B = NIXIE_DUTY_MAX_1 - NIXIE_BLANK;		// 20800 x 0.125us = 2.6ms
	
	next_nixie_duty = NIXIE_DUTY_MAX_1 - NIXIE_BLANK;
}

/*
 * void initI2C(void)
 * Initialize I2C
 */
void initI2C(void)
{
	uint8_t i;
	for(i=0;i<4;i++) digit[i] = 0;
	TWAR = I2C_SLAVE_ADDR & 0xFE;			// no response General call
	TWCR = ((1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE));
}

/*
 * void dispNumber(uint8_t digit, uint8_t num)
 * Display Nixie-Tube
 */
void dispNumber(uint8_t digit, uint8_t num)
{
	// Set Anode 
	PORTD = PORTD&0xE1;
	switch(digit){
		case 0:
			PORTD |= 0x04;
			break;
		case 1:
			PORTD |= 0x02;
			break;
		case 2:
			PORTD |= 0x10;
			break;
		case 3:
			PORTD |= 0x08;
			break;
	}
	
	// Set Cathode
	PORTB &= 0xEC;
	PORTC &= 0xF0;
	PORTD &= 0x1E;
	switch(num){
		case 0:
			PORTD |= 0x01;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			PORTC |= 0x10 >> num;
			break;
		case 5:
		case 6:
			PORTB |= 0x02 >> (num-5);
			break;
		case 7:
		case 8:
		case 9:
			PORTD |= 0x80 >> (num-7);
			break;
		case K_ALL_ON:
			PORTB |= 0x03;
			PORTC |= 0x0F;
			PORTD |= 0xE1;
			break;
	}
}

/*
 * int8_t write_register(uint8_t addr, uint8_t data)
 * Write Register
 * Argument addr: Register Address
 * Argument data: Data
 * Return: result (NG:-1)
 */
int8_t write_register(uint8_t addr, uint8_t data)
{
	switch(addr)
	{
		case ADDR_STATUS:
		status = data;
		break;
		
		case ADDR_DIGIT1:
		digit[0] = data;
		break;
		
		case ADDR_DIGIT2:
		digit[1] = data;
		break;
		
		case ADDR_DIGIT3:
		digit[2] = data;
		break;
		
		case ADDR_DIGIT4:
		digit[3] = data;
		break;
		
		default:
		return -1;
	}
	return 0;
}

/*
 * uint8_t read_register(uint8_t addr)
 * Read Register
 * Argument addr: Register Address
 * Return: read result
 */
uint8_t read_register(uint8_t addr)
{
	uint8_t return_val;
	switch(addr){
		case ADDR_STATUS:
		return_val = status;
		break;
		
		case ADDR_DIGIT1:
		return_val = digit[0];
		break;
		
		case ADDR_DIGIT2:
		return_val = digit[1];
		break;
		
		case ADDR_DIGIT3:
		return_val = digit[2];
		break;
		
		case ADDR_DIGIT4:
		return_val = digit[3];
		break;
		
		case ADDR_ID:
		return_val = 0x33;
		break;
		
		default:
		return_val = 0xFF;
		break;
	}
	return return_val;
}

