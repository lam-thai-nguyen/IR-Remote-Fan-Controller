#include "ir_send.h"

// Biến toàn cục để lưu trạng thái Timer0
uint8_t saved_TCCR0A;
uint8_t saved_TCCR0B;
uint8_t saved_OCR0A;
uint8_t saved_TIMSK0;

void IR_init(void)
{
    // Set PD6 (OC0A) is output
    IR_SEND_DDR |= (1 << IR_SEND_PIN);
}

// save current Timer0 state
void saveTimer0State(void)
{
    saved_TCCR0A = TCCR0A;
    saved_TCCR0B = TCCR0B;
    saved_OCR0A = OCR0A;
    saved_TIMSK0 = TIMSK0;
}

// Restore Timer0 state
void restoreTimer0State(void)
{
    TCCR0A = saved_TCCR0A;
    TCCR0B = saved_TCCR0B;
    OCR0A = saved_OCR0A;
    TIMSK0 = saved_TIMSK0;
}

// Config the waveform 38kHz using PWM from Timer0
void setupCarrier38kHz(void)
{
    // Fast PWM, Toggle OC0A, CTC mode (WGM01)
    TCCR0A = (1 << COM0A0) | (1 << WGM01);
    TCCR0B = (1 << CS00);            // no prescaler
    OCR0A = (F_CPU / 38000 / 2) - 1; // Tần số ~38kHz
}

// Turn on PWM (sóng mang)
void IR_enableCarrier(void)
{
    TCCR0A |= (1 << COM0A0); // Enable toggle OC0A
}

// Turn off PWM
void IR_disableCarrier(void)
{
    TCCR0A &= ~(1 << COM0A0);            // Tắt kết nối OC0A
    IR_SEND_PORT &= ~(1 << IR_SEND_PIN); // Đảm bảo chân xuống LOW
}

// Turn on waveform in `time_us`
void IR_mark(uint16_t time_us)
{
    IR_enableCarrier();
    while (time_us > 0)
    {
        _delay_us(10);
        time_us -= 10;
    }
}

// Turn off waveform in `time_us`
void IR_space(uint16_t time_us)
{
    IR_disableCarrier();
    while (time_us > 0)
    {
        _delay_us(10);
        time_us -= 10;
    }
}

// Send NEC
void IR_sendNEC(uint32_t code, uint8_t nbits)
{
    // Lưu trạng thái timer hiện tại
    saveTimer0State();

    // Thiết lập PWM 38kHz
    setupCarrier38kHz();

    // --- Gửi Header ---
    IR_mark(9000);  // 9ms burst
    IR_space(4500); // 4.5ms space

    // --- Gửi từng bit ---
    for (int i = nbits - 1; i >= 0; i--)
    {
        if (code & 0b1)
        {
            IR_mark(560);   // Bit '1': 560us mark
            IR_space(1690); //         1690us space
        }
        else
        {
            IR_mark(560);  // Bit '0': 560us mark
            IR_space(560); //         560us space
        }
        code >>= 1; // Shift bit tiếp theo lên MSB
    }

    // --- Gửi stop bit ---
    IR_mark(560);
    IR_disableCarrier();

    // Khôi phục trạng thái timer ban đầu
    restoreTimer0State();
}
