/*
 * NixPiZero.h
 *
 * Created: 2020/04/28 19:22:47
 * Author : jp7dki
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "NixPiZero.h"

//---- グローバル変数定義 ----
uint8_t dispdigit;
uint8_t dispnum[4];
uint8_t next_num[4];
uint8_t dispdigit = 0;

uint8_t change_num=0;			// 数値変更フラグ
uint16_t nixie_duty = NIXIE_DUTY_MAX_1;	// ニキシー管点灯デューティ比
uint16_t next_nixie_duty;

/*
 * void initIO (void)
 * Initialize IO Ports
 */
//---- 割り込み処理 ----
// タイマ1 インプットキャプチャ割り込み(ニキシー管点灯処理)
ISR(TIMER1_CAPT_vect)
{
	uint8_t i;
	
	// 割り込み許可
	sei();
	
	// 表示桁を変更：ダイナミック点灯
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

	// 表示桁に数値データを表示
	dispNumber(dispdigit, dispnum[dispdigit]);
	
	if(change_num==1  && ((next_num[dispdigit]^dispnum[dispdigit])!=0)){
		OCR1B = nixie_duty;
	}else{
		OCR1B = next_nixie_duty;
	}
}

// タイマ1 コンペアマッチA割り込み(ダイナミック点灯ブランク時間処理)
ISR(TIMER1_COMPA_vect)
{
	sei();
	// ニキシー管を消灯(カソードはオン：ゴースト対策)
	dispNumber(A_ALL_OFF,K_ALL_ON);
}

// タイマ1 コンペアマッチB割り込み(明るさ調整用ブランク処理・クロスフェード処理)
ISR(TIMER1_COMPB_vect)
{
	sei();
	
	// クロスフェードの場合、表示桁に数値データを表示
	if(change_num == 1){
		if((next_num[dispdigit]^dispnum[dispdigit]) != 0){
			dispNumber(dispdigit, next_num[dispdigit]);
		}
	}
}


void initIO(void)
{
	uint8_t i,j;
	
	// 配列変数の初期化
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

