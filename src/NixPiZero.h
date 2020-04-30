/*
 * NixPiZero.h
 *
 * Created: 2020/04/28 19:22:47
 * Author : jp7dki
 */

#include <avr/io.h>

/*
 * Define
 */
#define A_ALL_OFF 5
#define K_ALL_ON 10

//---- 定数定義 ----
#define NIXIE_DUTY_MAX_1 21600	// 最大デューティ(パターン1:明るい場合)
#define NIXIE_BLANK 800		// ブランク時間(一定とする)
#define NIXIE_FADE_DELTA_1	750	// クロスフェード変化定数(パターン1：明るい場合)

/*
 * 
 */
void initIO(void);
void initTimer(void);
void dispNumber(uint8_t digit, uint8_t num);
