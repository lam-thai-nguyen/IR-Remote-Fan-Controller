char cmd;
bool power = 0;
bool is_rhythm = 0;
bool is_timer = 0;


void reset_state() {
  power = 0;
  is_rhythm = 0;
  is_timer = 0;
}


void get_cmd_and_send_to_slave() {
  // If a command is received
  uint32_t data = decode_NEC();
  if (data) {
    // Check if the data is ON/OFF (Panasonic: 0x43430E7F)
    if (data == 0xE31CFF00 && power == 0) {
      reset_state();
      power = 1;
      cmd = '1';
      Serial.write(cmd);
      delay(300);
    }
    else if (data == 0xE31CFF00 && power == 1) {
      reset_state();
      cmd = '2';
      Serial.write(cmd);
      delay(300);
    }

    // Check if the data is SPEED (Panasonic: 0x54511082)
    if (data == 0xE718FF00  && power == 1 && is_rhythm == 0) {
      cmd = '3';
      Serial.write(cmd);
      delay(300);
    }

    // Check if the data is RHYTHM (Panasonic: 0x705C5422)
    if (data == 0xA55AFF00 && power == 1 && is_rhythm == 0) {
      is_rhythm = 1;
      cmd = '4';
      Serial.write(cmd);
      delay(300);
    }
    else if (data == 0xA55AFF00 && power == 1 && is_rhythm == 1) {
      is_rhythm = 0;
      cmd = '5';
      Serial.write(cmd);
      delay(300);
    }

    // Check if the data is OSCIL. (Panasonic: 0xE6F839DE)
    if (data == 0xAD52FF00 && power == 1) {
      cmd = '6';
      Serial.write(cmd);
      delay(300);
    }

    // Check if the data is TIMER (Panasonic: 0x1E11AAFE)
    if (data == 0xF708FF00 && power == 1 && is_timer == 0) {
      is_timer = 1;
      cmd = '7';
      Serial.write(cmd);
      delay(300);
    }
    else if (data == 0xF708FF00 && power == 1 && is_timer == 1) {
      is_timer = 0;
      cmd = '8';
      Serial.write(cmd);
      delay(300);
    }
  }
}

uint32_t decode_NEC() {
  // When DATA is LOW
  if (digitalRead(IR_RECEIVER_PIN) == LOW) {
    // Measure 9ms LOW
    unsigned long startLow = micros();
    while (digitalRead(IR_RECEIVER_PIN) == LOW);
    unsigned long lowDuration = micros() - startLow;

    // Measure 4.5ms HIGH
    unsigned long startHigh = micros();
    while (digitalRead(IR_RECEIVER_PIN) == HIGH);
    unsigned long highDuration = micros() - startHigh;

    // Check for NEC header
    if (lowDuration > 8500 && lowDuration < 9500 &&
        highDuration > 4000 && highDuration < 5000) {

      uint32_t data = 0;

      for (int i = 0; i < 32; i++) {
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
        if (bitHighDuration > 1000) {
          data |= (1UL << i);
        }
      }

      return data;  // Return command byte as a simple example
    } else {
      return 0;  // Header not detected, return 0
    }
  } else {
    return 0; // Signal not LOW to start
  }
}
