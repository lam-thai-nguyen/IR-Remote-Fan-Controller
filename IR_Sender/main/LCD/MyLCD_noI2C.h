/*
 * MyLCD_noI2C.h
 *
 * Created: 4/16/2025 10:37:52 AM
 *  Author: Vanhhh
 */

#ifndef MYLCD_NOI2C_H_
#define MYLCD_NOI2C_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define MYLCD_PORT PORTC
#define MYLCD_DDR DDRC

#define MYLCD_RS PC0
#define MYLCD_E PC1

#define MYLCD_D4 PC2
#define MYLCD_D5 PC3
#define MYLCD_D6 PC4
#define MYLCD_D7 PC5

void mylcd_pulseEnable(void);

void mylcd_write4Bits(uint8_t fourbits);

void mylcd_writeByte(uint8_t byte, uint8_t hey_data);

void mylcd_init();

void mylcd_string(const char *str);

void mylcd_cursor(uint8_t col, uint8_t row);

void mylcd_clear(void);

void mylcd_command(uint8_t cmd);

void mylcd_data(uint8_t data);

void hex2string(unsigned long value, char *str_map);

#endif /* MYLCD_NOI2C_H_ */
