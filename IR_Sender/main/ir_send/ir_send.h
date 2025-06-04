#ifndef IR_SEND_H
#define IR_SEND_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// PD6 = OC0A
#define IR_SEND_DDR   DDRD
#define IR_SEND_PORT  PORTD
#define IR_SEND_PIN   PD6

// Timer0 registor
extern uint8_t saved_TCCR0A;
extern uint8_t saved_TCCR0B;
extern uint8_t saved_OCR0A;
extern uint8_t saved_TIMSK0;

void IR_init(void);
void saveTimer0State(void);
void restoreTimer0State(void);
void setupCarrier38kHz(void);
void IR_enableCarrier(void);
void IR_disableCarrier(void);
void IR_mark(uint16_t time_us);
void IR_space(uint16_t time_us);
void IR_sendNEC(uint32_t code, uint8_t nbits);

#endif
