/*
 * NixPiZero.c
 *
 * Created: 2020/04/28 19:22:47
 * Author : jp7dki
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "NixPiZero.h"

//---- グローバル変数定義 ----
extern uint8_t dispdigit;
extern uint8_t dispnum[4];
extern uint8_t next_num[4];
extern uint8_t dispdigit;
extern uint8_t change_num;


int main(void)
{
	uint8_t i,j;
	
	initIO();
	initTimer();
	
	// 割り込み許可
	sei();
	
    while (1) 
    {
		for(i=0;i<4;i++){
			for(j=0;j<10;j++){
				next_num[i] = j;
				change_num = 1;
				_delay_ms(500);
			}
		}
    }
}

