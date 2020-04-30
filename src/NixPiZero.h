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

//---- �萔��` ----
#define NIXIE_DUTY_MAX_1 21600	// �ő�f���[�e�B(�p�^�[��1:���邢�ꍇ)
#define NIXIE_BLANK 800		// �u�����N����(���Ƃ���)
#define NIXIE_FADE_DELTA_1	750	// �N���X�t�F�[�h�ω��萔(�p�^�[��1�F���邢�ꍇ)

/*
 * 
 */
void initIO(void);
void initTimer(void);
void dispNumber(uint8_t digit, uint8_t num);
