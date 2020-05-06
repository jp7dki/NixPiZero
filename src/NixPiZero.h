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

// Nixie-tube
#define NIXIE_DUTY_MAX_1 21600		// Maximum Duty
#define NIXIE_BLANK 800				// Blank time
#define NIXIE_FADE_DELTA_1	750		// Cross-fade delta
#define A_ALL_OFF 5
#define K_ALL_ON 10

// I2C
#define I2C_SLAVE_ADDR (0x12<<1)	// Slave Address(0x12)
#define I2C_SLAVE_SLAW 0x60			// Slave Address + W received
#define I2C_SLAVE_DATA 0x80			// Data received
#define I2C_SLAVE_STOP 0xA0			// Stop bit received
#define I2C_SLAVE_SLAR 0xA8			// Slave Address + R received
#define I2C_SLAVE_DACK 0xB8			// Data acknowledge received

// State machine
#define STATE_LISTEN 0x00
#define STATE_SLA_RECEIVED 0x01
#define STATE_REGADDR_RECEIVED 0x02
#define STATE_REGDATA_RECEIVED 0x03

// Register address
#define ADDR_STATUS 0x01
#define ADDR_ID 0x0F
#define ADDR_DIGIT1 0x11
#define ADDR_DIGIT2 0x12
#define ADDR_DIGIT3 0x13
#define ADDR_DIGIT4 0x14

/*
 * function prototype
 */
void initIO(void);
void initTimer(void);
void initI2C(void);
uint8_t I2C_Slave_Receive(void);
int8_t I2C_Slave_Transmit(uint8_t data);
uint8_t I2C_Slave_Listen(void);
void dispNumber(uint8_t digit, uint8_t num);
int8_t write_register(uint8_t addr, uint8_t data);
uint8_t read_register(uint8_t addr);
