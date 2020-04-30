/*
 * NixPiZero.h
 *
 * Created: 2020/04/28 19:22:47
 * Author : jp7dki
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "NixPiZero.h"

//---- �O���[�o���ϐ���` ----
uint8_t dispdigit;
uint8_t dispnum[4];
uint8_t next_num[4];
uint8_t dispdigit = 0;

uint8_t change_num=0;			// ���l�ύX�t���O
uint16_t nixie_duty = NIXIE_DUTY_MAX_1;	// �j�L�V�[�Ǔ_���f���[�e�B��
uint16_t next_nixie_duty;

/*
 * void initIO (void)
 * Initialize IO Ports
 */
//---- ���荞�ݏ��� ----
// �^�C�}1 �C���v�b�g�L���v�`�����荞��(�j�L�V�[�Ǔ_������)
ISR(TIMER1_CAPT_vect)
{
	uint8_t i;
	
	// ���荞�݋���
	sei();
	
	// �\������ύX�F�_�C�i�~�b�N�_��
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

	// �\�����ɐ��l�f�[�^��\��
	dispNumber(dispdigit, dispnum[dispdigit]);
	
	if(change_num==1  && ((next_num[dispdigit]^dispnum[dispdigit])!=0)){
		OCR1B = nixie_duty;
	}else{
		OCR1B = next_nixie_duty;
	}
}

// �^�C�}1 �R���y�A�}�b�`A���荞��(�_�C�i�~�b�N�_���u�����N���ԏ���)
ISR(TIMER1_COMPA_vect)
{
	sei();
	// �j�L�V�[�ǂ�����(�J�\�[�h�̓I���F�S�[�X�g�΍�)
	dispNumber(A_ALL_OFF,K_ALL_ON);
}

// �^�C�}1 �R���y�A�}�b�`B���荞��(���邳�����p�u�����N�����E�N���X�t�F�[�h����)
ISR(TIMER1_COMPB_vect)
{
	sei();
	
	// �N���X�t�F�[�h�̏ꍇ�A�\�����ɐ��l�f�[�^��\��
	if(change_num == 1){
		if((next_num[dispdigit]^dispnum[dispdigit]) != 0){
			dispNumber(dispdigit, next_num[dispdigit]);
		}
	}
}


void initIO(void)
{
	uint8_t i,j;
	
	// �z��ϐ��̏�����
	for(i=0;i<4;i++){
		dispnum[i] = 0;
		next_num[i] = 0;
	}
	
	DDRB = 0b11101111;
	DDRC = 0b11111111;
	DDRD = 0b11111111;
	
	// all zero 
	PORTB = 0b00000100;
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

