#include "decode.h"

uint32_t decode_NEC() {
  // When DATA is LOW
  if (digitalRead(IR_RECEIVER_PIN) == LOW) 
  {
    // Measure 9ms LOW
    unsigned long startLow = micros();
    while (digitalRead(IR_RECEIVER_PIN) == LOW);
    unsigned long lowDuration = micros() - startLow;

    // Measure 4.5ms HIGH
    unsigned long startHigh = micros();
    while (digitalRead(IR_RECEIVER_PIN) == HIGH);
    unsigned long highDuration = micros() - startHigh;

    // Check for NEC header
    if (lowDuration > 8500 && lowDuration < 9500 && highDuration > 4000 && highDuration < 5000) 
    {
      uint32_t data = 0;

      for (int i = 0; i < 32; i++) 
      {
        // Wait for LOW pulse start
        while (digitalRead(IR_RECEIVER_PIN) == HIGH);  // wait for LOW

        unsigned long startBitLow = micros();
        while (digitalRead(IR_RECEIVER_PIN) == LOW);   // measure LOW duration
        unsigned long bitLowDuration = micros() - startBitLow;

        // Wait for HIGH pulse to decode bit value
        unsigned long startBitHigh = micros();
        while (digitalRead(IR_RECEIVER_PIN) == HIGH);
        unsigned long bitHighDuration = micros() - startBitHigh;

        // NEC bit: 1 if HIGH duration > ~1.5ms, else 0
        if (bitHighDuration > 1000) 
        {
          data |= (1UL << i);
        }
      }

      return data;  // Return command byte as a simple example
    } else 
    {
      return 0;  // Header not detected, return 0
    }
   } 
  else 
  {
    return 0; // Signal not LOW to start
  }
}
