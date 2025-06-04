#include "MyLCD_noI2C.h"

void mylcd_pulseEnable(void)
{
    MYLCD_PORT |= (1 << MYLCD_E);
    _delay_us(1);
    MYLCD_PORT &= ~(1 << MYLCD_E);
    _delay_us(100);
}

void mylcd_write4Bits(uint8_t fourbits)
{
    MYLCD_PORT &= 0x03;
    MYLCD_PORT |= (fourbits & 0x0F) << 2;
    mylcd_pulseEnable();
}

void mylcd_writeByte(uint8_t byte, uint8_t hey_data)
{
    if (hey_data)
    {
        MYLCD_PORT |= (1 << MYLCD_RS);
    }
    else
    {
        MYLCD_PORT &= ~(1 << MYLCD_RS);
    }
    mylcd_write4Bits(byte >> 4);
    mylcd_write4Bits(byte & 0x0F);
    _delay_us(100);
}

void mylcd_init(void)
{
    MYLCD_DDR |= (1 << MYLCD_RS) | (1 << MYLCD_E) | (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);
    _delay_ms(20);

    mylcd_write4Bits(0x03);
    _delay_ms(5);
    mylcd_write4Bits(0x03);
    _delay_us(150);
    mylcd_write4Bits(0x03);
    mylcd_write4Bits(0x02);

    mylcd_command(0x28);
    mylcd_command(0x0C);
    mylcd_command(0x06);
    mylcd_clear();
}

void mylcd_string(const char *str)
{
    while (*str)
    {
        mylcd_data(*str++);
    }
}

void mylcd_cursor(uint8_t col, uint8_t row)
{
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    mylcd_command(0x80 | (addr + col));
}

void mylcd_clear(void)
{
    mylcd_command(0x01);
    _delay_ms(2);
}

void mylcd_command(uint8_t cmd)
{
    mylcd_writeByte(cmd, 0);
}

void mylcd_data(uint8_t data)
{
    mylcd_writeByte(data, 1);
}

void hex2string(unsigned long value, char *str_map)
{
    const char hex_chars[] = "0123456789ABCDEF";
    str_map[0] = '0';
    str_map[1] = 'x';

    for (int i = 0; i < 8; i++)
    {
        int dichbit = (7 - i) * 4;
        str_map[i + 2] = hex_chars[(value >> dichbit) & 0xF];
    }

    str_map[10] = '\0';
}